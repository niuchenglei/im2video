#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <math.h>
#include "util.h"
#include "im2video.h"
#include "videomix.h"
#include "log5cxx.h"
#include "CXmlParser.h"
#include "ResService.h"

//#ifdef linux
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <getopt.h> 
//#endif

using namespace std;
#include <opencv2/opencv.hpp>
using namespace cv;

#ifndef MAX_PATH
#define MAX_PATH 4096
#endif

#ifdef linux
#define NONE         "\033[m"  
#define RED          "\033[0;32;31m"
#define LIGHT_RED    "\033[1;31m"
#define GREEN        "\033[0;32;32m"
#define LIGHT_GREEN  "\033[1;32m"
#define BLUE         "\033[0;32;34m"
#define LIGHT_BLUE   "\033[1;34m"
#define DARY_GRAY    "\033[1;30m"
#define CYAN         "\033[0;36m"
#define LIGHT_CYAN   "\033[1;36m"
#define PURPLE       "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN        "\033[0;33m"
#define YELLOW       "\033[1;33m"
#define LIGHT_GRAY   "\033[0;37m"
#define WHITE        "\033[1;37m"
#endif

static char pp[][1024] = {
    "|                           |%3d %% %s\r",
    "|#                          |%3d %% %s\r",
    "|##                         |%3d %% %s\r",
    "|###                        |%3d %% %s\r",
    "|####                       |%3d %% %s\r",
    "|#####                      |%3d %% %s\r",
    "|######                     |%3d %% %s\r",
    "|#######                    |%3d %% %s\r",
    "|########                   |%3d %% %s\r",
    "|#########                  |%3d %% %s\r",
    "|##########                 |%3d %% %s\r",
    "|###########                |%3d %% %s\r",
    "|############               |%3d %% %s\r",
    "|#############              |%3d %% %s\r",
    "|##############             |%3d %% %s\r",
    "|###############            |%3d %% %s\r",
    "|################           |%3d %% %s\r",
    "|#################          |%3d %% %s\r",
    "|##################         |%3d %% %s\r",
    "|###################        |%3d %% %s\r",
    "|####################       |%3d %% %s\r",
    "|#####################      |%3d %% %s\r",
    "|######################     |%3d %% %s\r",
    "|#######################    |%3d %% %s\r",
    "|########################   |%3d %% %s\r",
    "|#########################  |%3d %% %s\r"
};

/**
 *  Callback function to notify the percentage has been changed.
 */
void percentHandle(int percentage, const char* msg)
{
    char BUF[1024];
    //sprintf(BUF, "PER: [ %d %%], MSG: [ %s ]\r", percentage, msg);

    int nn = percentage/4;
    sprintf(BUF, pp[nn], percentage, msg);
    printf(BUF);

    if(percentage == 100)
        printf("\n");
    fflush(stdout);
}

/**
 *  video maker
 */
void animationMaker(const char *script, const char *path, unsigned int max_thread)
{
    // 0. prepare for resource
    ResService rs;

    // 1. call xml parser to get input info
    //char vpath[1024];
    vector<animation_t> v;

    CXmlParser xmlParser(script);
    if (!xmlParser.ParseXML()) {
        LOG5CXX_FATAL(xmlParser.GetErrInfo().c_str(), 120);
    }
    //string strDesPath;
    //xmlParser.GetDesPath(strDesPath);
    //strcpy(vpath, strDesPath.c_str());

    // get effect script information
    xmlParser.GetEffect(v);


    // filter out the resources can't get, used for remote resource like http://abc.jpg
    rs.Filter(v);
    
    char msg[2048];
    sprintf(msg, "Input xml info: [%d Animations] --> [%s]", v.size(), path);
    LOG5CXX_INFO(msg);

    // 2. call director to make movie
    AnimationGenerator director;
    int rows, cols;
    xmlParser.GetSize(rows, cols);
    director.setSize(rows, cols);
    director.setAnimations(v);
    director.setMaxThreads(max_thread);

    director.toVideo(path, percentHandle);
}

// 视频合成特效
void mixrenderMaker(const char *from, const char *to, const char *effectname, const char *output, unsigned int max_thread) 
{
    VideoMixGenerator director(from, to, effectname);
    director.setMaxThreads(max_thread);
    director.toVideo(output, percentHandle);
}


void help()
{
    printf("\
Usage: im2video [options...] <file>\n\
Options:\n\
 -a/--out <file> Output video path to save the result video\n\
 -A/--script <file> Script file full path\n\
 -b/--maxthread <number> Maximum number threads will be use\n\
 -B/--mixfrom <file> Mix video from 'from' to 'to' with effect\n\
 -c/--mixto <file>  Mix video to\n\
 -C/--effectname <string> The name of effect between two video\n\
 -h/--help          This help text\n\
 -v/--version       Show version number and quit\n\
 -p/--predict <point file> Predict transform matrix from point pairs(at least 3 point)\n\
");
}

static char *optstring = "a:A:b:B:c:C:hvp:";
static struct option long_options[] = 
{         //  no_argument--0,required_argument--1,optional_argument--2
    {"out", 1, NULL, 'a'},
    {"script",    1, NULL, 'A'},
    {"maxthread", 1, NULL, 'b'},
    {"mixfrom",   1, NULL, 'B'},
    {"mixto",     1, NULL, 'c'},
    {"effectname",1, NULL, 'C'},
    {"help",      0, NULL, 'h'},
    {"version",   0, NULL, 'v'},
    {"predict",   1, NULL, 'p'},
    {0, 0, 0, 0}  
};

void predict_transform(const char *pts_file);

int main(int argc, char** argv)
{
    char _videopath[MAX_PATH], _script[MAX_PATH], _maxthread[64], _predict_file[MAX_PATH], _mixfrom[MAX_PATH], _mixto[MAX_PATH], _effectname[64];
    strcpy(_videopath, ""); strcpy(_script, ""); strcpy(_maxthread, ""); strcpy(_predict_file, ""); strcpy(_mixfrom, ""); strcpy(_mixto, ""); strcpy(_effectname, "");

    int opt, option_index = 0;;
    while ((opt = getopt_long(argc, argv, optstring, long_options, &option_index)) != -1) {
        switch(opt){
            case 'h':{ help(); return 0; }
            case 'a':{ strcpy(_videopath, optarg); break; }
            case 'A':{ strcpy(_script, optarg); break; }
            case 'b':{ strcpy(_maxthread, optarg); break; }
            case 'p':{ strcpy(_predict_file, optarg); break; }
            case 'B':{ strcpy(_mixfrom, optarg); break; }
            case 'c':{ strcpy(_mixto, optarg); break; }
            case 'C':{ strcpy(_effectname, optarg); break; }
        }
        /*
        printf("opt = %c\n", opt);  
        printf("optarg = %s\n", optarg);
        printf("optind = %d\n", optind);  
        printf("argv[optind - 1] = %s\n", argv[optind - 1]);  
        printf("option_index = %d\n", option_index);
        */
    }
    
    LOG5CXX_INIT(argv[0]);

    if (strcmp(_predict_file, "") != 0) {
        predict_transform(_predict_file);
        return 0;
    }
    if (strcmp(_script, "") != 0 && strcmp(_videopath, "") != 0) {
        animationMaker(_script, _videopath, atoi(_maxthread));
        return 0;
    }
    if (strcmp(_mixfrom, "") != 0 && strcmp(_mixto, "") != 0 && strcmp(_videopath, "") != 0 && strcmp(_effectname, "") != 0) {
        mixrenderMaker(_mixfrom, _mixto, _effectname, _videopath, atoi(_maxthread));
        return 0;
    }

    help();
    return 0;
}

/**
 *  calculate transform matrix according the point-pairs
 */
#include "lsq.h"
using namespace std;

void predict_transform(const char *pts_file)
{
    if (!pts_file){
        printf("no points file input\n");
        return;
    }

    FILE *fp = NULL;
    fp = fopen(pts_file, "r");
    if (!fp){
        printf("points file open failed\n");
        return;
    }

    vector<float> data_vec;
    char buf[256];
    string line_str, dem = ",";
    vector<string> after_split;
    float v;
    while(feof(fp) == 0){
        if(fgets(buf, 256, fp) == NULL)
            break;

        line_str = buf;
        split(line_str, dem, &after_split);

        if (after_split.size() != 4)
            continue;
        for (int i=0; i<4; i++){
            v = atof(after_split[i].c_str());
            data_vec.push_back(v);
        }
        after_split.clear();
    }

    if (data_vec.size()%4 != 0){
        printf("data not valid in point pairs\n");
        return;
    }

    int num_pts = data_vec.size()/4; // indicates how many point-pair are there
    float *data = (float*)malloc(num_pts*4*sizeof(float));
    for (int i=0; i<num_pts*4; i++)
        data[i] = data_vec[i];

    printf("<INFO> Find %d points\n", num_pts);

    // data format: [x y x' y']
    M33 result1 = lsqCurveFit(data, num_pts);

    printf("<INFO> Perspective Transform matrix maybe: \n");
    for(int i=0; i<9; i++)
        printf("%.5f ", result1._[i]);
    printf("\n");

    M33 result2 = lsqAffine(data);
    printf("<INFO> Affine Transform matrix maybe: \n");
    for(int i=0; i<9; i++)
        printf("%.5f ", result2._[i]);
    printf("\n");

    free(data);
}

int testCV() {
    cv::Mat img = cv::imread("xxx.jpg");
    Size dsize = Size(img.cols, img.rows);
    int width = dsize.width / 3;
    int height = dsize.height / 3;
    cv::Rect roi(width, height, width, height);
    cv::Mat imgTiny = img(roi);

    cv::Mat imgResize;
    resize(imgTiny, imgResize, dsize);
    cv::Mat imgBlur;
    int i = 200;
    //medianBlur(imgResize, imgBlur, 21);
    GaussianBlur(imgResize, imgBlur, Size(21, 21), 5, 5);

    imshow("nn", imgBlur);
    int c = waitKey(15000);

    return 0;
}
