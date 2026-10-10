#include "stdio.h"
#include "stdlib.h"
#include "sys/time.h"
#include "time.h"
#include <cblas.h>
#include <math.h>

void degemm_native (int m, int n, int k,
                    double alpha, double *A, int lda,
                                  double *B, int ldb,
                    double beta,  double *C, int ldc);

double check_error(double *C1, double *C2, int size);

// 编译 gcc -o time_dgemm time_dgemm.c –lopenblas
// 运行 ./time_dgemm 1024
int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("Input Error\n");
    return 1;
  }

  printf("test!\n");
  int i, m, n, k;
  m = n = k = atoi(argv[1]);

  int sizeofa = m * k;
  int sizeofb = k * n;
  int sizeofc = m * n;
  int lda = m;
  int ldb = k;
  int ldc = m;

  double alpha = 1.2;
  double beta = 0.001;

  struct timeval start, finish;
  double duration;

  double *A = (double *)malloc(sizeof(double) * sizeofa);
  double *B = (double *)malloc(sizeof(double) * sizeofb);
  double *C_blas = (double *)malloc(sizeof(double) * sizeofc);
  double *C_native = (double *)malloc(sizeof(double) * sizeofc);

  srand((unsigned)time(NULL));

  for (i = 0; i < sizeofa; i++)
  {
    A[i] = i % 3 + 1; // (rand() % 100) / 100.0;
  }

  for (i = 0; i < sizeofb; i++)
  {
    B[i] = i % 3 + 1; //(rand()%100)/10.0;
  }

  for (i = 0; i < sizeofc; i++)
  {
    C_blas[i] = 0.1;
    C_native[i] = 0.1;
  }

  printf("m=%d,n=%d,k=%d,alpha=%lf,beta=%lf,sizeofc=%d\n", m, n, k, alpha, beta, sizeofc);
  gettimeofday(&start, NULL);
  cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, m, n, k, alpha, A, lda, B, ldb, beta, C_blas, ldc);
  gettimeofday(&finish, NULL);
  double blas_duration = finish.tv_sec - start.tv_sec + (finish.tv_usec - start.tv_usec) / 1.0e6;

  gettimeofday(&start, NULL);
  degemm_native(m, n, k, alpha, A, lda, B, ldb, beta, C_native, ldc);
  gettimeofday(&finish, NULL);
  double native_duration = finish.tv_sec - start.tv_sec + (finish.tv_usec - start.tv_usec) / 1.0e6;

  //Compare the result
  double error = check_error(C_blas, C_native, sizeofc);
  printf("error=%.12e\n", error);

  // 转成成秒数
  double blas_gflops = 2.0 * m * n * k;
  blas_gflops = blas_gflops / blas_duration * 1.0e-9;
  double native_gflops = 2.0 * m * n * k;
  native_gflops = native_gflops / native_duration * 1.0e-9;

  FILE *fp;
  fp = fopen("timeDGEMM.txt", "a"); // 追加写
  fprintf(fp, "%dx%dx%d\t%lf s\t%lf GFLOPS\n", m, n, k, blas_duration, blas_gflops);
  fprintf(fp, "%dx%dx%d\t%lf s\t%lf GFLOPS\n", m, n, k, native_duration, native_gflops);
  fclose(fp);

  free(A);
  free(B);
  free(C_blas);
  free(C_native); 
  return 0;
}

void degemm_native (int m, int n, int k,
                    double alpha, double *A, int lda,
                                  double *B, int ldb,
                    double beta,  double *C, int ldc)
{
    int i, j, l;
    for (i = 0; i < m; i++){
      for (j = 0; j < n; j++){
        C[i + j * ldc] = beta * C[i + j * ldc];
        for (l = 0; l < k; l++){
          C[i + j * ldc] += alpha * A[i + l * lda] * B[l + j * ldb];
        }
      }
    }
}

double check_error(double *C1, double *C2, int size)
{
  int i, j;
  double error = 0.0;
  for(i = 0; i < size; i ++){
    double e = fabs(C1[i] - C2[i]);
    if(e > error){
      error = e;
    }
  }
  return error;
}
