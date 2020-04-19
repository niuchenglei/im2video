#include <math.h>
#include <time.h>
#include "types.h"
#include "videomix.h"
#include <opencv2/opencv.hpp>
#include "log5cxx.h"

using namespace std;
using namespace cv;

int static inline CV_BUILD_MATRIX_TYPE(int elementSize, int nChannels){
    // Determine type of the matrix
    switch (elementSize){
    case sizeof(char):
         return CV_8UC(nChannels);
         break;
    case (2*sizeof(char)):
         return CV_16UC(nChannels);
         break;
    case sizeof(float):
         return CV_32FC(nChannels);
         break;
    case sizeof(double):
         return CV_64FC(nChannels);
         break;
    }
    return -1;
}

VideoMixGenerator::VideoMixGenerator(const char *mix1, const char *mix2, const char *effect_name, unsigned int fps):
    _rows(0),
    _cols(0),
    _channels(0),
    _fps(fps) {
    
    if (fps == 0)
        setFps(25);

    strcpy(_mix1, mix1);
    strcpy(_mix2, mix2);
    strcpy(_effect_name, effect_name);

    _valid = true;
    if (strcmp(_mix1, "") == 0 ||
        strcmp(_mix2, "") == 0 ||
        strcmp(_effect_name, "") == 0)
        _valid = false;
}

VideoMixGenerator::~VideoMixGenerator(){
    // do free
}

void VideoMixGenerator::setFps(unsigned int fps){
    _fps = fps;

    if(_fps < 0 || _fps > 40)
        _fps = 25;

    char msg[512];
    sprintf(msg, "VideoMixGenerator fps has been set, FPS is [%d].", _fps);
    LOG5CXX_INFO(msg);
}

bool VideoMixGenerator::toVideo(const char* vpath, PROGRESS_HANDLE func, int *percentage){
    if(!_valid)
        return false;

    // 0. time taken
    clock_t s,e;
    char msg[2048];

    // 1. 获取mix1和mix2的宽高、帧率等信息；以及创建相应输出
    Mat mat1_cap, mat2_cap;
    VideoCapture mix1_cap, mix2_cap;  
    mix1_cap.open(_mix1);
    mix2_cap.open(_mix2);
    if(!mix1_cap.isOpened() || !mix2_cap.isOpened()) {
        if ((strlen(_mix1) + strlen(_mix2)) < 800)
            sprintf(msg, "Read video failed!(%s or %s)\n", _mix1, _mix2);
        else
            sprintf(msg, "Read video failed!\n");
        LOG5CXX_FATAL(msg, 121);
    }
    _numberframes = mix1_cap.get(CV_CAP_PROP_FRAME_COUNT);
    unsigned int _numberframes2 = mix2_cap.get(CV_CAP_PROP_FRAME_COUNT);
    if (_numberframes > _numberframes2)
        _numberframes = _numberframes2;
    _fps = mix1_cap.get(CV_CAP_PROP_FPS);
    mix1_cap.read(mat1_cap);
    mix2_cap.read(mat2_cap);
    if (mat1_cap.rows != mat2_cap.rows || mat1_cap.cols != mat2_cap.cols ||
        mat1_cap.depth() != mat2_cap.depth() ||
        mat1_cap.channels() != mat2_cap.channels()) {
        sprintf(msg, "Two video format not equal(maybe width/height/depth/channels)\n");
        LOG5CXX_FATAL(msg, 121);
    }
    _rows = mat1_cap.rows; _cols = mat1_cap.cols; _depth = mat1_cap.depth(); _channels = mat1_cap.channels();
    char ext_name[8];
    if (fabs(float(_cols)/float(_rows) - 4.0/3.0) < 0.111)
        strcpy(ext_name, "43");
    else if (fabs(float(_cols)/float(_rows) - 16.0/9.0) < 0.111)
        strcpy(ext_name, "169");
    else
        strcpy(ext_name, "");

    // 1.1 创建输出
    VideoWriter vw;
    /*
    CV_FOURCC('P', 'I', 'M', '1') = MPEG-1 codec
    CV_FOURCC('M', 'J', 'P', 'G') = motion-jpeg codec
    CV_FOURCC('M', 'P', '4', '2') = MPEG-4.2 codec 
    CV_FOURCC('D', 'I', 'V', '3') = MPEG-4.3 codec 
    CV_FOURCC('D', 'I', 'V', 'X') = MPEG-4 codec 
    CV_FOURCC('U', '2', '6', '3') = H263 codec 
    CV_FOURCC('I', '2', '6', '3') = H263I codec 
    CV_FOURCC('F', 'L', 'V', '1') = FLV1 codec
    */
    try {
        vw.open(vpath, 
                CV_FOURCC('M', 'J', 'P', 'G'),
                _fps,
                Size(_cols, _rows),
                true);
    } catch(...) {
        sprintf(msg, "Open video output file failed, maybe file path not valid. [%s]", vpath);
        LOG5CXX_FATAL(msg, 121);
    }
    if (!vw.isOpened()) {
        sprintf(msg, "Open video output file failed, maybe disk is full or something else. [%s]", vpath);
        LOG5CXX_FATAL(msg, 120);
    }

    // 2. 构建帧渲染器和特效描述类
    int percent;
    FrameGenerator generator(_rows, _cols, _channels);
    generator.setMaxThreads(_max_threads);
    generator.setFrameNum(_numberframes);
    Effect effect(_effect_name, ext_name);
    if (!effect.fromXml()) {       // 如果特效文件没有找到，则使用默认特效，并记录日志
        sprintf(msg, "The specified effect config file [%s] not exist.", _effect_name);
        LOG5CXX_ERROR(msg);
    }

    // 3. 逐帧渲染，并写入输出
    s = clock();
    Mat mat_canvas; mat1_cap.copyTo(mat_canvas);
    int idx = 0;
    do {
        bool _v = generator.GenerateFrame((unsigned char*)mat_canvas.data, 
                                          (unsigned char*)mat1_cap.data, 
                                          (unsigned char*)mat2_cap.data, 
                                          &effect, 
                                          idx);

        // error process
        if(!_v){
            char msg[512];
            sprintf(msg, "FrameGenerator generate returned false, may error raised. \
                          Need to debug source code. status:[EffectName:%s, FrameIdx: %d]", _effect_name, idx);
            LOG5CXX_ERROR(msg);
            continue;
        }
        
        // Call opencv cvWriteFrame(img.data);
        //imshow("video", imgCanvasMat);
        //imwrite("/home/niu/im2video/Bin/1.jpg", imgCanvasMat);
        //cvWaitKey(0);
        vw << mat_canvas;

        if (idx%2 == 0){
            percent = float(idx)/float(_numberframes)*100;
            if (percentage)
                *percentage = percent;
            if (func)
                func(percent, "");
        }
        
        idx++;
        if (!mix1_cap.read(mat1_cap) || !mix2_cap.read(mat2_cap))
            break;
    } while(true);
    e = clock();

    // 4. 释放资源
    // Update final progress
    percent = 100;
    if(percentage)
        *percentage = percent;
    if(func)
        func(percent, "");
    
#ifdef OS_LINUX
    printf("\033[0;32;31mtime used: [%.4f seconds]\033[m\n", double(e-s)/CLOCKS_PER_SEC);
#else
    printf("TIME USED: [%.4f ms]\n", double(e-s)/CLOCKS_PER_SEC);
#endif

    // Log process result to file
    log(vpath);

    return true;
}

void VideoMixGenerator::setMaxThreads(unsigned int max_threads){
    _max_threads = max_threads;
}

void VideoMixGenerator::log(const char* vpath){
    size_t len = strlen(_mix1) + strlen(_mix2) + strlen(vpath);
    char *msg = (char*)malloc(len+64);
    if (!msg) {
        return;
    }

    sprintf(msg, "mix two video successful (%s + %s -> %s)\n", _mix1, _mix2, vpath);
    LOG5CXX_INFO(msg);

    free(msg);
}

void VideoMixGenerator::progress(int* percentage, int frame_num, int i) {
    if(percentage == NULL) 
        return;
    float v = float(i)/float(frame_num);
    *percentage = v;
}
