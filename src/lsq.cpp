#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include "lsq.h"
#include "lapacke.h"
#include "levmar.h"

/*!
  函数返回变换矩阵（透视变换）
*/
void __perspective(double *p, double *x, int m, int n, void *data){
    // m = 8;
    register int i;
    int NN = n/2;
    double *ydata = (double*)data, *xdata = x;
    
    for(i=0; i<NN; i++){
        xdata[2*i] = (p[0]*ydata[2*i] + p[1]*ydata[2*i+1] + p[2])/(p[6]*ydata[2*i] + p[7]*ydata[2*i+1] + 1.0);
        xdata[2*i+1] = (p[3]*ydata[2*i] + p[4]*ydata[2*i+1] + p[5])/(p[6]*ydata[2*i] + p[7]*ydata[2*i+1] + 1.0);
    }
}
void __check(double p[8], double *x, double *y, unsigned int num){
    double err_x = 0, err_y = 0, x1, x2, *ydata = y;
    for(int i=0; i<num; i++){
        x1 = (p[0]*ydata[2*i] + p[1]*ydata[2*i+1] + p[2])/(p[6]*ydata[2*i] + p[7]*ydata[2*i+1] + 1.0);
        x2 = (p[3]*ydata[2*i] + p[4]*ydata[2*i+1] + p[5])/(p[6]*ydata[2*i] + p[7]*ydata[2*i+1] + 1.0);
        err_x += (x1-x[2*i])*(x1-x[2*i]);
        err_y += (x2-x[2*i+1])*(x2-x[2*i+1]);
    }
    printf("ERR: %.5f %.5f\n", err_x, err_y);
}
int __getR(double R[8], double *xy, unsigned int num){      // num of points
    register int i, j;
    int ret;
    double p[8], *x, *y;
    int m, n;
    double opts[LM_OPTS_SZ], info[LM_INFO_SZ];

    opts[0] = LM_INIT_MU; opts[1] = 1E-15; opts[2] = 1E-15; opts[3] = 1E-20;
    opts[4] = LM_DIFF_DELTA; // relevant only if the Jacobian is approximated using finite differences; specifies forward differencing 
    //opts[4]=-LM_DIFF_DELTA; // specifies central differencing to approximate Jacobian; more accurate but more expensive to compute!

    m = 8; n = num*2;
    p[0]=1.0; p[1]=0; p[2]=0; p[3]=0; p[4]=1.0; p[5]=0; p[6]=0; p[7]=0;
    x = (double*)malloc(num*2*sizeof(double));
    y = (double*)malloc(num*2*sizeof(double));
    for(i=0; i<num; i++){
        x[2*i] = xy[4*i+2];
        x[2*i+1] = xy[4*i+3];
        y[2*i] = xy[4*i];
        y[2*i+1] = xy[4*i+1];
    }

    { 
        double *work, *covar;
        work=(double*)malloc((LM_DIF_WORKSZ(m, n)+m*m)*sizeof(double));
        if(!work){
    	    //fprintf(stderr, "memory allocation request failed in main()\n");
            //exit(1);
            return -1;
        }
        covar=work+LM_DIF_WORKSZ(m, n);

        ret=dlevmar_dif(__perspective, p, x, m, n, 1000, opts, info, work, covar, y); // no Jacobian, caller allocates work memory, covariance estimated

        //printf("Iterater: %d\n", ret);
        //printf("%.5f %.5f %.5f\n%.5f %.5f %.5f\n%.5f %.5f 1\n", p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8]);
        //__check(p, x, y, num);
        /*
        printf("Covariance of the fit:\n");
        for(i=0; i<m; ++i){
            for(j=0; j<m; ++j)
                printf("%g ", covar[i*m+j]);
            printf("\n");
        }
        printf("\n");
        */

        free(work);
    }
    free(x); free(y);
    memcpy(R, p, 8*sizeof(double));
    return ret;
}
M33 lsqCurveFit(float* data, unsigned int num){ // number of points
    register int i;
    double *xy = (double*)malloc(num*4*sizeof(double));
    double R[8];

    for(i=0; i<num; i++){
        xy[4*i] = data[4*i]; 
        xy[4*i+1] = data[4*i+1];
        xy[4*i+2] = data[4*i+2];
        xy[4*i+3] = data[4*i+3];
    }

    int ret = __getR(R, xy, num);
    free(xy);
    M33 r;
    if(ret != 0){
        r._[0] = R[0];
        r._[1] = R[1];
        r._[2] = R[2];
        r._[3] = R[3];
        r._[4] = R[4];
        r._[5] = R[5];
        r._[6] = R[6];
        r._[7] = R[7];
        r._[8] = 1.0;
    }else{
        memset(r._, 0, 9*sizeof(float));
    }

    return r;
}


extern "C" void dgetrf_( int* m, int* n, double* a, int* lda,
                    int* ipiv, int *info );
extern "C" void dgetri_( int* n, double* a, int* lda,
                    const int* ipiv, double* work, int* lwork,
                    int *info );
/*!
  函数返回变换矩阵（仿射变换）
*/
void __inv(double *A, int N){
    int *IPIV = new int[N+1];
    int LWORK = N*N;
    double *WORK = new double[LWORK];
    int INFO;

    dgetrf_(&N, &N, A, &N, IPIV, &INFO);
    dgetri_(&N, A, &N, IPIV, WORK, &LWORK, &INFO);

    delete IPIV;
    delete WORK;
}

void Inv(float *A, int N){
    double *_A = (double*)malloc(N*sizeof(double));
    for(int i=0; i<N*N; i++)
        _A[i] = A[i];
    __inv(_A, N);
    for(int i=0; i<N*N; i++)
        A[i] = _A[i];
    free(_A);
}
void Inv(double *A, int N){
    __inv(A, N);
}

M33 lsqAffine(float data[12]){
    register int i;
    double p[36] = {0}, t[6], R[9];
    
    for(i=0; i<3; i++){
        p[6*2*i] = data[4*i];
        p[6*2*i+1] = data[4*i+1];
        p[6*2*i+2] = 1;
        p[6*(2*i+1)+3] = data[4*i];
        p[6*(2*i+1)+4] = data[4*i+1];
        p[6*(2*i+1)+5] = 1;
        t[2*i] = data[4*i+2];
        t[2*i+1] = data[4*i+3];
    }
    __inv(p, 6);
    memset(R, 0, 9*sizeof(double));
    for(i=0; i<6; i++)
        R[i] = p[6*i]*t[0] + p[6*i+1]*t[1] + p[6*i+2]*t[2] + p[6*i+3]*t[3] + p[6*i+4]*t[4] + p[6*i+5]*t[5];
    R[8] = 1.0;
    
    M33 ret;
    for(i=0; i<9; i++)
        ret._[i] = R[i];//printf("%f ", R[i]);

    return ret;
}
