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

#include "DeviceInfoHandler.hpp"
#include "utils/XmlHelper.hpp"
#include "gbsip_server_log.h"

namespace gbsip_server {

DeviceInfoHandler::DeviceInfoHandler(eXosip_t* context,
                                     const std::string& serverId,
                                     const std::string& serverIp,
                                     int serverPort)
    : SipMessageHandler(context, serverId, serverIp, serverPort)
{
}

DeviceInfoHandler::~DeviceInfoHandler() {
}

std::string DeviceInfoHandler::queryDeviceInfo(
    const std::string& deviceId,
    const std::string& deviceIp,
    int devicePort,
    std::function<void(const DeviceInfo&)> onSuccess,
    std::function<void(int, const std::string&)> onError) {
    
    // 生成SN
    std::string sn = generateSN();
    
    // 构建设备信息查询XML
    std::string xml = GB28181XmlBuilder::buildDeviceInfoQuery(deviceId, std::stoi(sn));
    
    // 创建回调
    ResponseCallback callback;
    callback.onSuccess = [this, onSuccess](const std::string& xml) {
        try {
            DeviceInfo info = parseDeviceInfoXml(xml);
            if (onSuccess) {
                onSuccess(info);
            }
        } catch (const std::exception& e) {
            SIPSERVER_LOG(LS_ERROR) << "Failed to parse device info response: " << e.what();
        }
    };
    callback.onError = onError;
    
    // 发送MESSAGE
    return sendMessage(deviceId, deviceIp, devicePort, xml, callback);
}

std::string DeviceInfoHandler::queryDeviceStatus(
    const std::string& deviceId,
    const std::string& deviceIp,
    int devicePort,
    std::function<void(const DeviceStatus&)> onSuccess,
    std::function<void(int, const std::string&)> onError) {
    
    // 生成SN
    std::string sn = generateSN();
    
    // 构建设备状态查询XML
    std::string xml = GB28181XmlBuilder::buildDeviceStatusQuery(deviceId, std::stoi(sn));
    
    // 创建回调
    ResponseCallback callback;
    callback.onSuccess = [this, onSuccess](const std::string& xml) {
        try {
            DeviceStatus status = parseDeviceStatusXml(xml);
            if (onSuccess) {
                onSuccess(status);
            }
        } catch (const std::exception& e) {
            SIPSERVER_LOG(LS_ERROR) << "Failed to parse device status response: " << e.what();
        }
    };
    callback.onError = onError;
    
    // 发送MESSAGE
    return sendMessage(deviceId, deviceIp, devicePort, xml, callback);
}

void DeviceInfoHandler::handleNotify(eXosip_event_t* event) {
    // 设备信息查询不需要处理NOTIFY
    SIPSERVER_LOG(LS_WARNING) << "DeviceInfoHandler received unexpected NOTIFY";
}

DeviceInfo DeviceInfoHandler::parseDeviceInfoXml(const std::string& xml) {
    auto root = XmlHelper::parseXml(xml);
    if (!root) {
        throw std::runtime_error("Failed to parse XML");
    }
    
    DeviceInfo info;
    
    // 解析基本信息
    info.sn = XmlHelper::getNodeValue(root, "Response/SN");
    info.deviceId = XmlHelper::getNodeValue(root, "Response/DeviceID");
    info.deviceName = XmlHelper::getNodeValue(root, "Response/DeviceName");
    info.manufacturer = XmlHelper::getNodeValue(root, "Response/Manufacturer");
    info.model = XmlHelper::getNodeValue(root, "Response/Model");
    info.firmware = XmlHelper::getNodeValue(root, "Response/Firmware");
    
    // 解析通道数
    std::string channelStr = XmlHelper::getNodeValue(root, "Response/Channel");
    info.channel = channelStr.empty() ? 0 : std::stoi(channelStr);
    
    SIPSERVER_LOG(LS_INFO) << "Parsed device info: " << info.deviceId
                           << ", name: " << info.deviceName
                           << ", manufacturer: " << info.manufacturer;
    
    return info;
}

DeviceStatus DeviceInfoHandler::parseDeviceStatusXml(const std::string& xml) {
    auto root = XmlHelper::parseXml(xml);
    if (!root) {
        throw std::runtime_error("Failed to parse XML");
    }
    
    DeviceStatus status;
    
    // 解析基本信息
    status.sn = XmlHelper::getNodeValue(root, "Response/SN");
    status.deviceId = XmlHelper::getNodeValue(root, "Response/DeviceID");
    status.online = XmlHelper::getNodeValue(root, "Response/Online");
    status.status = XmlHelper::getNodeValue(root, "Response/Status");
    status.encode = XmlHelper::getNodeValue(root, "Response/Encode");
    status.record = XmlHelper::getNodeValue(root, "Response/Record");
    status.deviceTime = XmlHelper::getNodeValue(root, "Response/DeviceTime");
    
    SIPSERVER_LOG(LS_INFO) << "Parsed device status: " << status.deviceId
                           << ", online: " << status.online
                           << ", status: " << status.status;
    
    return status;
}

} // namespace gbsip_server
