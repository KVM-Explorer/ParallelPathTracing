
#include "utils/types.h"
#include "parallel/simd/utils/types.h"
#include <tuple>
namespace TypeConverter {

   SIMD::VecSSE convert(const Vec& a) {
      return SIMD::VecSSE(a.x, a.y, a.z);
   }

}