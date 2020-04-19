#include <memory.h>
#include <math.h>
#include <dirent.h>
#include <unistd.h>
#include <limits.h>
#include <opencv2/opencv.hpp>
#include "types.h"
#include "log5cxx.h"

using namespace std;
using namespace cv;

char __path[1024];
const char* getCurrentPath(){
#ifdef OS_LINUX
    getcwd(__path, 1024);
    /*readlink("/proc/self/exe", __path, BUFSIZ);
    int len = strlen(__path);
    while(true){
        if(__path[len-1] != '/')
            __path[len-1] = '\0';
        else
            break;
        len--;
    }*/
#else
    getcwd(__path, 1024);
#endif
    //printf("The current directory is: %s", buffer);
    return __path;
}

Alpha::Alpha():
    dataA(NULL),
    dataB(NULL){}

Alpha::~Alpha(){
    if(dataA != NULL){
        free(dataA);
        dataA = NULL;
    }
    if(dataB != NULL){
        free(dataB);
        dataB = NULL;
    }
}

double Alpha::getAlpha(float row, float col, int aOrb) const{
    unsigned int r = floor(row*rows), c = floor(col*cols);
    if(r < 0 || r >= rows || c < 0 || c >= cols)
        return 0.f;
    unsigned char v = 0;

    if(type == FixedValue)
        v = ((aOrb == 0) ? dataA[0] : dataB[0]);
    else if(type == FixedRow)
        v = ((aOrb == 0) ? dataA[c] : dataB[c]);
    else if(type == FixedColume)
        v = ((aOrb == 0) ? dataA[r] : dataB[r]);
    else
        v = ((aOrb == 0) ? dataA[r*cols+c] : dataB[r*cols+c]);

    return double(v)/255.0;
}

void Alpha::getAlpha(double* alpha, unsigned int len, float rowidx, int aOrb) const{
    unsigned int r = floor(rowidx*rows), idx;
    if(r < 0 || r >= rows)
        return;

    unsigned char *data_ptr = NULL;
    if(aOrb == 0)
        data_ptr = dataA;
    else
        data_ptr = dataB;

    if(type == FixedValue)
        for(int i=0; i<len; i++)
            alpha[i] = double(data_ptr[0])/255.0;
    else if(type == FixedRow)
        for(int i=0; i<len; i++){
            idx = round(float(i)/float(len)*cols);
            if(idx < 0) idx = 0;
            if(idx >= cols) idx = cols-1;
            alpha[i] = double(data_ptr[idx])/255.0;
        }
    else if(type == FixedColume)
        for(int i=0; i<len; i++)
            alpha[i] = double(data_ptr[r])/255.0;
    else
        for(int i=0; i<len; i++){
            idx = round(float(i)/float(len)*cols);
            if(idx < 0) idx = 0;
            if(idx >= cols) idx = cols-1;
            alpha[i] = double(data_ptr[r*cols+idx])/255.0;
        }
}

void Alpha::setAlpha(const char* effect_path, int index){
    char path[1024];
    strcpy(path, effect_path);
    strcat(path, SLASH);

    char buf[16];
    //itoa(index, buf, 10);
    sprintf(buf, "%d", index);
    strcat(path, buf);

    strcpy(AlphaA, path);
    strcpy(AlphaB, path);
    strcat(AlphaA, "A.jpg");
    strcat(AlphaB, "B.jpg");

    // call opencv to read image
    Mat imgA = imread(AlphaA, 0);
    Mat imgB = imread(AlphaB, 0);
    if(imgA.empty() || imgB.empty()){
        // read image failed, log it and quit.
        char buf[1024];
        sprintf(buf, "Read image file failed, image data not exist or error. [%s or %s]", AlphaA, AlphaB);
        LOG5CXX_FATAL(buf, 101);
    }

    rows = imgA.rows;
    cols = imgB.cols;
    if(rows == 1 && cols == 1)
        type = FixedValue;
    else if(rows == 1 && cols != 1)
        type = FixedRow;
    else if(rows != 1 && cols == 1)
        type = FixedColume;
    else
        type = Matrix;
    
    try{
        dataA = (unsigned char*)malloc(rows*cols);
        dataB = (unsigned char*)malloc(rows*cols);
        memcpy(dataA, imgA.data, rows*cols);
        memcpy(dataB, imgB.data, rows*cols);
    }catch(...){
        char msg[512];
        sprintf(msg, "Memory malloc or memcpy failed, operate memory size is [%d]=[%d MB].", rows*cols, rows*cols/1024/1024);
        LOG5CXX_FATAL(msg, 210);
    }
}
