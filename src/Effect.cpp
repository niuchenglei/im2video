#include <math.h>
#include <string.h>
#include "types.h"
#include "log5cxx.h"

#include <unistd.h>
#include <sstream>
//#include <dirent.h>
#include "xml/Markup.h"

#include <iostream>

using namespace std;

const std::string Effect::XML_NODE_CONF_SETTING = "render";
const std::string Effect::XML_NODE_EFFECT = "effect";
const std::string Effect::XML_NODE_SCRIPT = "script";
const std::string Effect::XML_NODE_A = "A";
const std::string Effect::XML_NODE_B = "B";

const std::string Effect::XML_EFFECT_ATTRIBUTE_NAME = "name";
const std::string Effect::XML_EFFECT_ATTRIBUTE_ALPHANUM = "alphanum";
const std::string Effect::XML_EFFECT_ATTRIBUTE_DURATION = "duration";
const std::string Effect::XML_EFFECT_ATTRIBUTE_FRONT = "front";
const std::string Effect::XML_EFFECT_ATTRIBUTE_BACKGROUND = "background";

const std::string Effect::XML_SCRIPT_ATTRIBUTE_POS = "pos";

Effect::Effect(const char *_name, const char *_scale)
{
    strcpy(name, _name);
    if (_scale == NULL)
        strcpy(scale, "");
    else
        strcpy(scale, _scale);
    whoOnFront = true;
    background = 0;
}

Effect::~Effect()
{
    clear();
}

bool Effect::interpolateTrans(float pos, Transpose *trans)
{
    if(pos < 0 || pos > 1.0)
        return false;

    std::map<float, Transpose*>::iterator iter = scripts_trans.begin(), iter_pre;
    while(iter != scripts_trans.end()){
        if(iter->first > pos)
            break;
        iter_pre = iter;
        iter++;
    }
    float left_pos = iter_pre->first, right_pos = iter->first;
    Transpose *left = iter_pre->second, *right = iter->second;

    float t = (pos-left_pos)/(right_pos-left_pos);
    for(int i=0; i<9; i++){
        trans->TransformA[i] = (1-t)*left->TransformA[i] + t*right->TransformA[i];
        trans->TransformB[i] = (1-t)*left->TransformB[i] + t*right->TransformB[i];
    }
    return true;
}

Alpha* Effect::interpolateAlpha(float pos)
{
    if(pos < 0 || pos > 1.0)
        return false;
    
    std::map<float, Alpha*>::iterator iter = scripts_alpha.begin(), iter_pre;
    while(iter != scripts_alpha.end()){
        if(iter->first > pos)
            break;
        iter_pre = iter;
        iter++;
    }
    float left_pos = iter_pre->first, right_pos = iter->first;
    Alpha *left = iter_pre->second, *right = iter->second;

    Alpha *v;
    float d1 = pos - left_pos, d2 = right_pos - pos, thresh = 0.5/float(num_of_alpha);
    if(d1 <= thresh || d2 <= thresh){
        v = (d1<d2)?iter_pre->second:iter->second;
    } else {
        v = new Alpha;
        char effect_path[1024];
        strcpy(effect_path, getCurrentPath());
        strcat(effect_path, SLASH);
        strcat(effect_path, "effects");
        strcat(effect_path, SLASH);
        strcat(effect_path, name);

        int ind = round(pos*num_of_alpha);
        if(ind >= num_of_alpha) ind = num_of_alpha - 1;
        if(ind < 0) ind = 0;
        v->setAlpha(effect_path, ind);
        scripts_alpha.insert(pair<float,Alpha*>(pos, v));
    }
    return v;
}

Script Effect::getScript(float pos)
{
    pos = float((int)(pos*100))/100.0;
    Script ret;

    if(pos < 0 || pos > 1.0)
        return ret;

    Transpose *trans = &(ret.trans);
    Alpha *alpha = NULL;

    if(trans){
        map<float, Transpose*>::iterator iter = scripts_trans.find(pos);
        if(iter != scripts_trans.end()){
            memcpy(trans->TransformA, iter->second->TransformA, 9*sizeof(double));
            memcpy(trans->TransformB, iter->second->TransformB, 9*sizeof(double));
        }
        else
            interpolateTrans(pos, trans);
    }

    if(!alpha){
        map<float, Alpha*>::iterator iter = scripts_alpha.find(pos);
        if(iter != scripts_alpha.end())
            alpha = iter->second;
        else
            alpha = interpolateAlpha(pos);
    }

    ret.alpha = alpha;
    return ret;
}

bool Effect::fromXml()
{
    /*  ================== test code =========*/
    /*
    num_of_alpha = 2;
    Script *from = new Script;
    Script *to = new Script;
    double v1[9] = {1,0,0,0,1,0,0,0,1};
    double v2[9] = {1,0,0,0,1,-1,0,0,1};
    double v3[9] = {1,0,0,0,1,1,0,0,1};
    memcpy(from->TransformA, v1, 9*sizeof(double));
    memcpy(from->TransformB, v3, 9*sizeof(double));
    //memcpy(from->TransformB, v1, 9*sizeof(double));
    memcpy(to->TransformA, v2, 9*sizeof(double));
    //memcpy(to->TransformA, v1, 9*sizeof(double));
    memcpy(to->TransformB, v1, 9*sizeof(double));

    char effect_path[1024];
    strcpy(effect_path, getCurrentPath());
    strcat(effect_path, SLASH);
    strcat(effect_path, "effects");
    strcat(effect_path, SLASH);
    strcat(effect_path, name);
    from->setAlpha(effect_path, round(0*num_of_alpha));
    to->setAlpha(effect_path, round(1*num_of_alpha)-1);
    type = FrameInter;*/
    
    if(isNone())
        return true;

    bool ret = ParseXML(name);
    if(ret){
        // set alpha image path and read to memory
        char effect_path[1024];
        strcpy(effect_path, getCurrentPath());
        strcat(effect_path, SLASH);
        strcat(effect_path, "effects");
        strcat(effect_path, SLASH);
        strcat(effect_path, name);

        int ind = 0;
        for(int i=0; i<2; i++){
            ind = round(i*num_of_alpha);
            if(ind < 0) ind = 0;
            if(ind >= num_of_alpha) ind = num_of_alpha - 1;

            Alpha *alpha = new Alpha;
            alpha->setAlpha(effect_path, ind);
            scripts_alpha.insert(pair<float,Alpha*>(float(i), alpha));
        }
    }else{
        strcpy(name, "none");
    }

    return ret;
}

bool Effect::isNone() const
{
    std::string _name = name;
    if(_name == "" || _name == "NULL" || _name == "null" || _name == "NONE" || _name == "none")
        return true;
    return false;
}

void Effect::clear()
{
    std::map<float, Transpose*>::iterator it1 = scripts_trans.begin();
    for ( ;it1 != scripts_trans.end(); it1++) {
        if (!(it1->second)) {
            delete it1->second;
        }
    }

    std::map<float, Alpha*>::iterator it2 = scripts_alpha.begin();
    for ( ;it2 != scripts_alpha.end(); it2++) {
        if (!(it2->second)) {
            delete it2->second;
        }
    }
}

bool Effect::isAFront() const
{
    return whoOnFront;
}
    
unsigned char Effect::getBackground() const
{
    return background;
}

std::string ReadItem(CMarkup &markup, std::string strItemName)
{
    std::string strDes = "";
    bool bResult = false;
    do {
        bResult = markup.FindElem(strItemName);
        if (bResult){
            strDes = markup.GetData();
        }
    }while (!bResult);
    markup.ResetMainPos();

    return strDes;
}

bool FillMat(double * Mat, int nNum, const char * szMatList)
{
    stringstream ss;
 
    int ni = 0;
    string strML(szMatList);
    while (!strML.empty() && ni < nNum-1) {
        int nIndex = strML.find(',');
        if (nIndex != string::npos) {
            ss << strML.substr(0, nIndex);
            ss >> Mat[ni++];
            ss.clear();
            strML.erase(0, nIndex+1);
        }
    }// while

    // handle the last
    ss << strML;
    ss >> Mat[ni];

    return true;
}

bool Effect::ParseXML(char * szXmlFileName)
{
    string strXmlFileName(szXmlFileName);
    if (strXmlFileName.empty()){
        return false;
    }

    const char *szCWD = getCurrentPath();
    string strCurPath(szCWD);
    //strCurPath.rfind("/");
    //strCurPath = strCurPath.substr(0, strCurPath.rfind("/"));


    bool bResult = false;
    CMarkup markup;
    bResult = markup.Load(strCurPath+"/effects/"+ strXmlFileName + "/config.xml");
    
    //cout << strCurPath+"/effects/" + strXmlFileName << endl;
    
    if (!bResult) {
        return false;
    }
    if (!markup.IsWellFormed()) {
        return false;
    }
    
    bResult = markup.FindElem(XML_NODE_CONF_SETTING);
    if (!bResult) {
        return false;
    }
    markup.IntoElem(); // into "ConfigurationSettings" node
    bResult = markup.FindElem(XML_NODE_EFFECT);
    if (!bResult) {
        return false;
    }

    string strFront = markup.GetAttrib(XML_EFFECT_ATTRIBUTE_FRONT);
    if(strFront == "A")
        whoOnFront = true;
    else
        whoOnFront = false;

    string strBackground = markup.GetAttrib(XML_EFFECT_ATTRIBUTE_BACKGROUND);
    background = atoi(strBackground.c_str());

    stringstream ss;
    ss << markup.GetAttrib(XML_EFFECT_ATTRIBUTE_ALPHANUM);
    ss >> num_of_alpha;

    markup.IntoElem();// into "effect" node
    while (markup.FindElem(XML_NODE_SCRIPT)){
        float fPos = 0;
        ss.clear();
        ss << markup.GetAttrib(XML_SCRIPT_ATTRIBUTE_POS);
        ss >> fPos;
        
        Transpose *pS = new Transpose();
        markup.IntoElem();
        string strMList, strMListKey;
        strMListKey = XML_NODE_A + scale;
        strMList = ReadItem(markup, strMListKey);
        FillMat(pS->TransformA, 9, strMList.c_str());

        strMListKey = XML_NODE_B + scale;
        strMList = ReadItem(markup, strMListKey);
        FillMat(pS->TransformB, 9, strMList.c_str());

        markup.OutOfElem();
        
        scripts_trans.insert(pair<float, Transpose*>(fPos, pS));
    }// while

    return true;
}
/*
Script* Effect::getMin() const{
    return getNear(0, 100);
}

Script* Effect::getMax() const{
    return getNear(1.2, 100);
}

Script* Effect::getNear(float pos, float limit) const{
    Script* ret = NULL;
    float dis = 100.0;
    map<float, Script*>::const_iterator iter;
    for(iter=scripts.begin(); iter!=scripts.end(); iter++){
        if(dis > fabs(iter->first - pos)){
            dis = fabs(iter->first - pos);
            ret = iter->second;
        }
    }

    if(limit == 0)
        limit = 1.0f/num_of_alpha + 0.001;
    if(dis <= limit)
        return ret;
    return NULL;
}*/