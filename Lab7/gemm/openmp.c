#include "defs.h"

/*
*Routine for computing C = A * B + C, parallelized with OpenMP
*/

void MY_MMult(int m, int n, int k, double *a, int lda,
            double *b, int ldb,
            double *c, int ldc)
{
    int i, j, p;

    #pragma omp parallel for private(j, p)
        for (i = 0; i < m; i++){
            for (p = 0; p < k; p++){
                double a_ip;
                a_ip = A(i, p);
                for(j = 0; j < n; j ++){
                    C(i, j) += a_ip * B(p, j);
                }
            }
        }
    }
