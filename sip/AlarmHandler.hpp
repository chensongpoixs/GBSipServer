/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Author: chensong
 *  Date: 2026-02-09
 *  Description: 报警SIP处理器
 ******************************************************************************/

#ifndef GBSIPSERVER_SIP_ALARMHANDLER_HPP
#define GBSIPSERVER_SIP_ALARMHANDLER_HPP

#include <string>
#include <functional>
#include <memory>
#include <unordered_map>
#include <future>
extern "C" {
#include <eXosip2/eXosip.h>
}
#include "dto/AlarmDto.hpp"
#include "utils/XmlHelper.hpp"

namespace gbsip_server {

/**
 * 报警SIP处理器
 * 负责处理GB28181报警相关的SIP消息
 */
class AlarmHandler {
public:
    /**
     * 报警通知回调函数类型
     */
    using AlarmNotifyCallback = std::function<void(const oatpp::Object<AlarmNotifyDto>&)>;

    /**
     * 构造函数
     */
    AlarmHandler(eXosip_t* context, 
                 const std::string& serverId,
                 const std::string& serverIp,
                 int serverPort);

    /**
     * 析构函数
     */
    ~AlarmHandler();

    /**
     * 订阅报警
     * @param deviceId 设备ID
     * @param startPriority 起始报警级别
     * @param endPriority 结束报警级别
     * @param alarmMethod 报警方式
     * @param expires 订阅有效期(秒)
     * @param deviceIp 设备IP
     * @param devicePort 设备端口
     * @return 对话ID
     */
    int subscribeAlarm(
        const std::string& deviceId,
        int startPriority,
        int endPriority,
        int alarmMethod,
        int expires,
        const std::string& deviceIp,
        int devicePort);

    /**
     * 取消订阅报警
     * @param dialogId 对话ID
     * @return 是否成功
     */
    bool unsubscribeAlarm(int dialogId);

    /**
     * 查询报警信息
     * @param deviceId 设备ID
     * @param startTime 开始时间
     * @param endTime 结束时间
     * @param startPriority 起始报警级别
     * @param endPriority 结束报警级别
     * @param alarmMethod 报警方式
     * @param deviceIp 设备IP
     * @param devicePort 设备端口
     * @return 序列号SN
     */
    std::string queryAlarm(
        const std::string& deviceId,
        const std::string& startTime,
        const std::string& endTime,
        int startPriority,
        int endPriority,
        int alarmMethod,
        const std::string& deviceIp,
        int devicePort);

    /**
     * 处理报警通知NOTIFY
     */
    void handleAlarmNotify(eXosip_event_t* event);

    /**
     * 处理MESSAGE响应
     */
    void handleMessageResponse(eXosip_event_t* event);

    /**
     * 设置报警通知回调
     */
    void setAlarmNotifyCallback(AlarmNotifyCallback callback) {
        alarm_notify_callback_ = callback;
    }

    /**
     * 注册查询回调
     */
    void registerQueryCallback(
        const std::string& sn,
        std::function<void(const std::vector<oatpp::Object<AlarmItemDto>>&)> onSuccess,
        std::function<void(int, const std::string&)> onError);

    /**
     * 获取查询结果的Future
     */
    std::shared_future<std::vector<oatpp::Object<AlarmItemDto>>> getQueryFuture(const std::string& sn);

private:
    /**
     * 构建报警订阅XML
     */
    std::string buildAlarmSubscribeXml(
        const std::string& deviceId,
        int startPriority,
        int endPriority,
        int alarmMethod);

    /**
     * 构建报警查询XML
     */
    std::string buildAlarmQueryXml(
        const std::string& deviceId,
        const std::string& startTime,
        const std::string& endTime,
        int startPriority,
        int endPriority,
        int alarmMethod);

    /**
     * 发送SUBSCRIBE请求
     */
    int sendSubscribe(
        const std::string& deviceId,
        const std::string& deviceIp,
        int devicePort,
        const std::string& body,
        int expires);

    /**
     * 发送MESSAGE请求
     */
    std::string sendMessage(
        const std::string& deviceId,
        const std::string& deviceIp,
        int devicePort,
        const std::string& body);

    /**
     * 解析报警通知XML
     */
    oatpp::Object<AlarmNotifyDto> parseAlarmNotifyXml(const std::string& xml);

    /**
     * 解析报警查询响应XML
     */
    std::vector<oatpp::Object<AlarmItemDto>> parseAlarmQueryXml(const std::string& xml);

    /**
     * 生成序列号
     */
    std::string generateSN();

private:
    eXosip_t* sip_context_;                    // eXosip上下文
    std::string server_id_;                    // 服务器ID
    std::string server_ip_;                    // 服务器IP
    int server_port_;                          // 服务器端口
    int sn_counter_;                           // 序列号计数器

    // 报警通知回调
    AlarmNotifyCallback alarm_notify_callback_;

    // 订阅映射表 (deviceId -> dialogId)
    std::unordered_map<std::string, int> subscriptions_;

    // 查询回调映射表
    struct QueryCallback {
        std::function<void(const std::vector<oatpp::Object<AlarmItemDto>>&)> onSuccess;
        std::function<void(int, const std::string&)> onError;
        std::shared_ptr<std::promise<std::vector<oatpp::Object<AlarmItemDto>>>> promise;
    };
    std::unordered_map<std::string, QueryCallback> query_callbacks_;
};

} // namespace gbsip_server

#endif // GBSIPSERVER_SIP_ALARMHANDLER_HPP
