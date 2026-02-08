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

#include "CatalogHandler.hpp"
#include "utils/XmlHelper.hpp"
#include "gbsip_server_log.h"

namespace gbsip_server {

CatalogHandler::CatalogHandler(eXosip_t* context,
                               const std::string& serverId,
                               const std::string& serverIp,
                               int serverPort)
    : SipMessageHandler(context, serverId, serverIp, serverPort)
{
}

CatalogHandler::~CatalogHandler() {
}

std::string CatalogHandler::queryCatalog(
    const std::string& deviceId,
    const std::string& deviceIp,
    int devicePort,
    std::function<void(const CatalogResponse&)> onSuccess,
    std::function<void(int, const std::string&)> onError) {
    
    // 生成SN
    std::string sn = generateSN();
    
    // 构建目录查询XML
    std::string xml = GB28181XmlBuilder::buildCatalogQuery(deviceId, std::stoi(sn));
    
    // 创建回调
    ResponseCallback callback;
    callback.onSuccess = [this, onSuccess](const std::string& xml) {
        try {
            CatalogResponse response = parseCatalogXml(xml);
            if (onSuccess) {
                onSuccess(response);
            }
        } catch (const std::exception& e) {
            SIPSERVER_LOG(LS_ERROR) << "Failed to parse catalog response: " << e.what();
        }
    };
    callback.onError = onError;
    
    // 发送MESSAGE
    return sendMessage(deviceId, deviceIp, devicePort, xml, callback);
}

int CatalogHandler::subscribeCatalog(
    const std::string& deviceId,
    const std::string& deviceIp,
    int devicePort,
    int expires,
    std::function<void()> onSuccess,
    std::function<void(int, const std::string&)> onError) {
    
    // 创建回调
    ResponseCallback callback;
    callback.onSuccess = [onSuccess](const std::string& xml) {
        if (onSuccess) {
            onSuccess();
        }
    };
    callback.onError = onError;
    
    // 发送SUBSCRIBE
    return sendSubscribe(deviceId, deviceIp, devicePort, "Catalog", expires, callback);
}

void CatalogHandler::handleNotify(eXosip_event_t* event) {
    if (!event || !event->request) {
        return;
    }
    
    // 获取消息体
    osip_body_t* body = nullptr;
    osip_message_get_body(event->request, 0, &body);
    if (!body || !body->body) {
        SIPSERVER_LOG(LS_WARNING) << "NOTIFY has no body";
        return;
    }
    
    std::string xml(body->body);
    
    try {
        // 解析目录通知
        CatalogResponse response = parseCatalogXml(xml);
        
        // 调用通知回调
        std::lock_guard<std::mutex> lock(notify_callback_mutex_);
        if (notify_callback_) {
            notify_callback_(response.deviceId, response.channels);
        }
        
        SIPSERVER_LOG(LS_INFO) << "Received catalog NOTIFY from device: " 
                               << response.deviceId
                               << ", channels: " << response.channels.size();
    } catch (const std::exception& e) {
        SIPSERVER_LOG(LS_ERROR) << "Failed to parse catalog NOTIFY: " << e.what();
    }
}

void CatalogHandler::setNotifyCallback(
    std::function<void(const std::string& deviceId,
                       const std::vector<ChannelInfo>& channels)> callback) {
    std::lock_guard<std::mutex> lock(notify_callback_mutex_);
    notify_callback_ = callback;
}

CatalogResponse CatalogHandler::parseCatalogXml(const std::string& xml) {
    auto root = XmlHelper::parseXml(xml);
    if (!root) {
        throw std::runtime_error("Failed to parse XML");
    }
    
    CatalogResponse response;
    
    // 解析基本信息
    response.cmdType = XmlHelper::getNodeValue(root, "Response/CmdType");
    response.sn = XmlHelper::getNodeValue(root, "Response/SN");
    response.deviceId = XmlHelper::getNodeValue(root, "Response/DeviceID");
    
    std::string sumNumStr = XmlHelper::getNodeValue(root, "Response/SumNum");
    response.sumNum = sumNumStr.empty() ? 0 : std::stoi(sumNumStr);
    
    // 获取DeviceList节点
    auto responseNode = XmlHelper::getChildNode(root, "Response");
    if (!responseNode) {
        throw std::runtime_error("Response node not found");
    }
    
    auto deviceListNode = XmlHelper::getChildNode(responseNode, "DeviceList");
    if (!deviceListNode) {
        // 没有通道列表，返回空结果
        return response;
    }
    
    // 获取所有Item节点
    auto items = XmlHelper::getChildNodes(deviceListNode, "Item");
    
    // 解析每个通道
    for (const auto& item : items) {
        try {
            ChannelInfo channel = parseChannelInfo(item);
            response.channels.push_back(channel);
        } catch (const std::exception& e) {
            SIPSERVER_LOG(LS_WARNING) << "Failed to parse channel: " << e.what();
        }
    }
    
    return response;
}

ChannelInfo CatalogHandler::parseChannelInfo(const std::shared_ptr<XmlNode>& itemNode) {
    ChannelInfo channel;
    
    // 解析必填字段
    channel.channelId = XmlHelper::getNodeValue(itemNode, "DeviceID");
    channel.name = XmlHelper::getNodeValue(itemNode, "Name");
    
    // 解析可选字段
    channel.manufacturer = XmlHelper::getNodeValue(itemNode, "Manufacturer");
    channel.model = XmlHelper::getNodeValue(itemNode, "Model");
    channel.owner = XmlHelper::getNodeValue(itemNode, "Owner");
    channel.civilCode = XmlHelper::getNodeValue(itemNode, "CivilCode");
    channel.address = XmlHelper::getNodeValue(itemNode, "Address");
    channel.parentId = XmlHelper::getNodeValue(itemNode, "ParentID");
    channel.status = XmlHelper::getNodeValue(itemNode, "Status");
    
    // 解析整数字段
    std::string parentalStr = XmlHelper::getNodeValue(itemNode, "Parental");
    channel.parental = parentalStr.empty() ? 0 : std::stoi(parentalStr);
    
    std::string safetyWayStr = XmlHelper::getNodeValue(itemNode, "SafetyWay");
    channel.safetyWay = safetyWayStr.empty() ? 0 : std::stoi(safetyWayStr);
    
    std::string registerWayStr = XmlHelper::getNodeValue(itemNode, "RegisterWay");
    channel.registerWay = registerWayStr.empty() ? 1 : std::stoi(registerWayStr);
    
    std::string secrecyStr = XmlHelper::getNodeValue(itemNode, "Secrecy");
    channel.secrecy = secrecyStr.empty() ? 0 : std::stoi(secrecyStr);
    
    std::string ptzTypeStr = XmlHelper::getNodeValue(itemNode, "PTZType");
    channel.ptzType = ptzTypeStr.empty() ? 0 : std::stoi(ptzTypeStr);
    
    // 解析浮点数字段
    std::string longitudeStr = XmlHelper::getNodeValue(itemNode, "Longitude");
    channel.longitude = longitudeStr.empty() ? 0.0 : std::stod(longitudeStr);
    
    std::string latitudeStr = XmlHelper::getNodeValue(itemNode, "Latitude");
    channel.latitude = latitudeStr.empty() ? 0.0 : std::stod(latitudeStr);
    
    return channel;
}

} // namespace gbsip_server
