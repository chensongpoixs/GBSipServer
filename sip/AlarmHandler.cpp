/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Author: chensong
 *  Date: 2026-02-09
 *  Description: 报警SIP处理器实现
 ******************************************************************************/

#include "AlarmHandler.hpp"
#include "gbsip_server_log.h"
#include <sstream>

namespace gbsip_server {

AlarmHandler::AlarmHandler(eXosip_t* context,
                           const std::string& serverId,
                           const std::string& serverIp,
                           int serverPort)
    : sip_context_(context)
    , server_id_(serverId)
    , server_ip_(serverIp)
    , server_port_(serverPort)
    , sn_counter_(1)
{
    SIPSERVER_LOG(LS_INFO) << "AlarmHandler initialized";
}

AlarmHandler::~AlarmHandler() {
    SIPSERVER_LOG(LS_INFO) << "AlarmHandler destroyed";
}

int AlarmHandler::subscribeAlarm(
    const std::string& deviceId,
    int startPriority,
    int endPriority,
    int alarmMethod,
    int expires,
    const std::string& deviceIp,
    int devicePort)
{
    // 构建报警订阅XML
    std::string xml = buildAlarmSubscribeXml(deviceId, startPriority, 
                                              endPriority, alarmMethod);
    
    // 发送SUBSCRIBE请求
    int dialogId = sendSubscribe(deviceId, deviceIp, devicePort, xml, expires);
    
    if (dialogId > 0) {
        // 保存订阅信息
        subscriptions_[deviceId] = dialogId;
        SIPSERVER_LOG(LS_INFO) << "Alarm subscribed, device: " << deviceId 
                               << ", dialog_id: " << dialogId;
    } else {
        SIPSERVER_LOG(LS_WARNING) << "Failed to subscribe alarm, device: " << deviceId;
    }
    
    return dialogId;
}

bool AlarmHandler::unsubscribeAlarm(int dialogId) {
    eXosip_lock(sip_context_);
    int ret = eXosip_subscription_send_refresh(sip_context_, dialogId, 0);
    eXosip_unlock(sip_context_);
    
    if (ret == 0) {
        // 从订阅表中移除
        for (auto it = subscriptions_.begin(); it != subscriptions_.end(); ++it) {
            if (it->second == dialogId) {
                subscriptions_.erase(it);
                break;
            }
        }
        SIPSERVER_LOG(LS_INFO) << "Alarm unsubscribed, dialog_id: " << dialogId;
        return true;
    }
    
    SIPSERVER_LOG(LS_WARNING) << "Failed to unsubscribe alarm, dialog_id: " << dialogId;
    return false;
}

std::string AlarmHandler::queryAlarm(
    const std::string& deviceId,
    const std::string& startTime,
    const std::string& endTime,
    int startPriority,
    int endPriority,
    int alarmMethod,
    const std::string& deviceIp,
    int devicePort)
{
    // 构建报警查询XML
    std::string xml = buildAlarmQueryXml(deviceId, startTime, endTime,
                                          startPriority, endPriority, alarmMethod);
    
    // 发送MESSAGE请求
    std::string sn = sendMessage(deviceId, deviceIp, devicePort, xml);
    
    SIPSERVER_LOG(LS_INFO) << "Query alarm sent, SN: " << sn;
    
    return sn;
}

void AlarmHandler::handleAlarmNotify(eXosip_event_t* event) {
    if (!event || !event->request) {
        return;
    }
    
    // 解析NOTIFY消息体
    osip_body_t* body = nullptr;
    osip_message_get_body(event->request, 0, &body);
    if (!body || !body->body) {
        // 回复200 OK
        eXosip_lock(sip_context_);
        eXosip_call_send_answer(sip_context_, event->tid, 200, nullptr);
        eXosip_unlock(sip_context_);
        return;
    }
    
    // 解析报警XML
    std::string xml(body->body);
    auto alarm = parseAlarmNotifyXml(xml);
    
    if (alarm) {
        SIPSERVER_LOG(LS_INFO) << "Alarm notify received, device: " 
                               << (alarm->device_id ? alarm->device_id->c_str() : "unknown")
                               << ", priority: " << alarm->alarm_priority
                               << ", description: " 
                               << (alarm->alarm_description ? alarm->alarm_description->c_str() : "");
        
        // 调用回调函数
        if (alarm_notify_callback_) {
            alarm_notify_callback_(alarm);
        }
    }
    
    // 回复200 OK
    eXosip_lock(sip_context_);
    eXosip_call_send_answer(sip_context_, event->tid, 200, nullptr);
    eXosip_unlock(sip_context_);
}

void AlarmHandler::handleMessageResponse(eXosip_event_t* event) {
    if (!event || !event->response) {
        return;
    }
    
    osip_body_t* body = nullptr;
    osip_message_get_body(event->response, 0, &body);
    if (!body || !body->body) {
        return;
    }
    
    std::string xml(body->body);
    auto root = XmlHelper::parseXml(xml);
    if (!root) {
        SIPSERVER_LOG(LS_WARNING) << "Failed to parse XML response";
        return;
    }
    
    std::string cmdType = XmlHelper::getNodeValue(root, "Response/CmdType");
    std::string sn = XmlHelper::getNodeValue(root, "Response/SN");
    
    if (cmdType == "Alarm") {
        auto it = query_callbacks_.find(sn);
        if (it != query_callbacks_.end()) {
            try {
                auto alarms = parseAlarmQueryXml(xml);
                
                // 调用成功回调
                if (it->second.onSuccess) {
                    it->second.onSuccess(alarms);
                }
                
                // 设置Promise值
                if (it->second.promise) {
                    it->second.promise->set_value(alarms);
                }
                
                SIPSERVER_LOG(LS_INFO) << "Alarm query response processed, SN: " << sn 
                                       << ", count: " << alarms.size();
            } catch (const std::exception& e) {
                SIPSERVER_LOG(LS_WARNING) << "Failed to parse alarm query: " << e.what();
                
                // 调用错误回调
                if (it->second.onError) {
                    it->second.onError(500, e.what());
                }
                
                // 设置Promise异常
                if (it->second.promise) {
                    it->second.promise->set_exception(std::current_exception());
                }
            }
            
            // 移除回调
            query_callbacks_.erase(it);
        }
    }
}

void AlarmHandler::registerQueryCallback(
    const std::string& sn,
    std::function<void(const std::vector<oatpp::Object<AlarmItemDto>>&)> onSuccess,
    std::function<void(int, const std::string&)> onError)
{
    QueryCallback callback;
    callback.onSuccess = onSuccess;
    callback.onError = onError;
    callback.promise = std::make_shared<std::promise<std::vector<oatpp::Object<AlarmItemDto>>>>();
    
    query_callbacks_[sn] = callback;
}

std::shared_future<std::vector<oatpp::Object<AlarmItemDto>>> 
AlarmHandler::getQueryFuture(const std::string& sn) {
    auto it = query_callbacks_.find(sn);
    if (it != query_callbacks_.end() && it->second.promise) {
        return it->second.promise->get_future().share();
    }
    
    // 返回一个已经设置异常的future
    auto promise = std::make_shared<std::promise<std::vector<oatpp::Object<AlarmItemDto>>>>();
    promise->set_exception(std::make_exception_ptr(std::runtime_error("SN not found")));
    return promise->get_future().share();
}

std::string AlarmHandler::buildAlarmSubscribeXml(
    const std::string& deviceId,
    int startPriority,
    int endPriority,
    int alarmMethod)
{
    std::string sn = generateSN();
    
    std::ostringstream oss;
    oss << "<?xml version=\"1.0\" encoding=\"GB2312\"?>\r\n"
        << "<Query>\r\n"
        << "  <CmdType>Alarm</CmdType>\r\n"
        << "  <SN>" << sn << "</SN>\r\n"
        << "  <DeviceID>" << deviceId << "</DeviceID>\r\n"
        << "  <StartAlarmPriority>" << startPriority << "</StartAlarmPriority>\r\n"
        << "  <EndAlarmPriority>" << endPriority << "</EndAlarmPriority>\r\n"
        << "  <AlarmMethod>" << alarmMethod << "</AlarmMethod>\r\n"
        << "</Query>\r\n";
    
    return oss.str();
}

std::string AlarmHandler::buildAlarmQueryXml(
    const std::string& deviceId,
    const std::string& startTime,
    const std::string& endTime,
    int startPriority,
    int endPriority,
    int alarmMethod)
{
    std::string sn = generateSN();
    
    std::ostringstream oss;
    oss << "<?xml version=\"1.0\" encoding=\"GB2312\"?>\r\n"
        << "<Query>\r\n"
        << "  <CmdType>Alarm</CmdType>\r\n"
        << "  <SN>" << sn << "</SN>\r\n"
        << "  <DeviceID>" << deviceId << "</DeviceID>\r\n"
        << "  <StartTime>" << startTime << "</StartTime>\r\n"
        << "  <EndTime>" << endTime << "</EndTime>\r\n"
        << "  <StartAlarmPriority>" << startPriority << "</StartAlarmPriority>\r\n"
        << "  <EndAlarmPriority>" << endPriority << "</EndAlarmPriority>\r\n"
        << "  <AlarmMethod>" << alarmMethod << "</AlarmMethod>\r\n"
        << "</Query>\r\n";
    
    return oss.str();
}

int AlarmHandler::sendSubscribe(
    const std::string& deviceId,
    const std::string& deviceIp,
    int devicePort,
    const std::string& body,
    int expires)
{
    osip_message_t* subscribe = nullptr;
    
    char from[256], to[256];
    snprintf(from, sizeof(from), "sip:%s@%s:%d", 
             server_id_.c_str(), server_ip_.c_str(), server_port_);
    snprintf(to, sizeof(to), "sip:%s@%s:%d", 
             deviceId.c_str(), deviceIp.c_str(), devicePort);
    
    eXosip_lock(sip_context_);
    
    // 构建SUBSCRIBE请求
    int ret = eXosip_subscription_build_initial_subscribe(
        sip_context_, &subscribe, to, from, nullptr, "Alarm", expires);
    
    if (ret == 0 && subscribe) {
        // 设置消息体
        osip_message_set_body(subscribe, body.c_str(), body.length());
        osip_message_set_content_type(subscribe, "Application/MANSCDP+xml");
        
        // 发送SUBSCRIBE
        int dialogId = eXosip_subscription_send_initial_request(sip_context_, subscribe);
        eXosip_unlock(sip_context_);
        return dialogId;
    }
    
    eXosip_unlock(sip_context_);
    return -1;
}

std::string AlarmHandler::sendMessage(
    const std::string& deviceId,
    const std::string& deviceIp,
    int devicePort,
    const std::string& body)
{
    osip_message_t* message = nullptr;
    
    char from[256], to[256];
    snprintf(from, sizeof(from), "sip:%s@%s:%d", 
             server_id_.c_str(), server_ip_.c_str(), server_port_);
    snprintf(to, sizeof(to), "sip:%s@%s:%d", 
             deviceId.c_str(), deviceIp.c_str(), devicePort);
    
    eXosip_lock(sip_context_);
    int ret = eXosip_message_build_request(sip_context_, &message, "MESSAGE", 
                                           to, from, nullptr);
    if (ret == 0 && message) {
        osip_message_set_body(message, body.c_str(), body.length());
        osip_message_set_content_type(message, "Application/MANSCDP+xml");
        
        eXosip_message_send_request(sip_context_, message);
    }
    eXosip_unlock(sip_context_);
    
    // 从XML中提取SN
    size_t snStart = body.find("<SN>");
    size_t snEnd = body.find("</SN>");
    if (snStart != std::string::npos && snEnd != std::string::npos) {
        return body.substr(snStart + 4, snEnd - snStart - 4);
    }
    
    return "";
}

oatpp::Object<AlarmNotifyDto> AlarmHandler::parseAlarmNotifyXml(const std::string& xml) {
    auto root = XmlHelper::parseXml(xml);
    if (!root) {
        return nullptr;
    }
    
    auto alarm = AlarmNotifyDto::createShared();
    
    alarm->device_id = XmlHelper::getNodeValue(root, "Notify/DeviceID").c_str();
    
    std::string priority = XmlHelper::getNodeValue(root, "Notify/AlarmPriority");
    alarm->alarm_priority = priority.empty() ? 0 : std::stoi(priority);
    
    std::string method = XmlHelper::getNodeValue(root, "Notify/AlarmMethod");
    alarm->alarm_method = method.empty() ? 0 : std::stoi(method);
    
    alarm->alarm_time = XmlHelper::getNodeValue(root, "Notify/AlarmTime").c_str();
    alarm->alarm_description = XmlHelper::getNodeValue(root, "Notify/AlarmDescription").c_str();
    
    std::string lon = XmlHelper::getNodeValue(root, "Notify/Longitude");
    alarm->longitude = lon.empty() ? 0.0 : std::stod(lon);
    
    std::string lat = XmlHelper::getNodeValue(root, "Notify/Latitude");
    alarm->latitude = lat.empty() ? 0.0 : std::stod(lat);
    
    return alarm;
}

std::vector<oatpp::Object<AlarmItemDto>> 
AlarmHandler::parseAlarmQueryXml(const std::string& xml) {
    std::vector<oatpp::Object<AlarmItemDto>> alarms;
    
    auto root = XmlHelper::parseXml(xml);
    if (!root) {
        return alarms;
    }
    
    // 解析AlarmList
    auto alarmList = XmlHelper::getChildNode(root, "Response/AlarmList");
    if (!alarmList) {
        return alarms;
    }
    
    // 遍历Item节点
    for (auto item = alarmList->first_node("Item"); item; item = item->next_sibling("Item")) {
        auto alarm = AlarmItemDto::createShared();
        
        alarm->device_id = XmlHelper::getNodeValue(item, "DeviceID").c_str();
        alarm->channel_id = alarm->device_id;
        
        std::string priority = XmlHelper::getNodeValue(item, "AlarmPriority");
        alarm->alarm_priority = priority.empty() ? 0 : std::stoi(priority);
        
        std::string method = XmlHelper::getNodeValue(item, "AlarmMethod");
        alarm->alarm_method = method.empty() ? 0 : std::stoi(method);
        
        alarm->alarm_time = XmlHelper::getNodeValue(item, "AlarmTime").c_str();
        alarm->alarm_description = XmlHelper::getNodeValue(item, "AlarmDescription").c_str();
        
        std::string lon = XmlHelper::getNodeValue(item, "Longitude");
        alarm->longitude = lon.empty() ? 0.0 : std::stod(lon);
        
        std::string lat = XmlHelper::getNodeValue(item, "Latitude");
        alarm->latitude = lat.empty() ? 0.0 : std::stod(lat);
        
        alarm->status = 0; // 默认未处理
        
        alarms.push_back(alarm);
    }
    
    return alarms;
}

std::string AlarmHandler::generateSN() {
    return std::to_string(sn_counter_++);
}

} // namespace gbsip_server
