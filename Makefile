# Makefile with auto-dependency check
#
# Project Directory Structure
#   .
#   ....source/        --->  All source files
#   .
#   ....include/       --->  All header files
#   .
#   ....build/         --->  object, dependency and binary files
#   
# author : Vinas

TARGET = Flash
VERBOSE = FALSE

INC_DIR := include/
SRC_DIR := source/
BUILD_DIR := build/

INCS := $(wildcard $(INC_DIR)*.h)
SRCS := $(wildcard $(SRC_DIR)*.c)
OBJS := $(addprefix $(BUILD_DIR),$(notdir $(SRCS:.c=.o)))
DEPS := $(addprefix $(BUILD_DIR),$(notdir $(SRCS:.c=.d)))

CC=gcc
CFLAGS = -Wall -Werror -c -I$(INC_DIR)
DEPFLAGS = -MMD -MP -MF"$(@:%.o=%.d)"

$(BUILD_DIR)$(TARGET): $(OBJS) 
	$(CC) -o $@ $(OBJS)

$(BUILD_DIR)%.o: $(SRC_DIR)%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(DEPFLAGS) $< -o $@ 

$(BUILD_DIR):
	mkdir $@

-include $(DEPS)

.PHONY: run
.PHONY: clean

run:
	-./build/$(TARGET) Empty.bin

clean:
	rm -fR $(BUILD_DIR)
