#include <stdlib.h>
#include <pthread.h>
#include "defs.h"

#define TILE 64

typedef struct 
{
    int m, n, k;
    double *a, *b, *c;
    int lda, ldb, ldc;
    int r0, r1;/* the row range*/
}gemm_arg_t;

static void *gemm_worker(void *arg)
{
    gemm_arg_t *p = (gemm_arg_t *)arg;
    int n = p->n, k = p->k;
    int lda = p->lda, ldb = p->ldb, ldc = p->ldc;
    double *a = p->a, *b = p->b, *c = p->c;
    int r0 = p->r0, r1 = p->r1;

    /*Divide jc, k by TILE */
    for(int jc = 0; jc < n; jc += TILE){
        int j1 = (jc + TILE < n) ? jc + TILE : n;
        for (int pc = 0; pc < k; pc += TILE){
            int p1 = (pc + TILE < k) ? pc +TILE : k;
            for(int i = r0; i < r1; i++){
                for(int p = pc; p < p1; p++){
                    double aip = A(i, p);
                    for(int j = jc; j < j1; j ++){
                        C(i, j) += aip * B(p, j);
                    }
                }
            }
        }
    }
    return NULL;
}

/*Initialize A, B, C, create childre thread & allocate task, caculate join*/
void MY_MMult(int m, int n, int k, double *a, int lda, 
                                    double *b, int ldb,
                                    double *c, int ldc)
{
    const char *env = getenv("GEMM_THREADS");
    int nthreads = (env ? atoi(env) : 1);
    if (nthreads < 1)
    nthreads = 1;
    if (nthreads > m)
    nthreads = m;/*no more than total rows*/

    pthread_t *threads = (pthread_t*)malloc((size_t)nthreads * sizeof(pthread_t));
    gemm_arg_t *args = (gemm_arg_t*)malloc((size_t)nthreads * sizeof(gemm_arg_t));
    
    /*Divide by rows*/
    int rows_per = m / nthreads;
    int rem = m % nthreads;
    for(int t = 0; t < nthreads; t++)
    {
        args[t].n = n;
        args[t].k = k;
        args[t].a = a;
        args[t].b = b;
        args[t].c = c;
        args[t].lda = lda;
        args[t].ldb = ldb;
        args[t].ldc = ldc;
        args[t].r0 = t * rows_per + (t < rem ? t : rem);
        args[t].r1 = args[t].r0 + rows_per + (t < rem ? 1 : 0);
        pthread_create(&threads[t], NULL, gemm_worker, &args[t]);
    }

    for (int t = 0; t < nthreads; t++)
        pthread_join(threads[t], NULL);

    free(threads);
    free(args);
}