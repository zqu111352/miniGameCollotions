#pragma once


#include <iostream>
#include <functional>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <any>
#include <typeindex>
#include <algorithm>

// 1. 定义消息总线类
class MessageBus {
public:
    // 获取单例实例 (可选，这里为了方便演示使用静态方法或直接实例化)
    static MessageBus& getInstance() {
        static MessageBus instance;
        return instance;
    }

    // 订阅消息
    // T: 消息类型
    // callback: 当收到类型为 T 的消息时执行的函数
    template<typename T>
    void subscribe(std::function<void(const T&)> callback) {
        // 使用 type_index 作为键，确保不同类型的消息不会冲突
        std::type_index typeId = std::type_index(typeid(T));
        
        // 将回调包装为 std::function<void(const std::any&)> 以统一存储
        auto wrappedCallback = [callback](const std::any& msg) {
            try {
                // 尝试将 any 转换回原始类型 T
                const T& actualMsg = std::any_cast<const T&>(msg);
                callback(actualMsg);
            } catch (const std::bad_any_cast& e) {
                std::cerr << "Error: Bad any_cast in message bus for type: " 
                          << typeid(T).name() << std::endl;
            }
        };

        subscribers_[typeId].push_back(wrappedCallback);
    }

    // 取消订阅 (简化版：清除某类型的所有订阅，实际项目中可能需要返回句柄来取消特定订阅)
    template<typename T>
    void unsubscribeAll() {
        std::type_index typeId = std::type_index(typeid(T));
        if (subscribers_.find(typeId) != subscribers_.end()) {
            subscribers_[typeId].clear();
        }
    }

    // 发送消息
    template<typename T>
    void emit(const T& message) {
        std::type_index typeId = std::type_index(typeid(T));
        
        if (subscribers_.find(typeId) == subscribers_.end()) {
            return; // 没有订阅者，直接返回
        }

        // 遍历并调用所有订阅该消息类型的回调
        for (auto& callback : subscribers_[typeId]) {
            callback(message);
        }
    }

private:
    MessageBus() = default;
    
    // 键: 消息类型的 type_index
    // 值: 该类型消息对应的回调函数列表
    std::unordered_map<std::type_index, std::vector<std::function<void(const std::any&)>>> subscribers_;
};