cmake_minimum_required(VERSION 2.6)
project(im2video)

set(CMAKE_INCLUDE_CURRENT_DIR ON)


#############################################
# OpenMP support
option(USE_OPENMP "use openmp as multi-thread implement" ON)

# Compiler variables
set(BUILD_SHARED_LIBS ON)
set(BUILD_DEPENDENCE_DIR ${PROJECT_SOURCE_DIR}/../deps/lib)
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-w -O3 -W -Wall")
set(CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS})
set(CMAKE_INSTALL_PREFIX $HOME/im2video)
set(CMAKE_SHARED_LINKER_FLAGS -fopenmp)
set(CMAKE_EXE_LINKER_FLAGS "-L${BUILD_DEPENDENCE_DIR} -L/usr/lib64 -L/usr/local/opt/opencv@2/lib")
set(BUILD_BIN_DIR ${PROJECT_SOURCE_DIR}/../build)		# niuniu, this value can be defferent rely on developer

# Compiler options
add_definitions()
#############################################


#############################################
# Compile file option
# Get whole source files that need to compile
aux_source_directory(. SOURCE_FILES)
aux_source_directory(xml SOURCE_FILES_XML)
set(SOURCE_FILES ${SOURCE_FILES} ${SOURCE_FILES_XML})

# Compile application to executable program
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

add_library(blas SHARED IMPORTED)
add_library(lapack SHARED IMPORTED)
add_library(levmar STATIC IMPORTED)
add_library(glog SHARED IMPORTED)
add_library(iconv SHARED IMPORTED)
set_property(TARGET blas PROPERTY IMPORTED_LOCATION /usr/local/opt/lapack/lib/libblas.dylib)
set_property(TARGET lapack PROPERTY IMPORTED_LOCATION /usr/local/opt/lapack/lib/liblapack.dylib)
set_property(TARGET levmar PROPERTY IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/../deps/lib/liblevmar.a)
set_property(TARGET glog PROPERTY IMPORTED_LOCATION /usr/local/opt/glog/lib/libglog.dylib)
set_property(TARGET iconv PROPERTY IMPORTED_LOCATION /usr/local/opt/libiconv/lib/libiconv.dylib)
#############################################


#############################################
# Dependence library setting
target_link_libraries(${PROJECT_NAME}
pthread
levmar
blas
lapack
blas
opencv_core
opencv_features2d
opencv_imgproc
opencv_flann
opencv_highgui
opencv_nonfree
opencv_calib3d
opencv_contrib
opencv_legacy
opencv_photo
opencv_video
glog
iconv
)

# Properties setting
set_target_properties(${PROJECT_NAME} PROPERTIES
RUNTIME_OUTPUT_DIRECTORY "${BUILD_BIN_DIR}"
ARCHIVE_OUTPUT_DIRECTORY "${BUILD_BIN_DIR}"
)

# Include path
include_directories(
${PROJECT_SOURCE_DIR}
${PROJECT_SOURCE_DIR}/../deps/include
/usr/local/opt/opencv@2/include
/usr/local/include
)


#############################################
