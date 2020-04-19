/* 
 * File:   CXmlParser.h
 * Author: Taniey
 * 
 * Created on 2014年4月23日, 上午9:14
 */

#ifndef CXMLPARSER_H
#define	CXMLPARSER_H

#include <vector>
#include <string>
#include <sstream>
#include "xml/Markup.h"
#include "im2video.h"

// 功能 ：此类用于输入的XML解析
class CXmlParser
{
public:
    CXmlParser(const char * szXmlPath = NULL);
    virtual ~CXmlParser();
public:
    virtual bool ParseXML(); // 分析XML 内容 
    void GetAniName(std::string &strAniName);
    void GetDesPath(std::string &strDesPath);
    void GetEffect(std::vector<animation_t> &vctAni);
    
    std::string GetErrInfo();
private:
    std::string ReadItem(CMarkup &markup, std::string strItemName);
private:
    std::string m_strXmlPath;
    
    std::vector<animation_t> m_vctAni; // 特效内容
    
    std::string m_strAniName;       // 名称
    std::string m_strDesPath;        // 结果地址

    std::string m_strErrInfo;       // 出错的信息输出
    
    static const std::string XML_NODE_CONF_SETTING;
    static const std::string XML_NODE_ANIMATION;
    static const std::string XML_NODE_EFFECT;
    static const std::string XML_NODE_NAME;
    static const std::string XML_NODE_FROM;
    static const std::string XML_NODE_TO;
    static const std::string XML_NODE_DURTATION;

    static const std::string XML_ANIMATION_ATTRIBUTE_NAME;
    static const std::string XML_ANIMATION_ATTRIBUTE_PATH;

};

#endif	/* CXMLPARSER_H */

