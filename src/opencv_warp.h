#ifndef OPENCV_WARP_H
#define OPENCV_WARP_H

typedef struct{
    unsigned int rows;
    unsigned int cols;
    unsigned int channels;
    unsigned int bytesPerElement;
    void* data;                         // image data
} image_t;

#define WARPCVMAT(x, img) Mat x(img.rows, img.cols, CV_BUILD_MATRIX_TYPE(img.bytesPerElement, img.channels), img.data)

#endif
