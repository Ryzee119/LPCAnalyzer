TARGET  := libLPC.so

LINK := -L "../../lib/Linux" -lAnalyzer

CC       := g++
HFILE    := ../../inc/*.h
SRC      := ../src/*.cpp
INC      := -I ../../inc/
CXXFLAGS := -Wall -O2 -c
FPIC     := -fPIC
SHARE    := -shared -o
OBJ      := *.o
cpp		 :=  -std=c++11

$(TARGET) : $(HFILe) $(SRC)
	$(CC) $(CXXFLAGS) $(CPP_STD) $(FPIC) $(SRC) $(INC)
	$(CC) $(SHARE) $(TARGET) $(OBJ) $(LINK)

clean:
	rm *.so
	rm *.o
