CC      = g++
INCPATH = -Isrc/
LDPATH  = -lpthread -lncurses
STD     = -std=c++11
FLAGS   = ${INCPATH} ${LDPATH} ${STD}

SRC_DIR   = src
BUILD_DIR = build
EXE		  = secmes

SRCS  = $(wildcard $(SRC_DIR)/**/*.cpp) $(wildcard $(SRC_DIR)/*.cpp)
OBJS  = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

all: ${BUILD_DIR} ${EXE}

${BUILD_DIR}:
	$(shell mkdir -p $(dir $(OBJS)))

${EXE}: ${OBJS}
	${CC} -o $@ $^ ${FLAGS}

${BUILD_DIR}/%.o: ${SRC_DIR}/%.cpp
	${CC} -c $< ${FLAGS} -o $@

clean:
	rm -rf build
	rm -f ${EXE}
