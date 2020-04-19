#ifndef __LSQ_H__
#define __LSQ_H__

#include <stdlib.h>
#include <stdio.h>

typedef struct{
    double _[9];
}M33;

/*!
  函数返回变换矩阵（透视变换）

  \参数 data 是点集 [x y x' y']
  \参数 num 是点集和行数，即点对数量
  \返回值 经最小二乘计算出的变换矩阵
*/
M33 lsqCurveFit(float* data, unsigned int num);


/*!
  函数返回变换矩阵（仿射变换）

  \参数 data 是点集 3*[x y x' y']
  \返回值 经最小二乘计算出的变换矩阵
*/
M33 lsqAffine(float data[12]);

void Inv(float *A, int N);
void Inv(double *A, int N);

#endif