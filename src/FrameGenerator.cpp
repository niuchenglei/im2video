#include <math.h>
#include <memory.h>
//#include <omp.h>
#include <float.h>
#include <math.h>
#include "types.h"
#include "lsq.h"
#include "log5cxx.h"
#include "exprtk.hpp"

using namespace std;

#define CLIP(x) ((int(x)>255)?255:x)

#define MAX_THREADS 16
int __OMP_THREAD = 1; //omp_get_num_procs();
int __NUM_THREAD = 1; //(__OMP_THREAD > MAX_THREADS) ? MAX_THREADS : __OMP_THREAD;

vector< exprtk::expression<double> > __expressions;     // A1, B1, A2, B2 ...

FrameGenerator::FrameGenerator(unsigned int _rows, unsigned int _columes, unsigned int _channels){
    rows = _rows;
    cols = _columes;
    channels = _channels;
}

FrameGenerator::~FrameGenerator(){}

void FrameGenerator::setFrameNum(unsigned int _frame_num){
    frame_num = _frame_num;
}

inline unsigned int __round__(double v){
    return 0;
}
bool FrameGenerator::GenerateFrame(unsigned char* OUTPUT, const unsigned char* A, const unsigned char* B, Effect* effect, unsigned int frameIdx){
    double t = float(frameIdx)/float(frame_num);
    if(t < 0 || t > 1.0f){
        char msg[512];
        sprintf(msg, "The t value not valid. [%.2f = %d/%d]", t, frameIdx, frame_num);
        LOG5CXX_ERROR(msg);
        return false;
    }
    
    if(effect->isNone()){
        try{
            memcpy(OUTPUT, A, rows*cols*channels);
        }catch(...){
            char msg[512];
            sprintf(msg, "Memory memcpy failed, [%x] --> [%x].", OUTPUT, A);
            LOG5CXX_FATAL(msg, 210);
        }
        return true;
    }

    Script script = effect->getScript(t);
    Transpose *script_trans = &(script.trans);
    Alpha *script_alpha = script.alpha;
    bool afront = effect->isAFront();
    unsigned char background = effect->getBackground();

    // get inverse of the transform matrix
    double transA[9], transB[9]; 
    memcpy(transA, script_trans->TransformA, 9*sizeof(double));
    memcpy(transB, script_trans->TransformB, 9*sizeof(double));
    bool isEyeA = isEye(transA), isEyeB = isEye(transB);
    try{
        if(!isEyeA)
            Inv(transA, 3); 
        if(!isEyeB)
            Inv(transB, 3);
    }catch(...){
        LOG5CXX_ERROR("The inverse operate to matrix failed.");
        return false;
    }

    // foreach every pixel in output, to calc the destination to A/B, and multiply the alpha value
    unsigned int r, c, k, b, idx, thread_offset;
    int rowA, colA, rowB, colB;
    double x, y, w;
    unsigned char arr[6], pixel_value[3];
    unsigned int pid, pixel_value_type;
    double *alpha_arr_A = NULL, *alpha_arr_B = NULL, apA, apB;        // alpha_arr_A is a row alpha array
    try
    {
        alpha_arr_A = (double*)malloc(cols*__NUM_THREAD*sizeof(double));
        alpha_arr_B = (double*)malloc(cols*__NUM_THREAD*sizeof(double));
    }catch(...){
        char msg[512];
        sprintf(msg, "Memory malloc or memcpy failed, operate memory size is [%d]=[%d MB].", cols*__NUM_THREAD*sizeof(double), cols*__NUM_THREAD*sizeof(double)/1024/1024);
        LOG5CXX_FATAL(msg, 210);
    }

    /*#pragma omp parallel for \
        num_threads(__NUM_THREAD) \ 
        shared(OUTPUT, transA, transB, alpha_arr_A, alpha_arr_B, background, afront) \
        private(r, c, k, b, idx, pid, x, y, w, rowA, colA, rowB, colB, arr, thread_offset, apA, apB, pixel_value, pixel_value_type)
    */for(r=0; r<rows; r++){
        idx = r*cols*channels;
        pid = 0; //omp_get_thread_num();     // thread id
        thread_offset = pid*cols;
        // 关于坐标归一化：需要将xy按照比例归一化到0-1（x为0-1，y不限定）
        y = double(r)/double(rows);
        //y = double(r);

        // get row alpha value
        script_alpha->getAlpha(alpha_arr_A+thread_offset, cols, y, 0);
        script_alpha->getAlpha(alpha_arr_B+thread_offset, cols, y, 1);

        for(c=0; c<cols; c++){
            // 1. remap the pixel index
            x = double(c)/double(cols);
            //这样归一化到0-1有问题，可通过matlab测试，坐标转换前不能归一化
            //x = double(c)/double(rows);
            rowA = r; colA = c; rowB = r; colB = c;

            if(!isEyeA){
                w = x*transA[6] + y*transA[7] + transA[8];
                rowA = round((x*transA[3] + y*transA[4] + transA[5])/w*rows); 
                colA = round((x*transA[0] + y*transA[1] + transA[2])/w*cols);
            }
            if(!isEyeB){
                w = x*transB[6] + y*transB[7] + transB[8];
                rowB = round((x*transB[3] + y*transB[4] + transB[5])/w*rows);
                colB = round((x*transB[0] + y*transB[1] + transB[2])/w*cols);
            }

            // 2. remap the pixel alpha rely on row/colume/time. use colume alpha array instead to accelerate
            //alphaA = script->getAlpha(rowA, colA, 0);
            //alphaB = script->getAlpha(rowB, colB, 1);

            // 3. fill pixel value
            pixel_value_type = 0;
            if(rowA >= 0 && rowA < rows && colA >= 0 && colA < cols){
                memcpy(arr, A+(rowA*cols+colA)*channels, channels);
                pixel_value_type+=1;
            }
            if(rowB >= 0 && rowB < rows && colB >= 0 && colB < cols){
                memcpy(arr+3, B+(rowB*cols+colB)*channels, channels);
                pixel_value_type+=2;
            }

            switch(pixel_value_type){
                case 0: {
                    pixel_value[0] = background;
                    pixel_value[1] = background;
                    pixel_value[2] = background;
                    break;
                }
                case 1:{
                    apA = alpha_arr_A[thread_offset+c];
                    pixel_value[0] = apA*arr[0];
                    pixel_value[1] = apA*arr[1];
                    pixel_value[2] = apA*arr[2];
                    break;
                }
                case 2:{
                    apB = alpha_arr_B[thread_offset+c];
                    pixel_value[0] = apB*arr[3];
                    pixel_value[1] = apB*arr[4];
                    pixel_value[2] = apB*arr[5];
                    break;
                }
                case 3:{
                    apA = alpha_arr_A[thread_offset+c];
                    apB = alpha_arr_B[thread_offset+c];
                    if(apA == 1.0f && apB == 1.0f){
                        if(afront){
                            pixel_value[0] = arr[0];
                            pixel_value[1] = arr[1];
                            pixel_value[2] = arr[2];
                        }
                        else{
                            pixel_value[0] = arr[3];
                            pixel_value[1] = arr[4];
                            pixel_value[2] = arr[5];
                        }
                    }else{
                        pixel_value[0] = CLIP(apA*arr[0] + apB*arr[3]);
                        pixel_value[1] = CLIP(apA*arr[1] + apB*arr[4]);
                        pixel_value[2] = CLIP(apA*arr[2] + apB*arr[5]);
                    }
                }
            }
            OUTPUT[idx+0] = pixel_value[0];
            OUTPUT[idx+1] = pixel_value[1];
            OUTPUT[idx+2] = pixel_value[2];
            
            memset(arr, 0, 6);
            idx += channels;
        }
    }
    free(alpha_arr_A);
    free(alpha_arr_B);

    return true;
}

void FrameGenerator::setMaxThreads(unsigned int nn){
    if(nn != 0)
        __NUM_THREAD = nn;
}

bool FrameGenerator::isEye(double v[9]){
    bool ret = true;
    double eye[9] = {1.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,1.0f};
    for(int i=0; i<9; i++){
      ret &= (abs(v[i]-eye[i])<FLT_EPSILON);
    }

    return ret;
}

/*
void FrameGenerator::InitFormula(const char* formulaA, const char* formulaB, double* R, double* C, double T){
    string exp_strA = formulaA, exp_strB = formulaB;

    int idx;
    for(int i=0; i<__NUM_THREAD; i++){
        idx = i*2;
        exprtk::symbol_table<double> symbol_table_A, symbol_table_B;
        symbol_table_A.add_variable("R", R[idx]);
        symbol_table_A.add_variable("C", C[idx]);
        symbol_table_A.add_variable("T", T);
        symbol_table_B.add_variable("R", R[idx+1]);
        symbol_table_B.add_variable("C", C[idx+1]);
        symbol_table_B.add_variable("T", T);

        exprtk::expression<double> expressionA, expressionB;
        expressionA.register_symbol_table(symbol_table_A);
        expressionB.register_symbol_table(symbol_table_B);

        exprtk::parser<double> parserA, parserB;
        parserA.compile(exp_strA, expressionA);
        parserB.compile(exp_strB, expressionB);

        __expressions.push_back(expressionA);
        __expressions.push_back(expressionB);
    }
}

double FrameGenerator::FormulaValue(int tid, int aOrb){
    //return __expressions[2*tid+aOrb].value();
    return 1.0;
}*/
