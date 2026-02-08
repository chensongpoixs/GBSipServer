/*
*  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source tree.
*/
/*****************************************************************************
				  Author: chensong
				  date:  2026-02-09
				  
				  PTZ控制SIP处理器
******************************************************************************/

#ifndef _C_PTZ_HANDLER_HPP
#define _C_PTZ_HANDLER_HPP

extern "C" {
#include <eXosip2/eXosip.h>
}
#include <string>
#include <functional>

namespace gbsip_server {

/**
 * PTZ控制SIP处理器
 * 负责发送PTZ控制INFO消息
 */
class PTZHandler {
public:
    /**
     * 构造函数
     * @param context eXosip上下文
     * @param serverId 服务器ID
     * @param serverIp 服务器IP
     * @param serverPort 服务器端口
     */
    PTZHandler(eXosip_t* context,
               const std::string& serverId,
               const std::string& serverIp,
               int serverPort);
    
    /**
     * 析构函数
     */
    virtual ~PTZHandler();
    
    /**
     * 发送PTZ控制命令
     * @param deviceId 设备ID
     * @param channelId 通道ID
     * @param ptzCmd PTZ命令（十六进制字符串）
     * @param deviceIp 设备IP
     * @param devicePort 设备端口
     * @param onSuccess 成功回调
     * @param onError 错误回调
     * @return true=发送成功，false=发送失败
     */
    bool sendPTZControl(const std::string& deviceId,
                        const std::string& channelId,
                        const std::string& ptzCmd,
                        const std::string& deviceIp,
                        int devicePort,
                        std::function<void()> onSuccess = nullptr,
                        std::function<void(int, const std::string&)> onError = nullptr);

private:
    /**
     * 构建PTZ控制XML消息
     * @param deviceId 设备ID
     * @param channelId 通道ID
     * @param ptzCmd PTZ命令
     * @return XML字符串
     */
    std::string buildPTZXml(const std::string& deviceId,
                            const std::string& channelId,
                            const std::string& ptzCmd);
    
    /**
     * 发送SIP INFO消息
     * @param deviceId 设备ID
     * @param deviceIp 设备IP
     * @param devicePort 设备端口
     * @param xml XML消息体
     * @return true=发送成功，false=发送失败
     */
    bool sendInfoMessage(const std::string& deviceId,
                         const std::string& deviceIp,
                         int devicePort,
                         const std::string& xml);
    
    /**
     * 生成序列号
     * @return 序列号
     */
    int generateSN();

private:
    eXosip_t* context_;
    std::string server_id_;
    std::string server_ip_;
    int server_port_;
    int sn_counter_;
};

} // namespace gbsip_server

#endif /* _C_PTZ_HANDLER_HPP */
