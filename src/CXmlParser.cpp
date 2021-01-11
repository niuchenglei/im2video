#include <iostream>
#include "CXmlParser.h"

const std::string CXmlParser::XML_NODE_CONF_SETTING = "ConfigurationSettings";
const std::string CXmlParser::XML_NODE_ANIMATION = "animation";
const std::string CXmlParser::XML_NODE_EFFECT = "effect";
const std::string CXmlParser::XML_NODE_NAME = "name";
const std::string CXmlParser::XML_NODE_FROM = "from";
const std::string CXmlParser::XML_NODE_TO = "to";
const std::string CXmlParser::XML_NODE_DURTATION = "duration";
const std::string CXmlParser::XML_NODE_FILL = "fill";

const std::string CXmlParser::XML_ANIMATION_ATTRIBUTE_NAME = "name";
const std::string CXmlParser::XML_ANIMATION_ATTRIBUTE_PATH = "path";
const std::string CXmlParser::XML_ANIMATION_ATTRIBUTE_HEIGHT = "height";
const std::string CXmlParser::XML_ANIMATION_ATTRIBUTE_WIDTH = "width";
const std::string CXmlParser::XML_ANIMATION_ATTRIBUTE_PREFIX = "prefix";

CXmlParser::CXmlParser(const char * szXmlPath)
{
    m_strXmlPath = szXmlPath;
}

CXmlParser::~CXmlParser() {
}

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
    m_strHeight = markup.GetAttrib(XML_ANIMATION_ATTRIBUTE_HEIGHT);
    m_strWidth = markup.GetAttrib(XML_ANIMATION_ATTRIBUTE_WIDTH);
    m_strPrefix = markup.GetAttrib(XML_ANIMATION_ATTRIBUTE_PREFIX);
    printf("[%s], [%s], [%s]\n", m_strHeight.c_str(), m_strWidth.c_str(), m_strPrefix.c_str());

    markup.IntoElem(); // into "animation" node
    char tmp[16];
    while (markup.FindElem(XML_NODE_EFFECT)) {
        animation_t aniInfo;

        aniInfo.fillA = markup.GetAttrib("filla");
        aniInfo.fillB = markup.GetAttrib("fillb");
        if (aniInfo.fillA == "")
            aniInfo.fillA = "blur";
        if (aniInfo.fillB == "")
            aniInfo.fillB = "blur";

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
        ss << m_strPrefix;
        ss << ReadItem(markup, XML_NODE_FROM);
        ss >> aniInfo.A;
        ss.clear();

        //3.取出 to 值
        ss << m_strPrefix;
        ss << ReadItem(markup, XML_NODE_TO);
        ss >> aniInfo.B;
        ss.clear();

        //4.取出 duration 值
        ss << ReadItem(markup, XML_NODE_DURTATION);
        ss >> aniInfo.Duration;
        markup.OutOfElem();
        /*
        //5.
        ss << ReadItem(markup, XML_NODE_FILL);
        ss >> aniInfo.Fill;
        markup.OutOfElem();
        */
        
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

void CXmlParser::GetSize(int& rows, int& cols)
{
    if (m_strHeight != "") rows = atoi(m_strHeight.c_str());
    else rows = 0;
    if (m_strWidth != "") cols = atoi(m_strWidth.c_str());
    else cols = 0;
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
    } while (!bResult);
    markup.ResetMainPos();

    return strDes;
}

