/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Author: chensong
 *  Date: 2026-02-09
 *  Description: 语音对讲SIP处理器
 ******************************************************************************/

#ifndef GBSIPSERVER_SIP_VOICEHANDLER_HPP
#define GBSIPSERVER_SIP_VOICEHANDLER_HPP

#include <string>
#include <memory>
extern "C" {
#include <eXosip2/eXosip.h>
}

namespace gbsip_server {

/**
 * 语音对讲SIP处理器
 * 负责处理GB28181语音对讲相关的SIP消息
 */
class VoiceHandler {
public:
    /**
     * 构造函数
     */
    VoiceHandler(eXosip_t* context, 
                 const std::string& serverId,
                 const std::string& serverIp,
                 int serverPort);

    /**
     * 析构函数
     */
    ~VoiceHandler();

    /**
     * 开始语音广播
     * @param deviceId 设备ID
     * @param localIp 本地IP
     * @param localPort 本地端口
     * @param deviceIp 设备IP
     * @param devicePort 设备端口
     * @return 会话ID
     */
    int startBroadcast(
        const std::string& deviceId,
        const std::string& localIp,
        int localPort,
        const std::string& deviceIp,
        int devicePort);

    /**
     * 停止语音广播
     * @param callId 会话ID
     * @return 是否成功
     */
    bool stopBroadcast(int callId);

    /**
     * 开始双向对讲
     * @param deviceId 设备ID
     * @param localIp 本地IP
     * @param localPort 本地端口
     * @param deviceIp 设备IP
     * @param devicePort 设备端口
     * @return 会话ID
     */
    int startTalk(
        const std::string& deviceId,
        const std::string& localIp,
        int localPort,
        const std::string& deviceIp,
        int devicePort);

    /**
     * 停止双向对讲
     * @param callId 会话ID
     * @return 是否成功
     */
    bool stopTalk(int callId);

private:
    /**
     * 构建音频SDP
     * @param localIp 本地IP
     * @param localPort 本地端口
     * @param mode 模式（sendonly/sendrecv）
     * @return SDP字符串
     */
    std::string buildAudioSDP(
        const std::string& localIp,
        int localPort,
        const std::string& mode);

    /**
     * 发送INVITE请求
     * @param deviceId 设备ID
     * @param deviceIp 设备IP
     * @param devicePort 设备端口
     * @param sdp SDP内容
     * @return 会话ID
     */
    int sendInvite(
        const std::string& deviceId,
        const std::string& deviceIp,
        int devicePort,
        const std::string& sdp);

    /**
     * 发送BYE请求
     * @param callId 会话ID
     * @return 是否成功
     */
    bool sendBye(int callId);

private:
    eXosip_t* sip_context_;                    // eXosip上下文
    std::string server_id_;                    // 服务器ID
    std::string server_ip_;                    // 服务器IP
    int server_port_;                          // 服务器端口
};

} // namespace gbsip_server

#endif // GBSIPSERVER_SIP_VOICEHANDLER_HPP
