/*
*  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source tree.
*/
/*****************************************************************************
				  Author: chensong
				  date:  2026-02-09
				  
				  设备信息查询SIP处理器
******************************************************************************/

#ifndef _C_DEVICE_INFO_HANDLER_HPP
#define _C_DEVICE_INFO_HANDLER_HPP

#include "SipMessageHandler.hpp"
#include <string>
#include <functional>

namespace gbsip_server {

/**
 * 设备信息结构
 */
struct DeviceInfo {
    std::string deviceId;
    std::string deviceName;
    std::string manufacturer;
    std::string model;
    std::string firmware;
    int channel;
    std::string sn;
};

/**
 * 设备状态结构
 */
struct DeviceStatus {
    std::string deviceId;
    std::string online;      // ONLINE/OFFLINE
    std::string status;      // OK/ERROR
    std::string encode;      // ON/OFF
    std::string record;      // ON/OFF
    std::string deviceTime;
    std::string sn;
};

/**
 * 设备信息查询SIP处理器
 */
class DeviceInfoHandler : public SipMessageHandler {
public:
    /**
     * 构造函数
     */
    DeviceInfoHandler(eXosip_t* context,
                      const std::string& serverId,
                      const std::string& serverIp,
                      int serverPort);
    
    /**
     * 析构函数
     */
    virtual ~DeviceInfoHandler();
    
    /**
     * 查询设备信息
     * @param deviceId 设备ID
     * @param deviceIp 设备IP
     * @param devicePort 设备端口
     * @param onSuccess 成功回调
     * @param onError 错误回调
     * @return 序列号
     */
    std::string queryDeviceInfo(const std::string& deviceId,
                                 const std::string& deviceIp,
                                 int devicePort,
                                 std::function<void(const DeviceInfo&)> onSuccess,
                                 std::function<void(int, const std::string&)> onError);
    
    /**
     * 查询设备状态
     * @param deviceId 设备ID
     * @param deviceIp 设备IP
     * @param devicePort 设备端口
     * @param onSuccess 成功回调
     * @param onError 错误回调
     * @return 序列号
     */
    std::string queryDeviceStatus(const std::string& deviceId,
                                   const std::string& deviceIp,
                                   int devicePort,
                                   std::function<void(const DeviceStatus&)> onSuccess,
                                   std::function<void(int, const std::string&)> onError);
    
    /**
     * 处理NOTIFY消息（设备信息不需要NOTIFY）
     */
    virtual void handleNotify(eXosip_event_t* event) override;

private:
    /**
     * 解析设备信息XML
     * @param xml XML字符串
     * @return 设备信息
     */
    DeviceInfo parseDeviceInfoXml(const std::string& xml);
    
    /**
     * 解析设备状态XML
     * @param xml XML字符串
     * @return 设备状态
     */
    DeviceStatus parseDeviceStatusXml(const std::string& xml);
};

} // namespace gbsip_server

#endif /* _C_DEVICE_INFO_HANDLER_HPP */
