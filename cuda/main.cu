
#include "utils.h"
#include <cuda.h>
#include <curand.h>
#include <curand_kernel.h>
#include <math.h> // smallpt, a Path Tracer by Kevin Beason, 2008
// #include <nvToolsExt.h>
#include <stdio.h>  //        Remove "-fopenmp" for g++ version < 4.2
#include <stdlib.h> // Make : g++ -O3 -fopenmp smallpt.cpp -o smallpt

struct Vec {        // Usage: time ./smallpt 5000 && xv image.ppm
    double x, y, z; // position, also color (r,g,b)
    __host__ __device__ Vec(double x_ = 0, double y_ = 0, double z_ = 0) {
        x = x_;
        y = y_;
        z = z_;
    }
    __host__ __device__ Vec operator+(const Vec &b) const {
        return Vec(x + b.x, y + b.y, z + b.z);
    }
    __host__ __device__ Vec operator-(const Vec &b) const {
        return Vec(x - b.x, y - b.y, z - b.z);
    }
    __host__ __device__ Vec operator*(double b) const {
        return Vec(x * b, y * b, z * b);
    }
    __host__ __device__ Vec mult(const Vec &b) const {
        return Vec(x * b.x, y * b.y, z * b.z);
    }
    __host__ __device__ Vec &norm() {
        return *this = *this * (1 / sqrt(x * x + y * y + z * z));
    }
    __host__ __device__ double dot(const Vec &b) const {
        return x * b.x + y * b.y + z * b.z;
    } // cross:
    __host__ __device__ Vec operator%(Vec &b) {
        return Vec(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x);
    }
};
struct Ray {
    Vec o, d;
    __host__ __device__ Ray(Vec o_, Vec d_) : o(o_), d(d_) {}
};
enum Refl_t {
    DIFF,
    SPEC,
    REFR
}; // material types, used in radiance(num_spheres, )
struct Sphere {
    float rad;   // radius
    Vec p, e, c; // position, emission, color
    Refl_t refl; // reflection type (DIFFuse, SPECular, REFRactive)
    Sphere() {}
    __host__ __device__ Sphere(float rad_, Vec p_, Vec e_, Vec c_, Refl_t refl_)
        : rad(rad_), p(p_), e(e_), c(c_), refl(refl_) {}
    __host__ __device__ double
    intersect(const Ray &r) const { // returns distance, 0 if nohit
        Vec op = p - r.o; // Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
        float eps = 1e-4;
        double t, b = op.dot(r.d), det = b * b - op.dot(op) + rad * rad;
        if (det < 0)
            return 0;
        else
            det = sqrt(det);
        return (t = b - det) > eps ? t : ((t = b + det) > eps ? t : 0);
    }
};

#define MAX_SPHERES 16
Sphere h_spheres[MAX_SPHERES];
int numOfSpheres = 0;
__constant__ int w = 1024;
__constant__ int h = 768;
__constant__ float M_PI = 3.14159265358979323846;

void readFromFile(const char *input) {
    FILE *fp;
    if ((fp = fopen(input, "r")) == NULL) {
        fprintf(stderr, "Fail to read from %s: No such file!\n", input);
        exit(1);
    }
    char buf[256];
    int len;
    char *p;
    double t[11];
    int j = 0;
    try {
        while (fgets(buf, 256, fp) != NULL) {
            int i = 0;

            len = strlen(buf);
            buf[len - 1] = '\0';
            p = strtok(buf, ", ");
            while (p != NULL && i < 11) {
                if (i != 10) {
                    t[i] = atof(p);
                } else {
                    if (strcmp(p, "DIFF") == 0) {
                        t[i] = DIFF;
                    } else if (strcmp(p, "SPEC") == 0) {
                        t[i] = SPEC;
                    } else if (strcmp(p, "REFR") == 0) {
                        t[i] = REFR;
                    } else {
                        throw;
                    }
                }
                p = strtok(NULL, ", ");
                i++;
            }
            if (i < 11)
                throw;
            if (numOfSpheres >= 16) {
                printf("Number of spheres exceeds the limit! MAX_SPHERES: %d\n",
                       MAX_SPHERES);
                exit(1);
            }
            h_spheres[j] =
                Sphere(t[0], Vec(t[1], t[2], t[3]), Vec(t[4], t[5], t[6]),
                       Vec(t[7], t[8], t[9]), (Refl_t)t[10]);
            j++;
            numOfSpheres++;
        }

    } catch (...) {
        printf("Fail to read from %s: Invalid syntax!\n", input);
        exit(1);
    }
}

__host__ __device__ inline float clamp(float x) {
    return x < 0 ? 0 : x > 1 ? 1 : x;
}
__host__ __device__ inline int toInt(float x) {
    return int(pow(clamp(x), 1 / 2.2) * 255 + .5);
}
__device__ inline bool intersect(const int num_spheres, const Sphere *spheres,
                                 const Ray &r, float &t, int &id) {
    float n = num_spheres, d, inf = t = 1e20;
    for (int i = int(n); i--;)
        if ((d = spheres[i].intersect(r)) && d < t) {
            t = d;
            id = i;
        }
    return t < inf;
}
__device__ Vec radiance(const int num_spheres, const Sphere *spheres,
                        const Ray _r, int _depth, curandState *state) {
    float t;    // distance to intersection
    int id = 0; // id of intersected object
    Ray r = _r;
    int depth = _depth;
    Vec cl(0, 0, 0); // accumulated color
    Vec cf(1, 1, 1); // accumulated reflectance
    while (1) {
        if (!intersect(num_spheres, spheres, r, t, id))
            return cl;                   // if miss, return black
        const Sphere &obj = spheres[id]; // the hit object
        Vec x = r.o + r.d * t, n = (x - obj.p).norm(),
            nl = n.dot(r.d) < 0 ? n : n * -1, f = obj.c;
        float p = f.x > f.y && f.x > f.z ? f.x
                  : f.y > f.z            ? f.y
                                         : f.z; // max refl
        cl = cl + cf.mult(obj.e);
        if (++depth > 5)
            if (curand_uniform(state) < p)
                f = f * (1 / p);
            else
                return cl; // R.R.
        cf = cf.mult(f);
        if (obj.refl == DIFF) { // Ideal DIFFUSE reflection
            double r1 = 2 * M_PI * curand_uniform(state),
                   r2 = curand_uniform(state), r2s = sqrt(r2);
            Vec w = nl, u = ((fabs(w.x) > .1 ? Vec(0, 1) : Vec(1)) % w).norm(),
                v = w % u;
            Vec d = (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2))
                        .norm();
            // return obj.e + f.mult(radiance(Ray(x,d),depth,Xi));
            r = Ray(x, d);
            continue;
        } else if (obj.refl == SPEC) { // Ideal SPECULAR reflection
            // return obj.e +
            // f.mult(radiance(Ray(x,r.d-n*2*n.dot(r.d)),depth,Xi));
            r = Ray(x, r.d - n * 2 * n.dot(r.d));
            continue;
        }
        Ray reflRay(x, r.d - n * 2 * n.dot(r.d)); // Ideal dielectric REFRACTION
        bool into = n.dot(nl) > 0;                // Ray from outside going in?
        int nc = 1;
        float nt = 1.5, nnt = into ? nc / nt : nt / nc, ddn = r.d.dot(nl),
              cos2t;
        if ((cos2t = 1 - nnt * nnt * (1 - ddn * ddn)) <
            0) { // Total internal reflection
            // return obj.e + f.mult(radiance(reflRay,depth,Xi));
            r = reflRay;
            continue;
        }
        Vec tdir =
            (r.d * nnt - n * ((into ? 1 : -1) * (ddn * nnt + sqrt(cos2t))))
                .norm();
        float a = nt - nc, b = nt + nc, R0 = a * a / (b * b),
              c = 1 - (into ? -ddn : tdir.dot(n));
        float Re = R0 + (1 - R0) * c * c * c * c * c, Tr = 1 - Re,
              P = .25f + .5f * Re, RP = Re / P, TP = Tr / (1 - P);
        // return obj.e + f.mult(erand48(Xi)<P ?
        //                       radiance(reflRay,    depth,Xi)*RP:
        //                       radiance(Ray(x,tdir),depth,Xi)*TP);
        if (curand_uniform(state) < P) {
            cf = cf * RP;
            r = reflRay;
        } else {
            cf = cf * TP;
            r = Ray(x, tdir);
        }
        continue;
    }
}

__global__ void render(const int num_spheres, const Sphere *spheres, Vec *c,
                       int samps) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    Ray cam(Vec(50, 52, 295.6), Vec(0, -0.042612, -1).norm()); // cam pos, dir
    Vec cx = Vec(w * .5135f / h), cy = (cx % cam.d).norm() * .5135f, r;

    if (y < h && x < w) {
        curandState state;
        curand_init(y * y * y, 0, 0, &state);

        for (int sy = 0, i = (h - y - 1) * w + x; sy < 2; sy++) {
            for (int sx = 0; sx < 2; sx++, r = Vec()) { // 2x2 subpixel cols
                for (int s = 0; s < samps; s++) {
                    float r1 = 2 * curand_uniform(&state),
                          dx = r1 < 1 ? sqrt(r1) - 1 : 1 - sqrt(2 - r1);
                    float r2 = 2 * curand_uniform(&state),
                          dy = r2 < 1 ? sqrt(r2) - 1 : 1 - sqrt(2 - r2);
                    Vec d = cx * (((sx + .5f + dx) / 2 + x) / w - .5f) +
                            cy * (((sy + .5f + dy) / 2 + y) / h - .5f) + cam.d;
                    r = r + radiance(num_spheres, spheres,
                                     Ray(cam.o + d * 140, d.norm()), 0,
                                     &state) *
                                (1. / samps);
                } // Camera rays are pushed ^^^^^ forward to start in interior
                c[i] = c[i] + Vec(clamp(r.x), clamp(r.y), clamp(r.z)) * .25f;
            }
        }
    }
}
int main(int argc, char *argv[]) {
    int w = 1024, h = 768,
        samps = argc >= 2 ? atoi(argv[1]) / 4 : 1; // # samples
    const char *filename = argc == 3 ? argv[2] : "input.txt";
    readFromFile(filename);
    timer_start("Getting GPU Data."); //@@ start a timer
    // CUDA memory allocation
    Sphere *spheres;
    Vec *h_c = new Vec[w * h];
    Vec *c;

    cudaMalloc((void **)&c, w * h * sizeof(Vec));
    cudaMalloc((void **)&spheres, sizeof(h_spheres));

    cudaMemset(c, 0, w * h * sizeof(Vec));
    cudaMemcpy(spheres, &h_spheres, numOfSpheres * sizeof(Sphere),
               cudaMemcpyHostToDevice);

    dim3 dimGrid(ceil((1.0 * w) / 32), ceil((1.0 * h) / 16), 1);
    dim3 dimBlock(32, 16, 1);

    render<<<dimGrid, dimBlock>>>(numOfSpheres, spheres, c, samps);
    cudaMemcpy(h_c, c, w * h * sizeof(Vec), cudaMemcpyDeviceToHost);

    cudaFree(c);
    cudaFree(spheres);

    timer_stop(); //@@ stop the timer

    FILE *f = fopen("image-cuda.ppm", "w"); // Write image to PPM file.
    fprintf(f, "P3\n%d %d\n%d\n", w, h, 255);
    for (int i = 0; i < w * h; i++)
        fprintf(f, "%d %d %d ", toInt(h_c[i].x), toInt(h_c[i].y),
                toInt(h_c[i].z));
}