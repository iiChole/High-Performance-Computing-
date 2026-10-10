#!/bin/bash
set -e
cd "$(dirname "$0")"
mkdir -p _build _data

export OMP_NUM_THREADS=4
export OPENBLAS_NUM_THREADS=4

CFLAGS="-std=gnu11 -O2 -Wall -Wno-unused-result -Wno-unused-variable -Wno-unused-value -Wno-unused-function"

# 公共目标文件
gcc $CFLAGS -c util.c -o _build/util.o
gcc $CFLAGS -c REF_MMult.c -o _build/REF_MMult.o

# $1=impl  $2=编译flags  $3=链接flags
build() {
    gcc $CFLAGS $2 -c "$1.c" -o "_build/$1.o"
    gcc _build/util.o _build/REF_MMult.o _build/test_MMult.o "_build/$1.o" -o "_build/$1.x" -lm $3
}

# 验证正确性: 带 -DRUN_REF 编译驱动, 输出 diff 到 /tmp
echo "########## 第一步: 验证正确性(看 diff 是否 ~1e-15)##########"
gcc $CFLAGS -DRUN_REF -c test_MMult.c -o _build/test_MMult.o
for impl in naive pthread openmp blas; do
    case $impl in
        naive)   cflags="";        lflags="";;
        pthread) cflags="-pthread"; lflags="-pthread";;
        openmp)  cflags="-fopenmp"; lflags="-fopenmp";;
        blas)    cflags="";        lflags="-lopenblas";;
    esac
    build $impl "$cflags" "$lflags"
    echo "===== $impl 验证结果 ====="
    "_build/$impl.x" 2>/dev/null | grep -v "^MY_MMult" | head -40
done

# 正式数据: 不带 RUN_REF(更快), 输出到 _data
echo ""
echo "########## 第二步: 正式数据(RUN_REF=0)##########"
gcc $CFLAGS -c test_MMult.c -o _build/test_MMult.o
for impl in naive pthread openmp blas; do
    case $impl in
        naive)   cflags="";        lflags="";;
        pthread) cflags="-pthread"; lflags="-pthread";;
        openmp)  cflags="-fopenmp"; lflags="-fopenmp";;
        blas)    cflags="";        lflags="-lopenblas";;
    esac
    build $impl "$cflags" "$lflags"
    {
        echo "date = '$(date)';"
        echo "version = '$impl';"
    } > "_data/output_$impl.m"
    "_build/$impl.x" 2>/dev/null >> "_data/output_$impl.m"
    echo "--- $impl 数据已写入 _data/output_$impl.m ---"
done

echo ""
echo "########## 完成, _data 内容 ##########"
ls -la _data/
