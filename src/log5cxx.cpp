#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include <assert.h>
#include <iostream>
#include "log5cxx.h"
#include "types.h"
#ifdef OS_LINUX
#include <glog/logging.h>
#endif

using namespace std;

void LOG5CXX_INIT(const char* v){
#ifdef OS_LINUX
    // Initialize Google's logging library.
    google::InitGoogleLogging(v);
#endif
}
void LOG5CXX_DEBUG(const char* msg){
#ifdef OS_LINUX
    DLOG(INFO) << msg;
    //DLOG_IF(INFO, num_cookies > 10) << "Got lots of cookies";
    //DLOG_EVERY_N(INFO, 10) << "Got the " << google::COUNTER << "th cookie";
#endif
cout << "<< DEBUG >> : " << msg << endl;
}
void LOG5CXX_INFO(const char* msg){
#ifdef OS_LINUX
    LOG(INFO) << msg;
#endif
cout << "<< INFO >> : " << msg << endl;
}
void LOG5CXX_WARN(const char* msg){
#ifdef OS_LINUX
    LOG(WARNING) << msg;
#endif
cerr << "<< WARN >> : " << msg << endl;
}
void LOG5CXX_ERROR(const char* msg){
#ifdef OS_LINUX
    LOG(ERROR) << msg;
    //LOG_IF(ERROR, x > y) << "This should be also OK";
#endif
cerr << "<< ERROR >> : " << msg << endl;
}
void LOG5CXX_FATAL(const char* msg, int code){
#ifdef OS_LINUX
    LOG(FATAL) << msg;
#endif
cerr << "<< FATAL >> : " << msg << endl;

    exit(code);    //terminal the app
}
