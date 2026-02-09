/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Author: chensong
 *  Date: 2026-02-09
 *  Description: 语音对讲SIP处理器实现
 ******************************************************************************/

#include "VoiceHandler.hpp"
#include "gbsip_server_log.h"
#include <sstream>

namespace gbsip_server {

VoiceHandler::VoiceHandler(eXosip_t* context, 
                           const std::string& serverId,
                           const std::string& serverIp,
                           int serverPort)
    : sip_context_(context)
    , server_id_(serverId)
    , server_ip_(serverIp)
    , server_port_(serverPort)
{
    SIPSERVER_LOG(LS_INFO) << "VoiceHandler initialized";
}

VoiceHandler::~VoiceHandler() {
    SIPSERVER_LOG(LS_INFO) << "VoiceHandler destroyed";
}

int VoiceHandler::startBroadcast(
    const std::string& deviceId,
    const std::string& localIp,
    int localPort,
    const std::string& deviceIp,
    int devicePort)
{
    // 1. 构建音频SDP（sendonly）
    std::string sdp = buildAudioSDP(localIp, localPort, "sendonly");
    
    // 2. 发送INVITE请求
    int callId = sendInvite(deviceId, deviceIp, devicePort, sdp);
    
    if (callId > 0) {
        SIPSERVER_LOG(LS_INFO) << "Broadcast started, device: " << deviceId 
                               << ", call_id: " << callId;
    } else {
        SIPSERVER_LOG(LS_WARNING) << "Failed to start broadcast, device: " << deviceId;
    }
    
    return callId;
}

bool VoiceHandler::stopBroadcast(int callId) {
    return sendBye(callId);
}

int VoiceHandler::startTalk(
    const std::string& deviceId,
    const std::string& localIp,
    int localPort,
    const std::string& deviceIp,
    int devicePort)
{
    // 1. 构建音频SDP（sendrecv）
    std::string sdp = buildAudioSDP(localIp, localPort, "sendrecv");
    
    // 2. 发送INVITE请求
    int callId = sendInvite(deviceId, deviceIp, devicePort, sdp);
    
    if (callId > 0) {
        SIPSERVER_LOG(LS_INFO) << "Talk started, device: " << deviceId 
                               << ", call_id: " << callId;
    } else {
        SIPSERVER_LOG(LS_WARNING) << "Failed to start talk, device: " << deviceId;
    }
    
    return callId;
}

bool VoiceHandler::stopTalk(int callId) {
    return sendBye(callId);
}

std::string VoiceHandler::buildAudioSDP(
    const std::string& localIp,
    int localPort,
    const std::string& mode)
{
    std::ostringstream sdp;
    sdp << "v=0\r\n";
    sdp << "o=" << server_id_ << " 0 0 IN IP4 " << localIp << "\r\n";
    sdp << "s=" << (mode == "sendonly" ? "Broadcast" : "Talk") << "\r\n";
    sdp << "c=IN IP4 " << localIp << "\r\n";
    sdp << "t=0 0\r\n";
    sdp << "m=audio " << localPort << " RTP/AVP 8\r\n";
    sdp << "a=rtpmap:8 PCMA/8000\r\n";
    sdp << "a=" << mode << "\r\n";
    
    return sdp.str();
}

int VoiceHandler::sendInvite(
    const std::string& deviceId,
    const std::string& deviceIp,
    int devicePort,
    const std::string& sdp)
{
    if (!sip_context_) {
        SIPSERVER_LOG(LS_WARNING) << "SIP context is null";
        return -1;
    }

    // 构建SIP URI
    std::ostringstream from_uri, to_uri, contact_uri;
    from_uri << "sip:" << server_id_ << "@" << server_ip_ << ":" << server_port_;
    to_uri << "sip:" << deviceId << "@" << deviceIp << ":" << devicePort;
    contact_uri << "sip:" << server_id_ << "@" << server_ip_ << ":" << server_port_;

    osip_message_t* invite = nullptr;
    int ret = eXosip_call_build_initial_invite(
        sip_context_,
        &invite,
        to_uri.str().c_str(),
        from_uri.str().c_str(),
        nullptr,
        "Voice Call"
    );

    if (ret != 0 || !invite) {
        SIPSERVER_LOG(LS_WARNING) << "Failed to build INVITE message";
        return -1;
    }

    // 设置Contact头
    osip_message_set_contact(invite, contact_uri.str().c_str());

    // 设置SDP
    osip_message_set_body(invite, sdp.c_str(), sdp.length());
    osip_message_set_content_type(invite, "application/sdp");

    // 发送INVITE
    eXosip_lock(sip_context_);
    int call_id = eXosip_call_send_initial_invite(sip_context_, invite);
    eXosip_unlock(sip_context_);

    if (call_id < 0) {
        SIPSERVER_LOG(LS_WARNING) << "Failed to send INVITE";
        return -1;
    }

    return call_id;
}

bool VoiceHandler::sendBye(int callId) {
    if (!sip_context_ || callId <= 0) {
        return false;
    }

    eXosip_lock(sip_context_);
    int ret = eXosip_call_terminate(sip_context_, callId, -1);
    eXosip_unlock(sip_context_);

    if (ret == 0) {
        SIPSERVER_LOG(LS_INFO) << "BYE sent, call_id: " << callId;
        return true;
    } else {
        SIPSERVER_LOG(LS_WARNING) << "Failed to send BYE, call_id: " << callId;
        return false;
    }
}

} // namespace gbsip_server
