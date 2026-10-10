# code:utf-8

import random
import time
import numpy as np
import argparse
import matplotlib.pyplot as plt   


TOLERANCE = 1e-10
n_list = [4, 16, 64, 256, 512, 1024, 2048] 
nrepeats = 1  # 3

result = []

def naive_gemm(n):
    #populate the matrices with random values between 0.0 and 1.0
    A = [[random.random() for row in range(n)] for col in range(n)]
    B = [[random.random() for row in range(n)] for col in range(n)]
    C = [[0 for row in range(n)] for col in range(n)]
    # https://realpython.com/python-data-types/
    # https://stackoverflow.com/questions/6663272/double-precision-floating-values-in-python
    # https://docs.python.org/3/tutorial/floatingpoint.html
    
    gflops = 2*n*n*n/(1000*1000*1000)
    elapsed_time = 1000000000
    for i in range(0,nrepeats):
        C = [[0 for row in range(n)] for col in range(n)]  # np.dot没有累加，每次计算前清零
        start = time.time()
        #matrix multiplication
        for i in range(n):
            for j in range(n):
                for k in range(n):
                    C[i][j] += A[i][k] * B[k][j]

        end = time.time()
        tmp_time = end - start
        if elapsed_time > tmp_time:
            elapsed_time = tmp_time
    
    print("%5d\t %5d\t %5d\t %5.2lf" % (n,n,n, gflops/elapsed_time))

def numpy_gemm(n, A, B):
    A = [[random.random() for row in range(n)] for col in range(n)]
    B = [[random.random() for row in range(n)] for col in range(n)]
    C_np = [[0 for row in range(n)] for col in range(n)]

    gflops = 2*n*n*n/(1000*1000*1000)
    
    # numpy性能测试
    np_elapsed_time = 1000000000
    for i in range(0,nrepeats):
        start = time.time()
        C_np = np.dot(A, B)
        end  = time.time()
        tmp_time = end - start
        if np_elapsed_time > tmp_time:
            np_elapsed_time = tmp_time
    
    print("%5d\t %5d\t %5d\t %5.2lf" % (n,n,n, gflops/np_elapsed_time))

    


print("python naive gemm:")
print("%m\t%n\t%k\t%naive_GFLOPS")
for n in n_list:   
    naive_gemm(n)


print("python numpy gemm:")
print("%m\t%n\t%k\t%numpy_GFLOPS")
for n in n_list:   
    numpy_gemm(n)
