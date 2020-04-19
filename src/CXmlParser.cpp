/* 
 * File:   CXmlParser.cpp
 * Author: Taniey
 * 
 * Created on 2014年4月23日, 上午9:14
 */

#include <iostream>
#include "CXmlParser.h"

const std::string CXmlParser::XML_NODE_CONF_SETTING = "ConfigurationSettings";
const std::string CXmlParser::XML_NODE_ANIMATION = "animation";
const std::string CXmlParser::XML_NODE_EFFECT = "effect";
const std::string CXmlParser::XML_NODE_NAME = "name";
const std::string CXmlParser::XML_NODE_FROM = "from";
const std::string CXmlParser::XML_NODE_TO = "to";
const std::string CXmlParser::XML_NODE_DURTATION = "duration";

const std::string CXmlParser::XML_ANIMATION_ATTRIBUTE_NAME = "name";
const std::string CXmlParser::XML_ANIMATION_ATTRIBUTE_PATH = "path";

CXmlParser::CXmlParser(const char * szXmlPath)
{
    m_strXmlPath = szXmlPath;
}

CXmlParser::~CXmlParser() {
}

///////////////////////////////////////////////////////////////////////////////
 // XML 格式是否完好
// 分析XML 内容 
bool CXmlParser::ParseXML()
{
    if (m_strXmlPath.empty()){
        m_strErrInfo = "Path is empty.";
        return false;
    }
    
    bool bResult = false;
    CMarkup markup;
    bResult = markup.Load(m_strXmlPath);
    if (!bResult) {
        m_strErrInfo = markup.GetError();
        return false;
    }
    if (!markup.IsWellFormed()) {
        m_strErrInfo = markup.GetError();
        return false;
    }
    
    bResult = markup.FindElem(XML_NODE_CONF_SETTING);
    if (!bResult) {
        m_strErrInfo = markup.GetError();
        return false;
    }
    markup.IntoElem(); // into "ConfigurationSettings" node
    bResult = markup.FindElem(XML_NODE_ANIMATION);
    if (!bResult) {
        m_strErrInfo = markup.GetError();
        return false;
     }
    m_strAniName = markup.GetAttrib(XML_ANIMATION_ATTRIBUTE_NAME);
    m_strDesPath = markup.GetAttrib(XML_ANIMATION_ATTRIBUTE_PATH);

    markup.IntoElem(); // into "animation" node
    while (markup.FindElem(XML_NODE_EFFECT)) {
        animation_t aniInfo;
        bResult = markup.IntoElem(); // into "effect" node
        if (!bResult) {
            bResult = markup.OutOfElem(); // outof "effect" node
           continue;
        }
        std::stringstream ss;
        //1.取出 Name值
        ss << ReadItem(markup, XML_NODE_NAME);
        ss >> aniInfo.EffectName;
        ss.clear();
        //2.取出 from 值
        ss << ReadItem(markup, XML_NODE_FROM);
        ss >> aniInfo.A;
        ss.clear();
        //3.取出 to 值
        ss << ReadItem(markup, XML_NODE_TO);
        ss >> aniInfo.B;
        ss.clear();
        //4.取出 duration 值
        ss << ReadItem(markup, XML_NODE_DURTATION);
        ss >> aniInfo.Duration;
        markup.OutOfElem();
        
        m_vctAni.push_back(aniInfo);
    }// while
    
    return true;
}// end

void CXmlParser::GetAniName(std::string &strAniName)
{
    strAniName = m_strAniName;
}

void CXmlParser::GetDesPath(std::string &strDesPath)
{
    strDesPath = m_strDesPath;
}
void CXmlParser::GetEffect(std::vector<animation_t> &vctAni)
{
    vctAni = m_vctAni;
}

std::string CXmlParser::GetErrInfo()
{
    return m_strErrInfo;
}

std::string CXmlParser::ReadItem(CMarkup &markup, std::string strItemName)
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

