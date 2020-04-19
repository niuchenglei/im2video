#ifndef IM2VIDEO_H
#define IM2VIDEO_H

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
} animation_t;

/*****************************************************************************
 *  Progress callback function, fist arg is percentage, second arg is msg
 *****************************************************************************/
typedef void(*PROGRESS_HANDLE)(int, const char*);

/*****************************************************************************
 *  动画生成器类，负责从脚本生成AVI视频
 *****************************************************************************/
class AnimationGenerator{
public:
    /**
     *  Construct a AnimationGenerator.
     *
     *  @param   fps
     *           The video fps.
     */
    AnimationGenerator(unsigned int fps=25);
    
    /**
     *  Construct a AnimationGenerator.
     *
     *  @param   animations
     *           The animation descriptors.
     *
     *  @param   fps
     *           The video fps.
     */
    AnimationGenerator(std::vector<animation_t>& animations, unsigned int fps=25);  // 15/24

    /**
     *  Destructor
     */
    ~AnimationGenerator();

    /**
     *  Set the video fps.
     *
     *  @param   fps
     *           The video fps.
     */
    void setFps(unsigned int fps);

    /**
     *  Set the video animation descriptors.
     *
     *  @param   animations
     *           The animation descriptors.
     */
    void setAnimations(std::vector<animation_t>& animations);

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
     *  Get the images file list of animations
     *
     *  @return  Returns the image list
     */
    std::vector<std::string> getImageList() const;

    /**
     *  Set the max thread to use
     *
     *  @param  max_threads
     *          The max number of threads the program will use
     */
    void setMaxThreads(unsigned int max_threads);

public:
    AnimationGenerator(const AnimationGenerator& rhs);
    AnimationGenerator& operator=(const AnimationGenerator& rhs);

    image_t loadImage(const char*);
    void releaseImage(image_t&);
    void log(const char*);
    void progress(int* percentage, int animation_num, int frame_num, int i, int j);

    unsigned int _rows, _cols, _channels, _depth, _max_threads;
    unsigned int _fps;
    std::vector<animation_t> _animations;
    bool _valid;
};

#endif
