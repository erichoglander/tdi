#The parent src path
SRCPATH = ../../src
LIBPATH = ../../lib

#OBJS specifies which files to compile as part of the project 
OBJS = src/main.cpp $(SRCPATH)/tdilib.cpp $(SRCPATH)/tdilib_client.cpp

#CC specifies which compiler we're using 
CC = g++ -std=c++11

#COMPILER_FLAGS specifies the additional compilation options we're using 
# -w suppresses all warnings 
COMPILER_FLAGS = 

#LINKER_FLAGS specifies the libraries we're linking against 
LINKER_FLAGS = -I$(SRCPATH) -I$(LIBPATH)

#OBJ_NAME specifies the name of our exectuable 
OBJ_NAME = tdi

#This is the target that compiles our executable 
all : $(OBJS) 
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME) 