#ifndef RESSERVICE_H
#define RESSERVICE_H

#include <string>
#include <vector>
#include <map>

#include "im2video.h"


/*****************************************************************************
 *  ResService 类
 *  负责任务执行期间的文件资源管理，在任务结束时是否所有资源，因此一个任务
 *  多个执行项间可以共享数据.
 *****************************************************************************/
class ResService {
public:
    /**
     *  构造函数.
     *
     *  @param  ll
     *          任务日志.
     */
    ResService();

    /**
     *  析构函数.
     */
    ~ResService();

    /**
     *  过滤参数，给参数分配相应临时资源.
     *
     *  @return  返回执行情况，true代表成功，false代表失败.
     */
    bool Filter(std::vector<animation_t>& args);

    /**
     *  获取资源路径.
     *
     *  @return  返回路径.
     */
    const std::string& getTmpDir() const;

private:
    /**
     *  情况资源，移除资源路径.
     */
    void clearTemp();

    std::string tmp_path;
};

#endif
