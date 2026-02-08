/*
*  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source tree.
*/
/*****************************************************************************
				  Author: chensong
				  date:  2026-02-09
				  
				  SIP消息处理基类
******************************************************************************/

#ifndef _C_SIP_MESSAGE_HANDLER_HPP
#define _C_SIP_MESSAGE_HANDLER_HPP

#include <string>
#include <functional>
#include <unordered_map>
#include <memory>
#include <atomic>
#include <thread>
#include <chrono>

extern "C" {
#include <eXosip2/eXosip.h>
#include <osip2/osip.h>
}

namespace gbsip_server {

/**
 * 响应回调结构
 */
struct ResponseCallback {
    std::function<void(const std::string& xml)> onSuccess;
    std::function<void(int code, const std::string& message)> onError;
    std::chrono::steady_clock::time_point startTime;
    int timeout; // 超时时间（秒）
};

/**
 * SIP消息处理基类
 * 提供MESSAGE和SUBSCRIBE消息的发送、响应处理、超时管理等基础功能
 */
class SipMessageHandler {
public:
    /**
     * 构造函数
     * @param context eXosip上下文
     * @param serverInfo SIP服务器信息
     */
    SipMessageHandler(eXosip_t* context, 
                      const std::string& serverId,
                      const std::string& serverIp,
                      int serverPort);
    
    /**
     * 析构函数
     */
    virtual ~SipMessageHandler();
    
protected:
    /**
     * 发送MESSAGE消息
     * @param deviceId 目标设备ID
     * @param deviceIp 目标设备IP
     * @param devicePort 目标设备端口
     * @param xml XML消息体
     * @param callback 响应回调
     * @param timeout 超时时间（秒），默认30秒
     * @return 序列号，失败返回空字符串
     */
    std::string sendMessage(const std::string& deviceId,
                            const std::string& deviceIp,
                            int devicePort,
                            const std::string& xml,
                            ResponseCallback callback,
                            int timeout = 30);
    
    /**
     * 发送SUBSCRIBE消息
     * @param deviceId 目标设备ID
     * @param deviceIp 目标设备IP
     * @param devicePort 目标设备端口
     * @param event 订阅事件类型
     * @param expires 过期时间（秒）
     * @param callback 响应回调
     * @return 订阅ID，失败返回-1
     */
    int sendSubscribe(const std::string& deviceId,
                      const std::string& deviceIp,
                      int devicePort,
                      const std::string& event,
                      int expires,
                      ResponseCallback callback);
    
    /**
     * 处理MESSAGE响应
     * @param event eXosip事件
     */
    void handleMessageResponse(eXosip_event_t* event);
    
    /**
     * 处理SUBSCRIBE响应
     * @param event eXosip事件
     */
    void handleSubscribeResponse(eXosip_event_t* event);
    
    /**
     * 处理NOTIFY消息
     * @param event eXosip事件
     */
    virtual void handleNotify(eXosip_event_t* event) = 0;
    
    /**
     * 生成序列号
     * @return 序列号字符串
     */
    std::string generateSN();
    
    /**
     * 从XML中提取序列号
     * @param xml XML字符串
     * @return 序列号
     */
    std::string extractSN(const std::string& xml);
    
    /**
     * 检查超时
     */
    void checkTimeout();
    
    /**
     * 启动超时检查线程
     */
    void startTimeoutChecker();
    
    /**
     * 停止超时检查线程
     */
    void stopTimeoutChecker();

protected:
    // eXosip上下文
    eXosip_t* sip_context_;
    
    // SIP服务器信息
    std::string server_id_;
    std::string server_ip_;
    int server_port_;
    
    // SN -> Callback映射
    std::unordered_map<std::string, ResponseCallback> callbacks_;
    std::mutex callbacks_mutex_;
    
    // 序列号生成器
    std::atomic<int> sn_generator_;
    
    // 超时检查线程
    std::thread timeout_checker_thread_;
    std::atomic<bool> timeout_checker_running_;
};

} // namespace gbsip_server

#endif /* _C_SIP_MESSAGE_HANDLER_HPP */
