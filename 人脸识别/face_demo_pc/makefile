CC=aarch64-linux-gnu-gcc
TARGET=main
#SOURCE=./src/$(wildcard *.c) ./src/$(wildcard *.cpp)
SOURCE=main.cpp SocketMatTransmissionClient.cpp detector.cpp chnsPyramid.cpp ObjDetector.cpp
INCLUDE=-I./include
LIB=-L./lib  -lwiringPi -Wl,-Bstatic  -lopenblas_armv8p-r0.2.20.dev  -Wl,-Bdynamic  -lstdc++ -lopencv_core  -lopencv_highgui -lopencv_imgproc -lm -lpthread -fpermissive -fopenmp -O3

all:
	$(CC) -std=c++11 -o $(TARGET) $(SOURCE) $(INCLUDE) $(LIB)  
