#include <math.h>
#include <time.h>
#include "types.h"
#include "im2video.h"
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

AnimationGenerator::AnimationGenerator(unsigned int fps):
    _rows(0),
    _cols(0),
    _channels(0),
    _fps(fps){
    // do construct
}

AnimationGenerator::AnimationGenerator(std::vector<animation_t>& animations, unsigned int fps){
    // do construct
    setFps(fps);
    setAnimations(animations);
}

AnimationGenerator::~AnimationGenerator(){
    // do free
}

void AnimationGenerator::setFps(unsigned int fps){
    _fps = fps;

    if(_fps < 0 || _fps > 40)
        _fps = 25;

    char msg[512];
    sprintf(msg, "Animation fps has been set, FPS is [%d].", _fps);
    LOG5CXX_INFO(msg);
}

void AnimationGenerator::setAnimations(std::vector<animation_t>& animations){
    _animations = animations;
    _valid = true;

    int num = animations.size();
    char msg[512];
    sprintf(msg, "Animation script has been set, animation size is [%d].", num);
    LOG5CXX_INFO(msg);
}

bool AnimationGenerator::toVideo(const char* vpath, PROGRESS_HANDLE func, int* percentage){
    if(!_valid)
        return false;

    int num_of_animations = _animations.size();
    if(num_of_animations < 1)
        return false;

    // 0. time taken
    clock_t s,e;

    // 1. Get image size information from first image
    image_t imgCanvas = loadImage(_animations[0].A);
    WARPCVMAT(imgCanvasMat, imgCanvas);     // warp struct to opencv mat
    // 准备宽高比
    char ext_name[8];
    if (fabs(float(_cols)/float(_rows) - 4.0/3.0) < 0.111)
        strcpy(ext_name, "43");
    else if (fabs(float(_cols)/float(_rows) - 16.0/9.0) < 0.111)
        strcpy(ext_name, "169");
    else
        strcpy(ext_name, "");

    // 2. Construct a FrameGenerator
    int percent;
    FrameGenerator generator(imgCanvas.rows, imgCanvas.cols, imgCanvas.channels);
    generator.setMaxThreads(_max_threads);
    
    // 3. Open output file
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
    try{
        vw.open(vpath, 
                CV_FOURCC('M', 'J', 'P', 'G'),
                _fps,
                Size(_cols, _rows),
                true);
    }catch(...){
        char msg[512];
        sprintf(msg, "Open video output file failed, maybe file path not valid. [%s]", vpath);
        LOG5CXX_FATAL(msg, 121);
    }
    if(!vw.isOpened()){
        // open video output error, so log it.
        char msg[512];
        sprintf(msg, "Open video output file failed, maybe disk is full or something else. [%s]", vpath);
        LOG5CXX_FATAL(msg, 120);
    }

    s = clock();
    // 4. Iterator every animation and produce it's interi frame
    animation_t& ani = _animations[0];
    unsigned int frames = 0;
    for(int i=0; i<num_of_animations; i++){
        ani = _animations[i];
        if(ani.Duration <= 0)
            ani.Duration = 1;

        frames = floor(ani.Duration*float(_fps));
        
        Effect effect(ani.EffectName, ext_name);
        if(!effect.fromXml()){       // the specified effect config file not exist, the use default effect instead and log it.
            char msg[512];
            sprintf(msg, "The specified effect config file [%s] not exist.", ani.EffectName);
            LOG5CXX_ERROR(msg);
        }

        generator.setFrameNum(frames);

        image_t imgA = loadImage(ani.A);
        image_t imgB;
        if(!effect.isNone())
            imgB = loadImage(ani.B);
        for(int j=0; j<frames; j++){
            bool _v = generator.GenerateFrame((unsigned char*)imgCanvas.data, 
                                              (unsigned char*)imgA.data, 
                                              (unsigned char*)imgB.data, 
                                              &effect, 
                                              j);

            // error process
            if(!_v){
                char msg[512];
                sprintf(msg, "FrameGenerator generate returned false, may error raised. \
                              Need to debug source code. status:[A:%s, B:%s, EffectName:%s, FrameIdx: %d]", 
                              ani.A, ani.B, ani.EffectName, j);
                LOG5CXX_ERROR(msg);
                continue;
            }

            // Call opencv cvWriteFrame(img.data);
            //imshow("video", imgCanvasMat);
            //imwrite("/home/niu/im2video/Bin/1.jpg", imgCanvasMat);
            //cvWaitKey(0);
	        vw << imgCanvasMat;

            if(j%2 == 0){
                // progress process
                progress(&percent, num_of_animations, frames, i, j);
                if(percentage)
                    *percentage = percent;
                if(func)
                    func(percent, "");
            }
        }
        
        releaseImage(imgA);
        if(!effect.isNone())
            releaseImage(imgB);
    }
    releaseImage(imgCanvas);
    e = clock();

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

std::vector<string> AnimationGenerator::getImageList() const{
    vector<string> list;
    if(_animations.size() == 0)
        return list;

    vector<string>::iterator iter;
    string str;

    // every animation has A and B image, sometimes some animation'B and the next animation'A is same.
    for(int i=0; i<_animations.size(); i++){
        str = _animations[i].A;
        iter = find(list.begin(), list.end(), str);
        if(iter == list.end())      // not find in file list, then add it to list
            list.push_back(str);

        str = _animations[i].B;
        iter = find(list.begin(), list.end(), str);
        if(iter == list.end())      // not find in file list, then add it to list
            list.push_back(str);
    }
    return list;
}

void AnimationGenerator::setMaxThreads(unsigned int max_threads){
    _max_threads = max_threads;
}

image_t AnimationGenerator::loadImage(const char* path){
    Mat img = imread(path);
    if(img.empty()){
        // read image failed, log it and quit.
        char msg[1024];
        sprintf(msg, "Read image file failed, image data not exist or error. [%s]", path);
        LOG5CXX_FATAL(msg, 110);
    }

    void *ptr = img.data;
    Mat imgResize;
    // if load the first image, then init the output video size with first image
    // that means the next image will resize to the size if it's size is not match.
    if(_rows == 0 && _cols == 0 && _channels == 0){
        _rows = img.rows;
        _cols = img.cols;
        _channels = img.channels();
        _depth = img.depth();
    }else if(_rows != img.rows || _cols != img.cols){
        // calculate the scale
        float s1 = float(_rows)/float(img.rows), s2 = float(_cols)/float(img.cols);
        Size dsize;
        if(s1 < s2)
            dsize = Size(img.cols*s1, _rows);
        else
            dsize = Size(_cols, img.rows*s2);
        Mat imgResizeSmall;
        resize(img, imgResizeSmall, dsize);

        // resize the image with hei/wid scale equal method
        imgResize = Mat::zeros(_rows, _cols, CV_8UC3);
        unsigned int i,j,k,idx1,idx2,offset_row, offset_col;
        if(s1 < s2){
            offset_row = 0;
            offset_col = float(_cols-imgResizeSmall.cols)/2.0;
        }else{
            offset_row = float(_rows-imgResizeSmall.rows)/2.0;
            offset_col = 0;
        }
        unsigned char *ptr1 = imgResize.data, *ptr2 = imgResizeSmall.data;
        for(i=0; i<imgResizeSmall.rows; i++){
            for(j=0; j<imgResizeSmall.cols; j++){
                idx1 = (i+offset_row)*_cols+(j+offset_col);
                idx2 = i*imgResizeSmall.cols+j;
                for(k=0; k<_channels; k++)
                    ptr1[idx1*_channels+k] = ptr2[idx2*_channels+k];
            }
        }
        ptr = imgResize.data;
    }else if(_channels != img.channels()){
        // resize the channel
    }

    image_t t; t.data = NULL;
    if(_depth != CV_8U){
        char msg[512];
        sprintf(msg, "The image depth[%d] is not support. (only CV_8U image is valid)", _depth);
        LOG5CXX_FATAL(msg, 110);
    }

    t.rows = _rows;
    t.cols = _cols;
    t.channels = _channels;
    t.bytesPerElement = 1;  // 1 byte
    try{
        t.data = malloc(_rows*_cols*_channels);
        memcpy(t.data, ptr, _rows*_cols*_channels);
    }catch(...){
        char msg[512];
        sprintf(msg, "Memory malloc or memcpy failed, operate memory size is [%d]=[%d MB].", _rows*_cols*_channels*_depth, _rows*_cols*_channels*_depth/1024/1024);
        LOG5CXX_FATAL(msg, 210);
    }
    
    return t;
}

void AnimationGenerator::releaseImage(image_t& t){
    if(t.data != NULL)
        free(t.data);
    t.data = NULL;
}

void AnimationGenerator::log(const char* vpath){
    vector<string> list = getImageList();
    int num = list.size();
    int nn = (num>4)?4:num;
    char msg[1024];

    sprintf(msg, "Whole images convert to video successfully [%d images] --> [%s], part of image list follows:\n", num, vpath);
    for(int i=0; i<nn; i++){
        strcat(msg, "   ");
        strcat(msg, list[i].c_str());
        strcat(msg, "\n");
    }

    if(nn != num){
        char buf[8];
        sprintf(buf, "%d", num-nn);
        strcat(msg, "     ... and ");
        strcat(msg, buf);
        strcat(msg, " file.\n");
    }
    strcat(msg, "<=========================== DONE ===========================>\n");
    LOG5CXX_INFO(msg);
}
    
void AnimationGenerator::progress(int* percentage, int animation_num, int frame_num, int i, int j){
    if(percentage == NULL) 
        return;

    float ani_base = 1.0f/animation_num*100;
    float fra_base = ani_base/frame_num;
    float v = ani_base*i + fra_base*j;
    *percentage = v;
}
