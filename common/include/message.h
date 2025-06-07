#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <vector>

// 消息类型枚举
enum class MessageType {
    // 连接相关
    CONNECT_REQUEST = 1,
    CONNECT_RESPONSE = 2,
    DISCONNECT = 3,

    // 用户相关
    USER_LOGIN = 10,
    USER_REGISTER = 11,
    USER_LOGOUT = 12,

    // 新的用户相关消息类型
    REGISTER_REQUEST = 13,
    REGISTER_RESPONSE = 14,
    LOGIN_REQUEST = 15,
    LOGIN_RESPONSE = 16,
    LOGOUT_REQUEST = 17,
    LOGOUT_RESPONSE = 18,

    // 账户管理相关
    CHANGE_PASSWORD_REQUEST = 19,
    CHANGE_PASSWORD_RESPONSE = 20,

    // 商品浏览相关
    PRODUCT_LIST_REQUEST = 30,
    PRODUCT_LIST_RESPONSE = 31,
    PRODUCT_SEARCH_REQUEST = 32,
    PRODUCT_SEARCH_RESPONSE = 33,
    PRODUCT_DETAIL_REQUEST = 34,
    PRODUCT_DETAIL_RESPONSE = 35,

    // 商家商品管理相关
    MERCHANT_ADD_PRODUCT_REQUEST = 36,
    MERCHANT_ADD_PRODUCT_RESPONSE = 37,
    MERCHANT_MODIFY_PRODUCT_REQUEST = 38,
    MERCHANT_MODIFY_PRODUCT_RESPONSE = 39,
    MERCHANT_PRODUCT_LIST_REQUEST = 40,
    MERCHANT_PRODUCT_LIST_RESPONSE = 41,
    MERCHANT_SET_DISCOUNT_REQUEST = 42,
    MERCHANT_SET_DISCOUNT_RESPONSE = 43,

    // 购物车相关
    CART_ADD_ITEM_REQUEST = 50,
    CART_ADD_ITEM_RESPONSE = 51,
    CART_VIEW_REQUEST = 52,
    CART_VIEW_RESPONSE = 53,
    CART_UPDATE_ITEM_REQUEST = 54,
    CART_UPDATE_ITEM_RESPONSE = 55,
    CART_REMOVE_ITEM_REQUEST = 56,
    CART_REMOVE_ITEM_RESPONSE = 57,
    CART_CLEAR_REQUEST = 58,
    CART_CLEAR_RESPONSE = 59,

    // 订单结算相关
    ORDER_CHECKOUT_REQUEST = 60,
    ORDER_CHECKOUT_RESPONSE = 61,

    // 订单查看相关
    ORDER_LIST_REQUEST = 62,
    ORDER_LIST_RESPONSE = 63,
    ORDER_DETAIL_REQUEST = 64,
    ORDER_DETAIL_RESPONSE = 65,

    // 响应消息
    SUCCESS_RESPONSE = 100,
    ERROR_RESPONSE = 101
};

// 网络消息结构
struct NetworkMessage {
    MessageType type;
    int length;
    std::string data;

    NetworkMessage() : type(MessageType::CONNECT_REQUEST), length(0) {}
    NetworkMessage(MessageType t, const std::string& d) : type(t), data(d), length(static_cast<int>(d.length())) {}

    // 序列化为字节流
    std::vector<char> serialize() const;

    // 从字节流反序列化
    static NetworkMessage deserialize(const std::vector<char>& buffer);
};

#endif