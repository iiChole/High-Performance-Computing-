#include "defs.h"

/* 分块大小 blk, 用 -DBLK=... 覆盖, 默认 16
 * 注意不能用 B: defs.h 里已有函数式宏 B(i,j) */
#ifndef BLK
#define BLK 16
#endif

#define MIN(a, b) ((a) < (b) ? (a) : (b))

/* C = A * B + C
 * 只把矩阵 C 的 i、j 外层按 blk 切成 blk×blk 子块, 让每个 C 块常驻缓存、提高复用率.
 * 内层仍是原始 i-j-p (点积), B 的列访问问题没有解决, 但 C 的访存局部性变好.
 */
void MY_MMult(int m, int n, int k, double *a, int lda,
              double *b, int ldb,
              double *c, int ldc)
{
  int i0, j0, i, j, p;
  for (i0 = 0; i0 < m; i0 += BLK)
    for (j0 = 0; j0 < n; j0 += BLK)
      for (i = i0; i < MIN(i0 + BLK, m); i++)
        for (j = j0; j < MIN(j0 + BLK, n); j++)
        {
          double cij = C(i, j);
          for (p = 0; p < k; p++)
            cij += A(i, p) * B(p, j);
          C(i, j) = cij;
        }
}
