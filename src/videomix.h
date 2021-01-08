#ifndef VIDEOMIX_H
#define VIDEOMIX_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include "opencv_warp.h"

typedef struct{
    char A[1024];               // First frame
    char B[1024];               // Last frame
    char EffectName[64];        // Effect name, a series of fixed A image will output if it's value is "NONE" or "none"
    float Duration;             // How long the effect, unit: second
    bool valid;                 // Whether the value is valid or not
} videomix_t;

/*****************************************************************************
 *  Progress callback function, fist arg is percentage, second arg is msg
 *****************************************************************************/
typedef void(*PROGRESS_HANDLE)(int, const char*);

class VideoMixGenerator{
public:
    /**
     *  Constructor.
     *
     *  @param   mix1
     *
     *  @param   mix2
     *
     *  @param   effect_name
     *
     *  @param   fps
     *           the fps of output video，keep same to source video by default.
     */
    VideoMixGenerator(const char *mix1, const char *mix2, const char *effect_name, unsigned int fps = 0);
    
    /**
     *  析构函数
     */
    ~VideoMixGenerator();

    /**
     *  Set the video fps.
     *
     *  @param   fps
     *           The video fps.
     */
    void setFps(unsigned int fps);

    /**
     *  Produce video.
     *
     *  @param   vpath
     *           The path of the output video.
     *
     *  @param   func
     *           The progress callback function.
     *
     *  @param   percentage
     *           The progress percentage.
     *
     *  @return  Returns true if the operation is successfully.
     */
    bool toVideo(const char* vpath, PROGRESS_HANDLE func = NULL, int* percentage = NULL);

    /**
     *  Set the max thread to use
     *
     *  @param  max_threads
     *          The max number of threads the program will use
     */
    void setMaxThreads(unsigned int max_threads);

public:
    VideoMixGenerator(const VideoMixGenerator& rhs);
    VideoMixGenerator& operator=(const VideoMixGenerator& rhs);

    void log(const char*);
    void progress(int* percentage, int frame_num, int i);

    unsigned int _rows, _cols, _channels, _depth, _max_threads;
    unsigned int _fps, _numberframes;
    char _mix1[4096], _mix2[4096], _effect_name[64];
    bool _valid;
};

#endif
