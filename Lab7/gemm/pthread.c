#include <pthread.h>
#include <stdlib.h>
#include "defs.h"

/* 与 OpenMP 版本相同的矩阵划分:只对最外层循环(矩阵 C 的行 i)按行划分给各线程 */

typedef struct {
    int start, end;   /* 本线程负责的行区间 [start, end) */
    int n, k;
    double *a, *b, *c;
    int lda, ldb, ldc;
} gemm_arg_t;

static void *gemm_rows(void *arg)
{
    gemm_arg_t *t = (gemm_arg_t *)arg;
    int i, j, p;
    for (i = t->start; i < t->end; i++) {
        for (p = 0; p < t->k; p++) {
            double a_ip = t->a[i * t->lda + p];
            for (j = 0; j < t->n; j++) {
                t->c[i * t->ldc + j] += a_ip * t->b[p * t->ldb + j];
            }
        }
    }
    return NULL;
}

void MY_MMult(int m, int n, int k, double *a, int lda,
              double *b, int ldb,
              double *c, int ldc)
{
    int nthreads = 4;
    const char *env = getenv("OMP_NUM_THREADS");
    if (env)
        nthreads = atoi(env);
    if (nthreads < 1)
        nthreads = 1;
    if (nthreads > m)
        nthreads = m;
    if (m <= 0)
        return;

    pthread_t *tids = (pthread_t *)malloc(nthreads * sizeof(pthread_t));
    gemm_arg_t *args = (gemm_arg_t *)malloc(nthreads * sizeof(gemm_arg_t));

    int rows_per = m / nthreads;
    int extra = m % nthreads;
    int start = 0;
    for (int t = 0; t < nthreads; t++) {
        int cnt = rows_per + (t < extra ? 1 : 0);
        args[t].start = start;
        args[t].end = start + cnt;
        args[t].n = n;
        args[t].k = k;
        args[t].a = a;
        args[t].b = b;
        args[t].c = c;
        args[t].lda = lda;
        args[t].ldb = ldb;
        args[t].ldc = ldc;
        pthread_create(&tids[t], NULL, gemm_rows, &args[t]);
        start += cnt;
    }
    for (int t = 0; t < nthreads; t++)
        pthread_join(tids[t], NULL);

    free(tids);
    free(args);
}
