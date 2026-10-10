#include "defs.h"

/* C = A * B + C
 * 在 MMult0 (i-j-p) 基础上, 对最内层 p 循环做 8 路展开:
 *   一次循环体计算 8 个 p 的贡献, 减少循环分支开销, 给编译器更多可并行的 FMA.
 * 用局部累加器 cij 避免每步都读/写内存里的 C(i,j).
 */
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
      for (p = 0; p + 7 < k; p += 8)
      {
        cij += A(i, p + 0) * B(p + 0, j)
             + A(i, p + 1) * B(p + 1, j)
             + A(i, p + 2) * B(p + 2, j)
             + A(i, p + 3) * B(p + 3, j)
             + A(i, p + 4) * B(p + 4, j)
             + A(i, p + 5) * B(p + 5, j)
             + A(i, p + 6) * B(p + 6, j)
             + A(i, p + 7) * B(p + 7, j);
      }
      for (; p < k; p++)            /* 尾部不足 8 个 */
        cij += A(i, p) * B(p, j);
      C(i, j) = cij;
    }
  }
}
