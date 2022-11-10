CXX=g++
SRCS=$(wildcard *.cpp)
LIB=-L../../opensocket/lib
INC=-I../../opensocket/src
LIBS=-lopensocket -lssl -lcrypto -lpthread -lglog
TARGET=main 

all: $(TARGET)

$(TARGET):
		$(CXX) -o $(TARGET) $(SRCS) $(INC) $(LIB) $(LIBS)

clean:
		rm -f $(TARGET)
