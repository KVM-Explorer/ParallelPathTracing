#include <stdint.h>
#define FLOAT_TYPE_HALF
#ifdef FLOAT_TYPE_HALF
using FF = float;
#else
using FF = double;
#endif

const int32_t WIDTH = 16; // min 16 继续小可能涉及数据对齐问题，无法通过验证
const int32_t HEIGHT = 16; // min 16
const int32_t SAMPLES = 1; // SAMPLES * 4 = total samples

enum MaterialType { DIFF, SPEC, REFR };

const float PI = 3.1415926535897932385f;