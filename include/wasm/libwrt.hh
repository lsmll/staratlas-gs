/**
 * @file libwrt.hh
 * @author wenjie (wjie@zju.edu.cn)
 * @brief WASM容器程序运行库
 * @version 0.1
 * @date 2022-03-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <string>
#include <wasm/buffer.hh>
#include <wasm/macros.hh>

namespace wasmtm {

// /**
//  * @brief 初始化容器时设置调度队列属性
//  * 
//  */
// struct DISTINCT {
//     DISTINCT(bool flag);
// };

/**
 * @brief 向终端输出日志，用于调试
 * 
 * @param s 
 * 
 * @todo 完整的分级日志功能
 */
void console_log(const std::string_view s);

/**
 * @brief 从宿主机下载节点
 * 
 * @param nid 
 * @return DataBuffer 
 */
DataBuffer get_node(nid_t nid);

/**
 * @brief 选中节点，作为结果返回
 * 
 * @param nid 
 */
void select_node(nid_t nid);
void select_node(std::span<nid_t> nids);

/**
 * @brief 将节点添加到候选队列里
 * 
 * @param nid 节点id
 * @param p 优先级
 */
void extend_node(nid_t nid);
void extend_node(nid_t nid, f64 p);
void extend_node(std::span<nid_t> nids);
void extend_node(std::span<nid_t> nids, std::span<f64> ps);

/**
 * @brief 导入外部时钟，毫秒级
 * 
 * @return u64 
 */
u64 clk_m();

}
/**
 * @brief 洁癖
 * 
 */
namespace wasm = wasmtm;
