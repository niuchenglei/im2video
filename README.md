# im2video

An effective video maker from image sequence and with transition effects between frames.

## Build from source

### For macos
Firstly, install dependencies with brew
```
brew install cmake opencv@2
brew install lapack glog libiconv
```
Compile to get the executable binary file
```
cd im2video
mkdir build && cd build
cmake ../src/
make
```

### For linux
### For windows


## How to use
Make sure the `effects` directory and the executable binary file in same directory.
```
./im2video --script ../example/input.xml --out out.avi
```
`input.xml` defines the images and the transition between frames need full fill by yourself, and it allow use an remote image like `http://lena.jpg`. 
The execution will generate temporally dir in `/tmp/` to store some images or logs.

### An example of input.xml
```
<?xml version="1.0" encoding="UTF-8" standalone="no" ?>
<ConfigurationSettings>
    <animation name="abc" height="748" width="1320" prefix="/Users/chenglei/nuclear/im2video/example/img/">  // it could be local directory or remote URI
        <effect>
            <name>none</name>
            <from>1.jpg</from> // it could be an direct file like /tmp/1.jpg
            <to>1.jpg</to>
            <duration>1.2</duration>
        </effect>
        <effect>
            <name>fadescale</name>
            <from>2.jpg</from>
            <to>3.jpg</to>
            <duration>2</duration>
        </effect>
        <effect>
            <name>none</name>
            <from>3.jpg</from>
            <to>3.jpg</to>
            <duration>2</duration>
        </effect>
    </animation>
</ConfigurationSettings>
```

```
Error code:
0           Success
1XX         IO error
2XX         Memory error
3XX         Logic or data error
4XX         Others
```