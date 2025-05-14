#include <gtest/gtest.h>
#include "shopping_cart.h"

class ShoppingCartTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 测试准备
    }
    
    void TearDown() override {
        // 测试清理
    }
};

TEST_F(ShoppingCartTest, AddItem) {
    // 测试添加商品
}

TEST_F(ShoppingCartTest, RemoveItem) {
    // 测试移除商品
}