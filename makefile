CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -D__linux__
DEBUGFLAGS = -DDEBUG -g -O0
RELEASEFLAGS = -O2 -DNDEBUG
INCLUDES = -Iinclude
LIBS = -lsqlite3

# 目标文件目录
BUILD_DIR = build
BIN_DIR = bin
RELEASE_DIR = $(BIN_DIR)/release

# 源文件
SRCS = $(wildcard src/*.cpp)
OBJS = $(SRCS:src/%.cpp=$(BUILD_DIR)/%.o)
RELEASE_OBJS = $(SRCS:src/%.cpp=$(BUILD_DIR)/release/%.o)

# 可执行文件
TARGET = $(BIN_DIR)/ecommerce
RELEASE_TARGET = $(RELEASE_DIR)/ecommerce

# 默认目标
all: debug

# Debug版本
debug: CXXFLAGS += $(DEBUGFLAGS)
debug: $(TARGET)

# Release版本
release: CXXFLAGS += $(RELEASEFLAGS)
release: $(RELEASE_TARGET)

# 创建目录
$(BUILD_DIR):
		mkdir -p $(BUILD_DIR)
		mkdir -p $(BUILD_DIR)/release

$(BIN_DIR):
		mkdir -p $(BIN_DIR)
		mkdir -p $(RELEASE_DIR)

# Debug版本编译规则
$(BUILD_DIR)/%.o: src/%.cpp | $(BUILD_DIR)
		$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Release版本编译规则
$(BUILD_DIR)/release/%.o: src/%.cpp | $(BUILD_DIR)
		$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Debug版本链接规则
$(TARGET): $(OBJS) | $(BIN_DIR)
		$(CXX) $(OBJS) -o $(TARGET) $(LIBS)
	@echo "Debug版本构建完成: $(TARGET)"

# Release版本链接规则
$(RELEASE_TARGET): $(RELEASE_OBJS) | $(BIN_DIR)
		$(CXX) $(RELEASE_OBJS) -o $(RELEASE_TARGET) $(LIBS)
		strip $(RELEASE_TARGET)
	@echo "Release版本构建完成: $(RELEASE_TARGET)"

# 打包规则
dist: release
		mkdir -p $(RELEASE_DIR)/dist
		cp $(RELEASE_TARGET) $(RELEASE_DIR)/dist/
		cp -r data $(RELEASE_DIR)/dist/
		cd $(RELEASE_DIR) && tar czf ecommerce.tar.gz dist
	@echo "发布包已创建: $(RELEASE_DIR)/ecommerce.tar.gz"

# 清理规则
clean:
		rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all debug release clean dist test