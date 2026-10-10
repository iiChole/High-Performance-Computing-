#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/time.h>

/*return current time*/
static double GetTime(void)
{
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec / 1e6;
}

/*malloc n x n's double  anonymous matrix, put in shared map,
enable the process of children is visible for parents */
static double ** alloc_matrix(int n)
{
    double *data = mmap(NULL, (size_t)n * n * sizeof(double),
                        PROT_READ | PROT_WRITE,
                        MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if(data == MAP_FAILED){
        perror("mmap");
        exit(1);
    }
    double **m = malloc((size_t)n * sizeof(double *));
    for(int i = 0; i < n; i++){
        m[i] = data + (size_t)i * n;
    }

    return m;
}

static void init_matrix(double **m, int n, unsigned seed)
{
    srand(seed);
    for(int i = 0; i < n; i++)
        for(int j = 0; j < n; j ++)
            m[i][j] = (double)rand() / (double)RAND_MAX;
}

/*caculate the [r0, r1) rows of C = A * B*/
static void gemm_rows(double **A, double **B, double **C, int n, int r0, int r1)
{
    for(int i = r0; i < r1; i++){
        double *ci = C[i];
        for(int k = 0; k < n; k++){
            double a = A[i][k];
            double *bk = B[k];
            for (int j = 0; j < n; j++)
            ci[j] += a * bk[j];
        }
    }
}

int main (int argc, char *argv[])
{
    int n = (argc > 1) ? atoi(argv[1]) : 2048;
    int np = (argc > 2) ? atoi(argv[2]) : 4;

    if (np < 1) np = 1;
    if (np > n) np = n;

    double **A = alloc_matrix(n);
    double **B = alloc_matrix(n);
    double **C = alloc_matrix(n);

    init_matrix(A, n, 1);
    init_matrix(B, n, 2);
    memset(C[0], 0, (size_t)n * n * sizeof(double));

    double t0 = GetTime();

    int rows_per = n / np;
    for (int p = 0;p < np; p++){
        int r0 = p * rows_per;
        int r1 = (p == np - 1) ? n : r0 + rows_per;

        pid_t rc = fork();
        if (rc < 0){
            perror("fork");
            exit(0);
        }
        /*children's fork caculate*/
        else if (rc == 0){
            gemm_rows(A, B, C, n, r0, r1);
            exit(0);
        }
    }

    /*parent's fork wait until end*/
    for (int p = 0; p < np; p++)
    {
        wait(NULL);
    }

    double t1 = GetTime();

    printf("GEMM %d x %d, %d processes, time = %.3f s\n", n, n, np, t1 - t0);
    printf("C[0][0] = %f, C[%d][%d] = %f\n", C[0][0], n - 1, n - 1, C[n - 1][n - 1]);
    
    return 0;
}