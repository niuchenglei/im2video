#ifndef CXMLPARSER_H
#define	CXMLPARSER_H

#include <vector>
#include <string>
#include <sstream>
#include "xml/Markup.h"
#include "im2video.h"

class CXmlParser
{
public:
    CXmlParser(const char * szXmlPath = NULL);
    virtual ~CXmlParser();
public:
    virtual bool ParseXML();
    void GetAniName(std::string &strAniName);
    void GetDesPath(std::string &strDesPath);
    void GetEffect(std::vector<animation_t> &vctAni);
    void GetSize(int& rows, int& cols);
    
    std::string GetErrInfo();
private:
    std::string ReadItem(CMarkup &markup, std::string strItemName);
private:
    std::string m_strXmlPath;
    
    std::vector<animation_t> m_vctAni;
    
    std::string m_strAniName;
    std::string m_strDesPath;
    std::string m_strHeight, m_strWidth;
    std::string m_strPrefix;

    std::string m_strErrInfo;
    
    static const std::string XML_NODE_CONF_SETTING;
    static const std::string XML_NODE_ANIMATION;
    static const std::string XML_NODE_EFFECT;
    static const std::string XML_NODE_NAME;
    static const std::string XML_NODE_FROM;
    static const std::string XML_NODE_TO;
    static const std::string XML_NODE_DURTATION;
    static const std::string XML_NODE_FILL;

    static const std::string XML_ANIMATION_ATTRIBUTE_NAME;
    static const std::string XML_ANIMATION_ATTRIBUTE_PATH;
    static const std::string XML_ANIMATION_ATTRIBUTE_HEIGHT;
    static const std::string XML_ANIMATION_ATTRIBUTE_WIDTH;
    static const std::string XML_ANIMATION_ATTRIBUTE_PREFIX;

};

#endif	/* CXMLPARSER_H */

