# im2video

An effective video maker from image sequence and with transition effects between frames.

## Build from source

### For macos
```
brew install cmake opencv@2
brew install lapack glog libiconv
```

### For linux
### For windows


## How to use
Step 1. Configurate images and the transition between frames in `xml` file.

Step 2. Execute the im2video with the configuration file and get output video file.

```
Error code:
0           Success
1XX         IO error
2XX         Memory error
3XX         Logic or data error
4XX         Others
```