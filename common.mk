ifeq ($(DEBUG), true)
    CC = g++ -g -std=c++11
    VERSION = DEBUG
else
    CC = g++ -std=c++11
    VERSION = RELEASE
endif

# 扫描当前目录下的所有.cpp文件 
#sigleton.cpp
CPPS = $(wildcard *.cpp)

#OBJS 文件名
#sigleton.o
OBJS = $(CPPS:.cpp=.o)

#链接文件
#sigleton.d
DEPS := $(CPPS:.cpp=.d)

#存放.d文件的地址
DLINK_OBJ_DIR = $(BUILD_ROOT)/app/dlink_obj

# 存放.o文件的地址
OOBJ_DIR = $(BUILD_ROOT)/app/oobj

# ./oobj/sigleton.o
OBJS := $(addprefix $(OOBJ_DIR)/,$(OBJS))

# ./dlink_obj/sigleton.d
DEPS := $(addprefix $(DLINK_OBJ_DIR)/,$(DEPS))

BIN := $(addprefix $(BUILD_ROOT)/,$(BIN))

LINK_OBJ = $(wildcard $(OOBJ_DIR)/*.o)
# @echo "变量Link_obj：$(LINK_OBJ)"
	
LINK_OBJ += $(OBJS)
# @echo "变量obj：$(OBJS)"


all:$(BIN) $(OBJS) $(DEPS) 
    #如果不为空,$(wildcard)是函数【获取匹配模式文件名】，这里 用于比较是否为""
ifneq ("$(wildcard $(DEPS))","")
include $(DEPS)
endif

$(BIN):$(LINK_OBJ)
	@echo "------------------------build $(VERSION) mode--------------------------------!!!"
	@echo $(BIN)
#一些变量：$@：目标，     $^：所有目标依赖
# gcc -o 是生成可执行文件
	$(CC) -llog4cpp -o $@ $^ 

$(OOBJ_DIR)/%.o:%.cpp
	$(CC) -I$(INCLUDE_PATH) -c $^ -o $@

$(DLINK_OBJ_DIR)/%.d:%.cpp
	$(CC) -I$(INCLUDE_PATH) -MM $^ >> $@

