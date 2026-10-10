#include "defs.h"
#include <arm_neon.h>      

void MY_MMult(int m, int n, int k, double *a, int lda,
              double *b, int ldb,
              double *c, int ldc)
{
  int i, j, p;
  for (i = 0; i < m; i++)
  {
    for (j = 0; j < n; j++)
    {
      double cij = C(i, j);
      float64x2_t c_v = vdupq_n_f64(0.0);

      for(p = 0; p + 1 < k; p += 2){
        float64x2_t a_v = vld1q_f64(&A(i, p));
        float64x2_t b_v = vld1q_lane_f64(&B(p, j), vdupq_n_f64(0.0), 0);
        b_v = vld1q_lane_f64(&B(p + 1, j), b_v, 1);
        c_v = vmlaq_f64(c_v, a_v, b_v);
      }
      cij += vgetq_lane_f64(c_v, 0) + vgetq_lane_f64(c_v, 1);

      for(; p < k; p++){
        cij += A(i, p) * B(p, j);
      }
      C(i, j) = cij;
    }
  }
}
