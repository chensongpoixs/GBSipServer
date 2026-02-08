/*
*  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source tree.
*/
/*****************************************************************************
				  Author: chensong
				  date:  2026-02-09
				  
				  PTZ控制SIP处理器实现
******************************************************************************/

#include "PTZHandler.hpp"
#include "gbsip_server_log.h"
#include <sstream>
#include <ctime>

extern "C" {
#include <osip2/osip.h>
}

namespace gbsip_server {

PTZHandler::PTZHandler(eXosip_t* context,
                       const std::string& serverId,
                       const std::string& serverIp,
                       int serverPort)
    : context_(context)
    , server_id_(serverId)
    , server_ip_(serverIp)
    , server_port_(serverPort)
    , sn_counter_(1)
{
    SIPSERVER_LOG(LS_INFO) << "PTZHandler initialized";
}

PTZHandler::~PTZHandler() {
    SIPSERVER_LOG(LS_INFO) << "PTZHandler destroyed";
}

bool PTZHandler::sendPTZControl(const std::string& deviceId,
                                 const std::string& channelId,
                                 const std::string& ptzCmd,
                                 const std::string& deviceIp,
                                 int devicePort,
                                 std::function<void()> onSuccess,
                                 std::function<void(int, const std::string&)> onError)
{
    if (!context_) {
        SIPSERVER_LOG(LS_WARNING) << "eXosip context is null";
        if (onError) {
            onError(-1, "eXosip context is null");
        }
        return false;
    }
    
    // 构建PTZ控制XML
    std::string xml = buildPTZXml(deviceId, channelId, ptzCmd);
    
    SIPSERVER_LOG(LS_INFO) << "Sending PTZ control to device: " << deviceId 
                           << ", channel: " << channelId 
                           << ", cmd: " << ptzCmd;
    
    // 发送INFO消息
    bool success = sendInfoMessage(deviceId, deviceIp, devicePort, xml);
    
    if (success) {
        SIPSERVER_LOG(LS_INFO) << "PTZ control sent successfully";
        if (onSuccess) {
            onSuccess();
        }
    } else {
        SIPSERVER_LOG(LS_WARNING) << "Failed to send PTZ control";
        if (onError) {
            onError(-2, "Failed to send SIP INFO message");
        }
    }
    
    return success;
}

std::string PTZHandler::buildPTZXml(const std::string& deviceId,
                                     const std::string& channelId,
                                     const std::string& ptzCmd)
{
    int sn = generateSN();
    
    std::ostringstream oss;
    oss << "<?xml version=\"1.0\" encoding=\"GB2312\"?>\r\n";
    oss << "<Control>\r\n";
    oss << "    <CmdType>DeviceControl</CmdType>\r\n";
    oss << "    <SN>" << sn << "</SN>\r\n";
    oss << "    <DeviceID>" << channelId << "</DeviceID>\r\n";
    oss << "    <PTZCmd>" << ptzCmd << "</PTZCmd>\r\n";
    oss << "</Control>\r\n";
    
    return oss.str();
}

bool PTZHandler::sendInfoMessage(const std::string& deviceId,
                                  const std::string& deviceIp,
                                  int devicePort,
                                  const std::string& xml)
{
    if (!context_) {
        return false;
    }
    
    osip_message_t* info = nullptr;
    char from[256] = {0};
    char to[256] = {0};
    
    // 构建From和To URI
    snprintf(from, sizeof(from), "sip:%s@%s:%d", 
             server_id_.c_str(), server_ip_.c_str(), server_port_);
    snprintf(to, sizeof(to), "sip:%s@%s:%d", 
             deviceId.c_str(), deviceIp.c_str(), devicePort);
    
    eXosip_lock(context_);
    
    // 构建INFO消息
    int ret = eXosip_message_build_request(context_, &info, "INFO", to, from, nullptr);
    if (ret != 0 || !info) {
        SIPSERVER_LOG(LS_WARNING) << "Failed to build INFO message, ret: " << ret;
        eXosip_unlock(context_);
        return false;
    }
    
    // 设置消息体
    osip_message_set_body(info, xml.c_str(), xml.length());
    osip_message_set_content_type(info, "Application/MANSCDP+xml");
    
    // 发送INFO消息
    ret = eXosip_message_send_request(context_, info);
    
    eXosip_unlock(context_);
    
    if (ret < 0) {
        SIPSERVER_LOG(LS_WARNING) << "Failed to send INFO message, ret: " << ret;
        return false;
    }
    
    SIPSERVER_LOG(LS_INFO) << "INFO message sent, transaction id: " << ret;
    return true;
}

int PTZHandler::generateSN() {
    return sn_counter_++;
}

} // namespace gbsip_server
