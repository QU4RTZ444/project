# 编译器设置
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# 目录设置
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
BIN_DIR = bin

# 源文件和目标文件
SRCS = $(shell find $(SRC_DIR) -name "*.cpp")
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
TARGET = $(BIN_DIR)/ecommerce

# 数据库相关
LIBS = -lsqlite3

# 确保目录存在
$(shell mkdir -p $(BUILD_DIR) $(BIN_DIR))

# 默认目标
all: $(TARGET)

# 链接目标文件
$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	$(CXX) $(OBJS) -o $(TARGET) $(LIBS)
	@echo "编译完成: $(TARGET)"

# 编译源文件
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

# 清理编译产物
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "清理完成"

# 运行程序
run: $(TARGET)
	./$(TARGET)

# 创建发布版本
release: CXXFLAGS += -O2 -DNDEBUG
release: clean all

# 创建调试版本
debug: CXXFLAGS += -g -DDEBUG
debug: clean all

# 声明伪目标
.PHONY: all clean run release debug

# 显示帮助信息
help:
	@echo "可用的 make 命令："
	@echo "  make        - 构建项目"
	@echo "  make clean  - 清理构建文件"
	@echo "  make run    - 运行程序"
	@echo "  make debug  - 构建调试版本"
	@echo "  make release- 构建发布版本"