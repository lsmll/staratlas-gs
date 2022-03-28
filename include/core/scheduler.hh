/**
 * @file scheduler.hh
 * @author wenjie (wjie@zju.edu.cn)
 * @brief 宿主机调度器
 * @version 0.1
 * @date 2022-03-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <variant>
#include <vector>
#include <queue>
#include <stack>
#include <unordered_set>
#include <memory>

#include <core/runtime.hh>
#include <core/storage.hh>

namespace wasmtm {

class SchedulerQueue {
public:
    /**
     * @brief 调度队列类型
     * 
     */
    enum Type {
        FIFO = 1, // 先进先出，模拟BFS
        FILO = 2, // 先进后出，模拟DFS
        PRIO = 3, // 优先级高的先出，可用来模拟随机选择
    };

    SchedulerQueue(SchedulerQueue::Type type);

    /**
     * @brief 将节点id放入调度队列
     * 
     * @param nid 节点id
     * @param p 优先级
     */
    void push(nid_t nid, f64 p = 0.0);

    /**
     * @brief 从调度队列里弹出节点，并返回
     * 
     * @return nid_t 节点id
     */
    nid_t pop();
    
    size_t size() const;
    bool empty() const;
    void clear();

private:
    std::variant<
        std::queue<nid_t>,
        std::stack<nid_t>,
        std::priority_queue<std::pair<f64, nid_t>>
    > _Q;

    Type _type;

    void init_queue();
};

class SchedulerBlock {
    std::unique_ptr<SchedulerQueue> _Q;
    std::unique_ptr<std::unordered_set<nid_t>> _distinct;
    std::unique_ptr<std::unordered_set<nid_t>> _selected;
    std::shared_ptr<Storage> _storage;

    std::unique_ptr<Runtime> _wasm;
public:
    /**
     * @brief 创建调度器
     * 
     * @param type 调度队列类型
     * @param storage 后端存储访问器
     * @param distinct 调度队列是否去重
     */
    SchedulerBlock(SchedulerQueue::Type type, std::shared_ptr<Storage> storage, bool distinct = true);
    
    /**
     * @brief 初始化WASM容器
     * 
     * @param wasm_bytes WASM字节码
     * @param pages WASM内存页数，上限为65536
     * @return true 
     * @return false 
     */
    bool initialize(std::span<u8> wasm_bytes, size_t pages = 65536);

    /**
     * @brief 将节点放入调度队列，一般用于设置起始节点
     * 
     * @param nid 
     * @param p 
     */
    void push(nid_t nid, f64 p = 0.0);

    /**
     * @brief 清除WASM容器外的数据（重置）
     * 
     */
    void clear();

    /**
     * @brief 推进采样流程，可多次调用并返回
     * 
     * @return true 调度队列不为空，需要继续执行
     * @return false 调度队列为空，可返回
     * 
     * @code {.c++}
     * while (obj.poll()); // 连续调用直到无法推进
     * @endcode
     * 
     */
    bool poll();

    /**
     * @brief 在单次采样结束之后调用，用于重置WASM容器状态，并返回一些数据到宿主机
     * 
     */
    void finish();

    /**
     * @brief 返回被选中的节点id
     * 
     * @return std::vector<nid_t> 
     */
    std::vector<nid_t> selected();

    /**
     * @brief poll + finish
     * 
     */
    void join();

    /**
     * @brief join + clear + selected，无状态采样
     * 
     * @param s 起始节点
     * @param p 优先级
     * @return std::vector<nid_t> 
     */
    std::vector<nid_t> sample(nid_t s, f64 p = 0.0);
    
    inline operator bool() const {
        return static_cast<bool>(this->_wasm);
    }
};

}