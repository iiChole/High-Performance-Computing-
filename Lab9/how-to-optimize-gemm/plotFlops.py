import sys
import os
import math
import numpy as np   # pip install numpy
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
# 如果file_list指定了.m文件，读取该列表中的文件绘图
# 若file_list为空未指定，自动读取_data目录下的.m文件（不包括output_new.m和output_old.m）绘图
file_list = [] # ['output_new.m', 'output_old.m']

def readFile(filename):
    f = open(filename)
    sizes = []
    gflops = []
    try:

        f.readline()  # 第一行date
        f.readline()  # 第二行version
        f.readline()  # 第三行MY_MMult = [
        while True:
            line = f.readline()
            if line:
                slices = line.split(" ")
                if len(slices) <= 2:
                    break
                size = int(slices[0])
                flops = float(slices[1])
                if math.isfinite(flops):   # 小矩阵计时为0会产生inf, 跳过
                    sizes.append(size)
                    gflops.append(flops)
    finally:
        f.close()
    return sizes, gflops

if __name__ == '__main__':
    plt.rcParams.update({'font.size': 15})
    plt.xlabel('shape')
    plt.ylabel('gflops')
    l = len(sys.argv)

    # 曲线的类型，如果文件数多余curves_marker的数量，需要再增加
    curves_marker = ['bo-.', 'r-*', 'g-.', 'k--', 'c:', 'y^', 'm,', 'peru#']

    data_dir = os.path.dirname(os.path.abspath(__file__)) + '/_data/'
    if file_list:
        files = file_list
    else:
        files = os.listdir(data_dir)
        files = [x for x in files if x not in ['output_new.m', 'output_old.m']] # 去掉old和new

    print(files)
    i = 0
    for filename in files:
        if not filename.endswith('.m') :
            continue
        lable = filename.replace('output_', '').replace('.m','')
        x,y = readFile(data_dir + filename)
        plt.plot(x,y, curves_marker[i],label=lable)
        i = i + 1

    plt.legend()
    plt.yscale('log')
    plt.grid(True, which='both', linestyle='--', alpha=0.4)
    out = os.path.dirname(os.path.abspath(__file__)) + '/../gemm_plot.png'
    plt.savefig(out, dpi=150, bbox_inches='tight')
    print("saved " + out)
