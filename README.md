im2video
========

create video with transition effects with image sequences.

[00 - INTRODUCTION]
The im2video convert tools is a simple to use, easy to entegrate
and extremely efficient video maker.

[01 - DIRECTORY]
├─Bin                   // Executable program
├─Branch                // Branch of whole project
├─Dependencies          // Third libraries and dependence library
│  ├─include
│  ├─lib
│  └─pkg                // Backup library
├─Doc                   // Some documents
└─Source                // Source code


1.  程序输入为input.xml文件路径，文件格式见input.xml
2.  程序进度和提示信息为： progress: [40 %], info: [the 4th animation is processing].
3.  日志、异常和错误信息在当前目录下log文件夹下，文件名为20140421-3415.log，
    前半部分为当天日期，后半部分为4位随机数，程序重新启动产生新日志文件，第二天再次产生日志文件。
4.  程序返回代码:
        0           成功
        1XX         IO错误
        2XX         内存错误
        3XX         逻辑或数据错误
        4XX         其他错误


im2video.exe -t 33d -i Doc/input_audi.xml