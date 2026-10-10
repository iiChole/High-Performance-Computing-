#include "defs.h"
#include <arm_neon.h>      

#define MC 64    /*C/A Rows*/
#define KC 256   /*k blocks*/
#define NC 256   /*C/B Columns*/
#define MIN(a, b) ((a) < (b) ? (a) : (b))

void MY_MMult(int m, int n, int k, double *a, int lda, 
              double *b, int ldb,
              double *c, int ldc)
{
  int i0, p0, j0, i, j, p;
  for(i0 = 0; i0 < m; i0 += MC)
    for(p0 = 0; p0 < k; p0 += KC)
      for(j0 = 0; j0 < n; j0 += NC)
      {
        int jend = MIN(j0 + NC, n);
        for(i = i0; i < MIN(i0 + MC, m); i ++)
          for(p = p0; p < MIN(p0 + KC, k); p ++)
            {
              float64x2_t a_v = vdupq_n_f64(A(i, p));

              for(j = j0; j < jend; j += 2)
              {
                float64x2_t b_v = vld1q_f64(&B(p, j));
                float64x2_t c_v = vld1q_f64(&C(i, j));
                c_v = vfmaq_f64(c_v, a_v, b_v);
                vst1q_f64(&C(i, j), c_v);
              }
            }
      }
}