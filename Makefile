DEBUG=0

CC=g++

SRC_DIR     = ./src
BUILD_DIR   = ./build
OBJ_DIR     = $(BUILD_DIR)/obj
TARGET_DIR  = $(BUILD_DIR)/bin
INSTALL_DIR = $(HOME)/bin

CFLAGS   = -MMD -MP -Wall -Wextra -pedantic -Wcast-align -Wcast-qual -Wconversion -Wdisabled-optimization -Wendif-labels -Wfloat-equal -Winit-self -Winline -Wlogical-op -Wmissing-include-dirs -Wpacked -Wpointer-arith -Wredundant-decls -Wshadow -Wswitch-default -Wswitch-enum -Wunsafe-loop-optimizations -Wvariadic-macros -Wwrite-strings
CXXFLAGS = $(CFLAGS) -std=c++14 -Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual -Wsign-promo

ifeq ($(DEBUG), 1)
    CFLAGS += -O0 -g3 -DDEBUG
else
    CFLAGS += -O2 -DNDEBUG
endif

SRCS    = $(wildcard $(SRC_DIR)/*.cc) $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*.c)
OBJS    = $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(notdir $(basename $(SRCS)))))
DEPS    = $(OBJS:.o=.d)
TARGETS = $(TARGET_DIR)/golomb

.PHONY: all clean

all: $(TARGETS)

# target
$(TARGET_DIR)/golomb: $(OBJ_DIR)/golomb.o
	@mkdir -p $(dir $@)
	$(CC) $< -o $@ $(LDFLAGS)

# object
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) -r $(BUILD_DIR)

-include mkfiles/gtest.mk
-include $(DEPS)

