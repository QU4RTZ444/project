#include "message.h"
#include <cstring>

std::vector<char> NetworkMessage::serialize() const {
    std::vector<char> buffer;
    
    // 添加消息类型 (4字节)
    int type_int = static_cast<int>(type);
    buffer.insert(buffer.end(), reinterpret_cast<const char*>(&type_int), 
                  reinterpret_cast<const char*>(&type_int) + sizeof(int));
    
    // 添加数据长度 (4字节)
    buffer.insert(buffer.end(), reinterpret_cast<const char*>(&length), 
                  reinterpret_cast<const char*>(&length) + sizeof(int));
    
    // 添加数据内容
    buffer.insert(buffer.end(), data.begin(), data.end());
    
    return buffer;
}

NetworkMessage NetworkMessage::deserialize(const std::vector<char>& buffer) {
    NetworkMessage msg;
    
    if (buffer.size() < 8) return msg; // 至少需要8字节头部
    
    // 读取消息类型
    int type_int;
    std::memcpy(&type_int, buffer.data(), sizeof(int));
    msg.type = static_cast<MessageType>(type_int);
    
    // 读取数据长度
    std::memcpy(&msg.length, buffer.data() + sizeof(int), sizeof(int));
    
    // 读取数据内容
    if (buffer.size() >= 8 + msg.length) {
        msg.data = std::string(buffer.begin() + 8, buffer.begin() + 8 + msg.length);
    }
    
    return msg;
}