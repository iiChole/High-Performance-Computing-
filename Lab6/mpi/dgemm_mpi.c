#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

#define MAX_N     8  
#define NUM_CASES 3   


#define TAG_N   0    
#define TAG_A   1     
#define TAG_B   2     
#define TAG_C   3     
#define TAG_RES 4     


#define DEBUG   0

/* -------------------------Test data------------------------------*/

static const int caseN[NUM_CASES] = { 2, 4, 8 };

/* data1: 2*2 */
static const double A2[2 * 2] = {
    -1.000000e+00, -9.167380e-01,
    -9.980292e-01, -6.467147e-01,
};
static const double B2[2 * 2] = {
    -2.707955e-01, -8.154047e-01,
    -8.173388e-01, -2.556555e-02,
};
static const double C2[2 * 2] = {
    1.020081e+00, 8.388416e-01,
    7.988468e-01, 8.303313e-01,
};

/* data2: 4*4 */
static const double A4[4 * 4] = {
    -1.000000e+00, -2.707955e-01,  5.350056e-02,  8.634630e-01,
    -9.980292e-01, -8.173388e-01, -9.113315e-02,  1.361192e-01,
    -9.167380e-01, -8.154047e-01, -5.336431e-01,  1.121887e-01,
    -6.467147e-01, -2.556555e-02,  6.625836e-01, -8.983362e-01,
};
static const double B4[4 * 4] = {
     5.341023e-01,  7.519617e-01,  6.208589e-01, -8.464509e-01,
    -9.621704e-01,  6.311373e-02, -6.231595e-01,  6.305478e-01,
    -4.952805e-01,  8.405219e-01,  7.726289e-01,  9.697820e-01,
    -4.036057e-01,  3.086230e-02,  1.412280e-01, -7.632966e-01,
};
static const double C4[4 * 4] = {
    -6.485472e-01, -6.974358e-01, -2.888289e-01,  6.850692e-02,
     2.435674e-01, -8.744635e-01, -1.614912e-01,  1.371329e-01,
     5.139494e-01, -1.185891e+00, -4.575017e-01, -3.413287e-01,
    -2.864046e-01,  4.127302e-02, -5.262187e-04,  1.859551e+00,
};

/* data3: 8*8 */
static const double A8[8 * 8] = {
     1.900078e-02,  8.548636e-02, -3.314322e-01,  2.440604e-02, -6.461445e-01, -7.460251e-01,  5.706861e-01, -6.391164e-02,
    -3.681220e-01,  3.359572e-01, -8.358069e-01,  1.730207e-01, -7.762620e-01, -9.918196e-01, -1.155304e-01, -6.060053e-01,
     4.034051e-01, -6.577163e-01, -4.593707e-01,  4.775171e-01,  7.053617e-01, -9.989944e-01, -4.241922e-01, -1.056461e-01,
    -2.655067e-01,  6.615583e-01, -8.514753e-01,  7.433776e-01,  6.223437e-01, -6.140962e-01,  7.067059e-01,  4.568796e-01,
     7.526247e-01,  7.238108e-01,  3.524515e-01,  1.559346e-01,  9.097746e-01,  1.735603e-01,  5.194394e-02,  9.321860e-01,
    -2.580107e-01,  5.384423e-01,  6.203171e-01,  9.294072e-01,  8.085664e-01, -2.500817e-01,  4.568334e-01,  5.136816e-01,
     5.050480e-02, -8.948411e-01, -7.157754e-02, -3.329414e-01, -7.221695e-01, -9.464281e-01, -4.673714e-01, -6.788623e-02,
    -1.443309e-01,  9.334593e-01,  1.383701e-01, -2.143441e-01,  3.723747e-01, -7.046941e-02,  5.669680e-01,  7.379329e-01,
};
static const double B8[8 * 8] = {
    -7.319215e-01,  8.165651e-01, -4.029224e-02,  8.852518e-01,  5.361704e-01, -2.435351e-01, -5.578819e-01, -3.161686e-01,
     6.921052e-01,  5.488294e-01, -9.377434e-01, -2.239507e-01,  5.038825e-01, -1.625671e-01,  5.086909e-02, -8.637625e-02,
    -9.638247e-01, -2.678706e-01,  2.168604e-01,  8.610245e-01, -8.182718e-01, -7.231037e-01, -1.094956e-02,  1.762435e-01,
     8.817096e-01,  8.885760e-01,  5.845223e-01,  8.227684e-01,  7.079334e-01, -2.383066e-01,  7.650440e-01,  1.702999e-01,
     3.401642e-01,  7.696433e-01, -6.350064e-01, -3.245633e-01, -6.507111e-01, -4.004399e-01,  5.476602e-01, -8.601459e-01,
     5.999431e-01, -1.096578e-01,  5.024259e-02,  6.562465e-02, -9.604593e-01, -9.579758e-01, -8.819950e-01,  6.378629e-01,
    -7.135664e-01, -6.739431e-01,  7.975177e-01,  4.041545e-01, -1.618930e-01, -5.223948e-01, -7.603972e-01, -1.936786e-01,
     2.209206e-01, -1.503654e-01, -3.824202e-01,  9.420213e-01, -4.072602e-01, -9.257813e-01,  9.560251e-02,  1.709661e-01,
};
static const double C8[8 * 8] = {
    -7.024897e-01, -6.175928e-01,  7.138586e-01,  6.358145e-02,  1.412361e+00,  9.497799e-01, -1.198863e-01, -1.091874e-01,
     5.495471e-01, -5.828324e-02,  2.023827e-01, -1.409117e+00,  2.501541e+00,  2.480544e+00,  8.435441e-01, -7.664723e-02,
     3.326345e-02,  1.469986e+00, -1.598580e-02, -6.368559e-02,  1.211029e+00,  1.221015e+00,  1.691703e+00, -1.250213e+00,
     1.568243e+00,  1.036256e+00, -3.969600e-01, -3.099133e-02,  1.298360e+00, -5.740635e-02,  1.148578e+00, -9.824463e-01,
     3.303513e-01,  1.561958e+00, -1.425547e+00,  1.551175e+00, -5.565087e-01, -2.013690e+00,  1.271708e-01, -7.343229e-01,
     6.956050e-01,  1.009126e+00, -1.748624e-01,  1.339494e+00, -2.857656e-01, -1.493150e+00,  1.240700e+00, -5.529886e-01,
    -1.375815e+00, -8.533851e-01,  6.912177e-01, -1.710138e-01,  8.812948e-01,  1.767113e+00,  4.605126e-01,  8.840335e-02,
     2.721833e-01, -3.181246e-02, -1.034846e+00,  4.047740e-01, -4.389420e-01, -1.226528e+00, -1.319781e-01, -3.960062e-01,
};

static const double *caseA[NUM_CASES] = { A2, A4, A8 };
static const double *caseB[NUM_CASES] = { B2, B4, B8 };
static const double *caseC[NUM_CASES] = { C2, C4, C8 };

/*Caculate the lacation of each block*/
static void get_rows(int rank, int N, int numworkers, int *start, int *end)
{
    int base = N / numworkers;
    int rem  = N % numworkers;
    int idx = rank - 1;

    *start = idx * base + (idx < rem ? idx : rem);
    *end = *start + base + (idx < rem ? 1 : 0);
}

/*Print N*N matrix*/
static void print_matrix(const char *name, const double *M, int N)
{
    int i,j;
    printf("%s = {\n", name);
    for(i = 0; i < N; i++){
        for(j = 0; j < N; j++){
            printf("%.6e", M[i * N + j]);
        }
        printf("\n");
    }
    printf("}\n");
}

int main(int argc, char *argv[])
{
    int rank, size, c;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int numworkers = size - 1;
    double A[MAX_N * MAX_N];
    double B[MAX_N * MAX_N];
    double C[MAX_N * MAX_N];

    for(c = 0; c < NUM_CASES; c++){
        int N = caseN[c];
        int i, j, k;

        if(rank == 0){
            /*1. Initialize A, B, C*/
            for(i = 0; i < N * N; i++){
                A[i] = caseA[c][i];
                B[i] = caseB[c][i];
                C[i] = 0.0;
            }

            /*2. Dot to Dot, send to children process*/
            int w;
            for(w = 1; w < size; w++){
                MPI_Send(&N, 1, MPI_INT, w, TAG_N, MPI_COMM_WORLD);
                MPI_Send(A, N * N, MPI_DOUBLE, w, TAG_A, MPI_COMM_WORLD);
                MPI_Send(B, N * N, MPI_DOUBLE, w, TAG_B, MPI_COMM_WORLD);
                MPI_Send(C,  N * N, MPI_DOUBLE, w, TAG_C, MPI_COMM_WORLD);
            }

            /*3. Summary to C*/
            for(w = 1; w < size; w++){
                int start, end, cnt;
                get_rows(w, N, numworkers, &start, &end);
                cnt = (end - start) * N;
                if (cnt > 0){
                    MPI_Recv(&C[start * N], cnt, MPI_DOUBLE, w, TAG_RES,
                            MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                }
            }

            /*4. Print & Check*/
            print_matrix("A", A, N);
            print_matrix("B", B, N);
            print_matrix("C", C, N);

            {
                int ok = 1;
                for(i = 0; i < N * N && ok; i++)
                    if(fabs(C[i] - caseC[c][i]) > 1e-6)
                        ok = 0;
                printf(">>> case %d (N=%d): %s\n\n", c + 1, N, ok ? "PASS" : "FAIL");
            }
        }else{
            /*None main process*/

            /*1. Recieve N, A, B, C*/
            MPI_Recv(&N, 1,     MPI_INT,    0, TAG_N, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(A,  N * N, MPI_DOUBLE, 0, TAG_A, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(B,  N * N, MPI_DOUBLE, 0, TAG_B, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(C,  N * N, MPI_DOUBLE, 0, TAG_C, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            printf("[rank %d] N=%d, A[0]=%.6e, B[0]=%.6e\n", rank, N, A[0], B[0]);
            fflush(stdout);

            /*2. Caculate: C[start:end][:] = A[start:end][:] * B*/
            int start, end, cnt;
            get_rows(rank, N, numworkers, &start, &end);
            for(i = start; i < end; i++){
                for(j = 0; j < N; j++){
                    double s = 0.0;
                    for(k = 0; k < N; k++){
                        s += A[i * N + k] * B[k * N + j];
                    }
                    C[i * N + j] = s;
                }
            }
            /* 3. Send to main process*/
            cnt =  (end - start) * N;
            if(cnt > 0)
                MPI_Send(&C[start * N], cnt, MPI_DOUBLE, 0, TAG_RES, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}