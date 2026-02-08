/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Author: chensong
 *  Date: 2026-02-09
 *  Description: 录像SIP处理器
 ******************************************************************************/

#ifndef GBSIPSERVER_SIP_RECORDHANDLER_HPP
#define GBSIPSERVER_SIP_RECORDHANDLER_HPP

#include <string>
#include <functional>
#include <memory>
#include <unordered_map>
#include <future>
extern "C" {
#include <eXosip2/eXosip.h>
}
#include "dto/RecordDto.hpp"
#include "utils/XmlHelper.hpp"

namespace gbsip_server {

/**
 * 录像SIP处理器
 * 负责处理GB28181录像相关的SIP消息
 */
class RecordHandler {
public:
    /**
     * 构造函数
     */
    RecordHandler(eXosip_t* context, 
                  const std::string& serverId,
                  const std::string& serverIp,
                  int serverPort);

    /**
     * 析构函数
     */
    ~RecordHandler();

    /**
     * 查询录像信息
     * @param deviceId 设备ID
     * @param channelId 通道ID
     * @param startTime 开始时间
     * @param endTime 结束时间
     * @param type 录像类型
     * @param deviceIp 设备IP
     * @param devicePort 设备端口
     * @return 序列号SN
     */
    std::string queryRecordInfo(
        const std::string& deviceId,
        const std::string& channelId,
        const std::string& startTime,
        const std::string& endTime,
        const std::string& type,
        const std::string& deviceIp,
        int devicePort);

    /**
     * 开始回放
     * @param deviceId 设备ID
     * @param channelId 通道ID
     * @param startTime 开始时间
     * @param endTime 结束时间
     * @param localIp 本地IP
     * @param localPort 本地端口
     * @param deviceIp 设备IP
     * @param devicePort 设备端口
     * @return 会话ID
     */
    int startPlayback(
        const std::string& deviceId,
        const std::string& channelId,
        const std::string& startTime,
        const std::string& endTime,
        const std::string& localIp,
        int localPort,
        const std::string& deviceIp,
        int devicePort);

    /**
     * 暂停回放
     */
    bool pausePlayback(
        const std::string& deviceId,
        const std::string& channelId,
        int callId,
        const std::string& deviceIp,
        int devicePort);

    /**
     * 继续回放
     */
    bool resumePlayback(
        const std::string& deviceId,
        const std::string& channelId,
        int callId,
        const std::string& deviceIp,
        int devicePort);

    /**
     * 停止回放
     */
    bool stopPlayback(int callId);

    /**
     * 跳转回放
     */
    bool seekPlayback(
        const std::string& deviceId,
        const std::string& channelId,
        int callId,
        const std::string& seekTime,
        const std::string& deviceIp,
        int devicePort);

    /**
     * 设置回放速度
     */
    bool setPlaybackSpeed(
        const std::string& deviceId,
        const std::string& channelId,
        int callId,
        float speed,
        const std::string& deviceIp,
        int devicePort);

    /**
     * 开始下载
     */
    int startDownload(
        const std::string& deviceId,
        const std::string& channelId,
        const std::string& startTime,
        const std::string& endTime,
        const std::string& localIp,
        int localPort,
        const std::string& deviceIp,
        int devicePort);

    /**
     * 停止下载
     */
    bool stopDownload(int callId);

    /**
     * 处理MESSAGE响应
     */
    void handleMessageResponse(eXosip_event_t* event);

    /**
     * 注册录像查询回调
     */
    void registerQueryCallback(
        const std::string& sn,
        std::function<void(const std::vector<oatpp::Object<RecordItemDto>>&)> onSuccess,
        std::function<void(int, const std::string&)> onError);

    /**
     * 获取查询结果的Future
     */
    std::shared_future<std::vector<oatpp::Object<RecordItemDto>>> getQueryFuture(const std::string& sn);

private:
    /**
     * 构建录像查询XML
     */
    std::string buildRecordInfoXml(
        const std::string& deviceId,
        const std::string& channelId,
        const std::string& startTime,
        const std::string& endTime,
        const std::string& type);

    /**
     * 构建回放SDP
     */
    std::string buildPlaybackSdp(
        const std::string& localIp,
        int localPort,
        const std::string& startTime,
        const std::string& endTime);

    /**
     * 构建回放控制XML
     */
    std::string buildPlaybackControlXml(
        const std::string& deviceId,
        const std::string& channelId,
        const std::string& command,
        const std::string& seekTime = "",
        float speed = 1.0f);

    /**
     * 发送MESSAGE请求
     */
    std::string sendMessage(
        const std::string& deviceId,
        const std::string& deviceIp,
        int devicePort,
        const std::string& body);

    /**
     * 发送INVITE请求
     */
    int sendInvite(
        const std::string& deviceId,
        const std::string& channelId,
        const std::string& deviceIp,
        int devicePort,
        const std::string& sdp);

    /**
     * 发送INFO消息
     */
    bool sendInfo(
        int callId,
        const std::string& body);

    /**
     * 发送BYE消息
     */
    bool sendBye(int callId);

    /**
     * 解析录像信息XML
     */
    std::vector<oatpp::Object<RecordItemDto>> parseRecordInfoXml(const std::string& xml);

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

    // 回调映射表
    struct QueryCallback {
        std::function<void(const std::vector<oatpp::Object<RecordItemDto>>&)> onSuccess;
        std::function<void(int, const std::string&)> onError;
        std::shared_ptr<std::promise<std::vector<oatpp::Object<RecordItemDto>>>> promise;
    };
    std::unordered_map<std::string, QueryCallback> query_callbacks_;
};

} // namespace gbsip_server

#endif // GBSIPSERVER_SIP_RECORDHANDLER_HPP
