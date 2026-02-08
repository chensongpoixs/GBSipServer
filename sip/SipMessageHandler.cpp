/*
*  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source tree.
*/
/*****************************************************************************
				  Author: chensong
				  date:  2026-02-09
******************************************************************************/

#include "SipMessageHandler.hpp"
#include "utils/XmlHelper.hpp"
#include "gbsip_server_log.h"
#include <sstream>

namespace gbsip_server {

SipMessageHandler::SipMessageHandler(eXosip_t* context,
                                     const std::string& serverId,
                                     const std::string& serverIp,
                                     int serverPort)
    : sip_context_(context)
    , server_id_(serverId)
    , server_ip_(serverIp)
    , server_port_(serverPort)
    , sn_generator_(1)
    , timeout_checker_running_(false)
{
    startTimeoutChecker();
}

SipMessageHandler::~SipMessageHandler() {
    stopTimeoutChecker();
}

std::string SipMessageHandler::sendMessage(const std::string& deviceId,
                                            const std::string& deviceIp,
                                            int devicePort,
                                            const std::string& xml,
                                            ResponseCallback callback,
                                            int timeout) {
    if (!sip_context_) {
        SIPSERVER_LOG(LS_ERROR) << "SIP context is null";
        if (callback.onError) {
            callback.onError(500, "SIP context is null");
        }
        return "";
    }
    
    // 提取SN
    std::string sn = extractSN(xml);
    if (sn.empty()) {
        SIPSERVER_LOG(LS_ERROR) << "Failed to extract SN from XML";
        if (callback.onError) {
            callback.onError(500, "Invalid XML: missing SN");
        }
        return "";
    }
    
    // 构建URI
    std::ostringstream to_uri, from_uri;
    to_uri << "sip:" << deviceId << "@" << deviceIp << ":" << devicePort;
    from_uri << "sip:" << server_id_ << "@" << server_ip_ << ":" << server_port_;
    
    osip_message_t* message = nullptr;
    
    // 锁定eXosip
    eXosip_lock(sip_context_);
    
    // 构建MESSAGE请求
    int ret = eXosip_message_build_request(sip_context_, &message, "MESSAGE",
                                           to_uri.str().c_str(),
                                           from_uri.str().c_str(),
                                           nullptr);
    
    if (ret != 0 || !message) {
        eXosip_unlock(sip_context_);
        SIPSERVER_LOG(LS_ERROR) << "Failed to build MESSAGE request";
        if (callback.onError) {
            callback.onError(500, "Failed to build MESSAGE request");
        }
        return "";
    }
    
    // 设置Content-Type
    osip_message_set_content_type(message, "Application/MANSCDP+xml");
    
    // 设置消息体
    osip_message_set_body(message, xml.c_str(), xml.length());
    
    // 发送消息
    ret = eXosip_message_send_request(sip_context_, message);
    
    eXosip_unlock(sip_context_);
    
    if (ret < 0) {
        SIPSERVER_LOG(LS_ERROR) << "Failed to send MESSAGE request";
        if (callback.onError) {
            callback.onError(500, "Failed to send MESSAGE request");
        }
        return "";
    }
    
    // 保存回调
    {
        std::lock_guard<std::mutex> lock(callbacks_mutex_);
        callback.startTime = std::chrono::steady_clock::now();
        callback.timeout = timeout;
        callbacks_[sn] = callback;
    }
    
    SIPSERVER_LOG(LS_INFO) << "Sent MESSAGE to device: " << deviceId 
                           << ", SN: " << sn;
    
    return sn;
}

int SipMessageHandler::sendSubscribe(const std::string& deviceId,
                                      const std::string& deviceIp,
                                      int devicePort,
                                      const std::string& event,
                                      int expires,
                                      ResponseCallback callback) {
    if (!sip_context_) {
        SIPSERVER_LOG(LS_ERROR) << "SIP context is null";
        if (callback.onError) {
            callback.onError(500, "SIP context is null");
        }
        return -1;
    }
    
    // 构建URI
    std::ostringstream to_uri, from_uri;
    to_uri << "sip:" << deviceId << "@" << deviceIp << ":" << devicePort;
    from_uri << "sip:" << server_id_ << "@" << server_ip_ << ":" << server_port_;
    
    osip_message_t* subscribe = nullptr;
    
    // 锁定eXosip
    eXosip_lock(sip_context_);
    
    // 构建SUBSCRIBE请求
    int ret = eXosip_subscribe_build_initial_request(sip_context_, &subscribe,
                                                      to_uri.str().c_str(),
                                                      from_uri.str().c_str(),
                                                      nullptr,
                                                      event.c_str(),
                                                      expires);
    
    if (ret != 0 || !subscribe) {
        eXosip_unlock(sip_context_);
        SIPSERVER_LOG(LS_ERROR) << "Failed to build SUBSCRIBE request";
        if (callback.onError) {
            callback.onError(500, "Failed to build SUBSCRIBE request");
        }
        return -1;
    }
    
    // 发送订阅
    int sid = eXosip_subscribe_send_initial_request(sip_context_, subscribe);
    
    eXosip_unlock(sip_context_);
    
    if (sid < 0) {
        SIPSERVER_LOG(LS_ERROR) << "Failed to send SUBSCRIBE request";
        if (callback.onError) {
            callback.onError(500, "Failed to send SUBSCRIBE request");
        }
        return -1;
    }
    
    // 保存回调（使用sid作为key）
    {
        std::lock_guard<std::mutex> lock(callbacks_mutex_);
        callback.startTime = std::chrono::steady_clock::now();
        callback.timeout = 30;
        callbacks_[std::to_string(sid)] = callback;
    }
    
    SIPSERVER_LOG(LS_INFO) << "Sent SUBSCRIBE to device: " << deviceId
                           << ", event: " << event
                           << ", expires: " << expires
                           << ", sid: " << sid;
    
    return sid;
}

void SipMessageHandler::handleMessageResponse(eXosip_event_t* event) {
    if (!event || !event->request) {
        return;
    }
    
    // 获取消息体
    osip_body_t* body = nullptr;
    osip_message_get_body(event->request, 0, &body);
    if (!body || !body->body) {
        SIPSERVER_LOG(LS_WARNING) << "MESSAGE response has no body";
        return;
    }
    
    std::string xml(body->body);
    
    // 提取SN
    std::string sn = extractSN(xml);
    if (sn.empty()) {
        SIPSERVER_LOG(LS_WARNING) << "Failed to extract SN from response";
        return;
    }
    
    // 查找回调
    ResponseCallback callback;
    {
        std::lock_guard<std::mutex> lock(callbacks_mutex_);
        auto it = callbacks_.find(sn);
        if (it == callbacks_.end()) {
            SIPSERVER_LOG(LS_WARNING) << "No callback found for SN: " << sn;
            return;
        }
        callback = it->second;
        callbacks_.erase(it);
    }
    
    // 调用成功回调
    if (callback.onSuccess) {
        callback.onSuccess(xml);
    }
    
    SIPSERVER_LOG(LS_INFO) << "Received MESSAGE response, SN: " << sn;
}

void SipMessageHandler::handleSubscribeResponse(eXosip_event_t* event) {
    if (!event) {
        return;
    }
    
    int sid = event->sid;
    std::string key = std::to_string(sid);
    
    // 查找回调
    ResponseCallback callback;
    {
        std::lock_guard<std::mutex> lock(callbacks_mutex_);
        auto it = callbacks_.find(key);
        if (it == callbacks_.end()) {
            SIPSERVER_LOG(LS_WARNING) << "No callback found for sid: " << sid;
            return;
        }
        callback = it->second;
        callbacks_.erase(it);
    }
    
    // 调用成功回调
    if (callback.onSuccess) {
        callback.onSuccess("");
    }
    
    SIPSERVER_LOG(LS_INFO) << "Received SUBSCRIBE response, sid: " << sid;
}

std::string SipMessageHandler::generateSN() {
    int sn = sn_generator_.fetch_add(1);
    return std::to_string(sn);
}

std::string SipMessageHandler::extractSN(const std::string& xml) {
    auto root = XmlHelper::parseXml(xml);
    if (!root) {
        return "";
    }
    
    // 尝试从Query节点提取
    std::string sn = XmlHelper::getNodeValue(root, "Query/SN");
    if (!sn.empty()) {
        return sn;
    }
    
    // 尝试从Response节点提取
    sn = XmlHelper::getNodeValue(root, "Response/SN");
    if (!sn.empty()) {
        return sn;
    }
    
    // 尝试从Control节点提取
    sn = XmlHelper::getNodeValue(root, "Control/SN");
    if (!sn.empty()) {
        return sn;
    }
    
    return "";
}

void SipMessageHandler::checkTimeout() {
    auto now = std::chrono::steady_clock::now();
    
    std::vector<std::string> timeoutKeys;
    std::vector<ResponseCallback> timeoutCallbacks;
    
    {
        std::lock_guard<std::mutex> lock(callbacks_mutex_);
        
        for (const auto& pair : callbacks_) {
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                now - pair.second.startTime).count();
            
            if (elapsed >= pair.second.timeout) {
                timeoutKeys.push_back(pair.first);
                timeoutCallbacks.push_back(pair.second);
            }
        }
        
        // 移除超时的回调
        for (const auto& key : timeoutKeys) {
            callbacks_.erase(key);
        }
    }
    
    // 调用超时回调
    for (const auto& callback : timeoutCallbacks) {
        if (callback.onError) {
            callback.onError(408, "Request Timeout");
        }
    }
    
    if (!timeoutKeys.empty()) {
        SIPSERVER_LOG(LS_WARNING) << "Timeout " << timeoutKeys.size() << " requests";
    }
}

void SipMessageHandler::startTimeoutChecker() {
    timeout_checker_running_ = true;
    
    timeout_checker_thread_ = std::thread([this]() {
        while (timeout_checker_running_) {
            checkTimeout();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });
}

void SipMessageHandler::stopTimeoutChecker() {
    timeout_checker_running_ = false;
    
    if (timeout_checker_thread_.joinable()) {
        timeout_checker_thread_.join();
    }
}

} // namespace gbsip_server
