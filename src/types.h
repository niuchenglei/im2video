#ifndef _TYPES_H
#define _TYPES_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <string>
using namespace std;

#ifndef MAX_PATH
#define MAX_PATH 4096
#endif

#ifdef WIN32
#define OS_WINDOWS
#define SLASH   "\\"
#define BUFSIZ  1024
#else 
#define OS_LINUX
#define SLASH   "/"
#define LINUX_MARKUP_STDCONV
#include <unistd.h>
#endif

const char* getCurrentPath();

class Transpose{
public:
    double TransformA[9];          // transform matrix of A
    double TransformB[9];          // transform matrix of B
};

class Alpha{
public:
    Alpha();
    ~Alpha();

    /**
     *  Get the alpha value of specified pixel location.
     *
     *  @param   row
     *           The row of image, 0~1.
     *
     *  @param   col
     *           The colume of image, 0~1.
     *
     *  @param   aOrb
     *           The A or B.
     *
     *  @return  Returns the alpha value.
     */
    double getAlpha(float row, float col, int aOrb) const;

    /**
     *  Get the alpha value of specified row.
     *
     *  @param   alpha
     *           The row alpha array.
     *
     *  @param   len
     *           The alpha array length.
     *
     *  @param   rowidx
     *           The row of image, 0~1.
     *
     *  @param   aOrb
     *           The A or B.
     */
    void getAlpha(double* alpha, unsigned int len, float rowidx, int aOrb) const;

    /**
     *  Set the alpha image to the script.
     *
     *  @param   effect_path
     *           The effect path, such like D:\workspace\1.im2video\Bin\effects\fadeinout.
     *
     *  @param   index
     *           The index of the alpha image.
     */
    void setAlpha(const char* effect_path, int index);

    enum Type {FixedValue, FixedRow, FixedColume, Matrix};
private:
    char AlphaA[1024];              // filename of alpha A
    char AlphaB[1024];              // filename of alpha B

    unsigned char *dataA, *dataB;   // alpha iamge data
    unsigned int rows, cols;        // alpha image size
    Type type;
};

/*****************************************************************************
 *  A abstract of script.
 *  The Script class descript a animation's "from" and "to", the animation 
 *  include perspective transform and alpha transform.
 *****************************************************************************/
class Script{
public:
    Transpose trans;
    Alpha *alpha;
};


/****************************************************************************
 *  A abstract of effect.
 *  The Effect class can parse from xml file or init from user input.
 *****************************************************************************/
class Effect{
public:
    /**
     *  Construct a Effect warp.
     *
     *  @param   name
     *           The name of effect.
     *
     *  @param   scale
     *           The scale of effect, such as "43", "169"
     */
    Effect(const char* name, const char* scale = NULL);

    /**
     *  Destructor
     */
    ~Effect();

    /**
     *  Get script number of the effect.
     */
    bool interpolateTrans(float pos, Transpose *trans);
    Alpha* interpolateAlpha(float pos);

    /**
     *  Get the script in specified postion, position was normalized to 0-1.0.
     *
     *  @param   pos
     *           The position of script.
     */
    Script getScript(float pos);

    /**
     *  Read effect content from xml file.
     *
     *  @return  Returns true if the effect content loaded right.
     */
    bool fromXml();

    /**
     *  Whether the effect is none or not, return true when it's name is "NULL" or "".
     *
     *  @return  Returns true if the effect name is "NULL", "null" or "".
     */
    bool isNone() const;

    /**
     *  Clear the scripts
     */
    void clear();

    bool isAFront() const;

    unsigned char getBackground() const;

private:
    Effect(const Effect& rhs);
    Effect& operator=(const Effect& rhs);

    bool ParseXML(char * szXmlFileName);
    /*Script* getMin() const;
    Script* getMax() const;
    Script* getNear(float pos, float limit=0) const;*/

    char name[1024], scale[8];
    bool whoOnFront;
    unsigned char background;
    std::map<float, Transpose*> scripts_trans;
    std::map<float, Alpha*> scripts_alpha;
    unsigned int num_of_trans, num_of_alpha;
    bool valid;
    
private:
    static const std::string XML_NODE_CONF_SETTING;
    static const std::string XML_NODE_EFFECT;
    static const std::string XML_NODE_SCRIPT;
    static const std::string XML_NODE_A;
    static const std::string XML_NODE_B;

    static const std::string XML_EFFECT_ATTRIBUTE_NAME;
    static const std::string XML_EFFECT_ATTRIBUTE_ALPHANUM;
    static const std::string XML_EFFECT_ATTRIBUTE_DURATION;
    static const std::string XML_EFFECT_ATTRIBUTE_FRONT;
    static const std::string XML_EFFECT_ATTRIBUTE_BACKGROUND;
  
    static const std::string XML_SCRIPT_ATTRIBUTE_POS;
};


/*****************************************************************************
 *  FrameGenerator class responsible for generate every frame of effect.
 *  It's input is image data in memory, and the size of A/B must be equal.
 *****************************************************************************/
class FrameGenerator{
public:
    /**
     *  Construct a FrameGenerator with specified image size.
     *
     *  @param   rows
     *           The rows of the input images.
     *
     *  @param   columes
     *           The columes of the input images.
     *
     *  @param   channels
     *           The channels of the input images.
     *
     *  @param   frame_num
     *           The total frame num of the effect.
     */
    FrameGenerator(unsigned int rows, unsigned int columes, unsigned int channels);

    /**
     *  Destruct the FrameGenerator.
     */
    ~FrameGenerator();

    /**
     *  Set the num of frames that will produce.
     *
     *  @param   frame_num
     *           The num of frames.
     */
    void setFrameNum(unsigned int frame_num);

    /**
     *  Generate the image of the specified frame position(frameIdx/frame_num).
     *
     *  @param   OUTPUT
     *           The output memory of the image.
     *
     *  @param   A
     *           The start image of the effect.
     *
     *  @param   B
     *           The end image of the effect.
     *
     *  @param   effect 
     *           The effect name, one effect corresponds to a xml file, if effect is "NONE" or "none" then a series of fixed image will be output.
     *
     *  @param   frameIdx
     *           The index of frame that want to produce.
     *
     *  @return  Returns true if the image data memory were filled, else return false.
     */
    bool GenerateFrame(unsigned char* OUTPUT, const unsigned char* A, const unsigned char* B, Effect* effect, unsigned int frameIdx);

    /**
     *  Set the max thread to use
     *
     *  @param  max_threads
     *          The max number of threads the program will use
     */
    void setMaxThreads(unsigned int nn);
private:
    FrameGenerator(const FrameGenerator& rhs);
    FrameGenerator& operator=(const FrameGenerator& rhs);

    bool isEye(double v[9]);

    unsigned int rows, cols, channels;
    unsigned int frame_num;
};


#endif
