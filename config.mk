
export BUILD_ROOT = $(shell pwd)

export INCLUDE_PATH = $(BUILD_ROOT)/headers/

export MAKE_PATH = $(BUILD_ROOT)/net/ $(BUILD_ROOT)/proc/ $(BUILD_ROOT)/tools/ $(BUILD_ROOT)/logs/ $(BUILD_ROOT)/signal/ $(BUILD_ROOT)/threadpoll/ $(BUILD_ROOT)/app/ 

export DEBUG = true