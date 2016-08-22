CC = g++
CXXFLAGS += -O3 -std=c++11 -Wall -shared -fPIC
C_FLAGS += -std=c++11
LIBNAME = libncs

SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj

LIB_INC = -larmadillo

SRCEXT      := cpp
DEPEXT      := d
OBJEXT      := o

headers    = $(wildcard $(INC_DIR)/*.h)
lib_hdr    = $(wildcard $(SRC_DIR)/*.h)
lib_src    = $(wildcard $(SRC_DIR)/*.cpp)
dist_files = $(headers) $(lib_hdr) $(lib_src)

OBJS :=  discrete_system.o system.o controller.o helpers.o ncs_simulation.o
OBJ_FILES =$(addprefix $(OBJ_DIR)/,$(OBJS))


$(OBJ_DIR)/%.$(OBJEXT): $(SRC_DIR)/%.$(SRCEXT)
	$(CC) $(C_FLAGS) -c -o $@ $<


$(LIBNAME).so: directories $(OBJ_FILES)
	$(CC) $(CXXFLAGS) -o $@ $(lib_src)
	cp $(SRC_DIR)/ncs_simulation.h $(INC_DIR)/ncs_simulation.h
	sudo cp $(LIBNAME).so /usr/local/lib
	sudo cp $(SRC_DIR)/ncs_simulation.h /usr/local/include


Test: 
	gcc $(C_FLAGS) -o MyTest Test.c -I$(INC_DIR) -L./ -lncs
	
directories:
	@mkdir -p $(SRC_DIR)
	@mkdir -p $(INC_DIR)	
	@mkdir -p $(OBJ_DIR)

clean:
	rm -rf *.o $(LIBNAME).a 

all: directories $(LIBNAME).a  $(OBJ_FILES)
.PHONY: all clean
