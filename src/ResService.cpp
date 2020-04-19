#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
//#include <sys/io.h>
#include <sys/stat.h> 
#include <dirent.h> 
#include <time.h>

#include "ResService.h"
#include "log5cxx.h"

#ifndef NULL
#define NULL 0
#endif

#ifndef MAX_PATH
#define MAX_PATH 2048
#endif

#define SLASH std::string("/")
#define SLASH_T "/"
#define SLASH_C '/'

using namespace std;

ResService::ResService(){
    srand((int)time(0));
    string sys_tmp_path = "/tmp/im2video";
    DIR *dirptr = NULL; 
    if((dirptr=opendir(sys_tmp_path.c_str())) == NULL){
        char command[MAX_PATH];
        sprintf(command, "mkdir -p %s", sys_tmp_path.c_str());
        system(command);
    }
    else
        closedir(dirptr);

    // generate a unique name for temp path with guid
    while(true){
        int64_t rand_int = (int64_t)(10000000000.0*rand()/(RAND_MAX+1.0));
        char rand_c[32];
        sprintf(rand_c, "tmp_%ld", rand_int);
        string rand_str(rand_c);

        tmp_path = sys_tmp_path + SLASH + rand_str + SLASH;
        //tmp_path = SLASH + rand_str + SLASH;

        DIR *dirptr = NULL; 
        if((dirptr=opendir(tmp_path.c_str())) == NULL){
            break;
        }
        closedir(dirptr);
    }

    string errMsg;
    int status = mkdir(tmp_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if(status != 0){
        errMsg = "Can't create folder " + tmp_path + " (" + strerror(errno) +").";
        LOG5CXX_FATAL(errMsg.c_str(), 121);
    }
}

ResService::~ResService(){
    clearTemp();
}

static std::string __getFileName(std::string& fullPath){
    int last_index_of = fullPath.length()-1;
    while(last_index_of >= 0){
        if(fullPath[last_index_of] == SLASH_C)
            break;
        last_index_of -= 1;
    }
    string name; 
    name.assign(fullPath, last_index_of+1, fullPath.length()-last_index_of-1);
    return name;
}
bool ResService::Filter(std::vector<animation_t>& args){
    int i, num = args.size();
    char command[MAX_PATH];

    string file, t1, t2;
    for(i=0; i<num; i++){
        for (int j=0; j<2; j++){
            if (j==0)
                t1 = args[i].A;
            else
                t1 = args[i].B;

            if(t1[0] == '/')
                continue;

            t2 = __getFileName(t1);
            file = tmp_path + t2.c_str();
            FILE *fp = fopen(file.c_str(), "r");

            if (j==0)
                strcpy(args[i].A, file.c_str());
            else
                strcpy(args[i].B, file.c_str());

            if (fp) {
                fclose(fp);
                fp = NULL;
            } else {
                sprintf(command, "/usr/bin/curl -o %s %s", file.c_str(), t1.c_str());
                int status = system(command);

                bool victory = false;
                if (status != -1){
                    if (WIFEXITED(status)){
                        if (0 == WEXITSTATUS(status)){
                            victory = true;
                        }
                    }
                }
                if (!victory) {
                    char msg[512];
                    sprintf(msg, "Download resource from remote error. [%s]", t1.c_str());
                    LOG5CXX_FATAL(msg, 121);
                }
            }
        }
    }
}

const std::string& ResService::getTmpDir() const {
    return tmp_path;
}

static void delete_dir(const char *path) { 
    DIR* dp = NULL; 
    DIR* dpin = NULL; 
    char pathname[1024];
    struct dirent* dirp; 
    dp = opendir(path); 
    if(dp == NULL) { 
        return; 
    }

    while((dirp = readdir(dp)) != NULL) { 
        if(strcmp(dirp->d_name, "..") == 0 || strcmp(dirp->d_name, ".") == 0) 
            continue; 
        strcpy(pathname, path); 
        strcat(pathname, SLASH_T); 
        strcat(pathname, dirp->d_name); 
        dpin = opendir(pathname); 
        if(dpin != NULL) 
            delete_dir(pathname); 
        else 
            remove(pathname); 
        strcpy(pathname, ""); 
        closedir(dpin); 
        dpin = NULL; 
    } 
    rmdir(path); 
    closedir(dp);
    dirp = NULL; 
} 
void ResService::clearTemp(){
    delete_dir(tmp_path.c_str());
    //remove(tmp_unzip_path.c_str());
    //remove(tmp_path.c_str());
}
