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

#include "CatalogService.hpp"
#include "sip_server.h"
#include "utils/XmlHelper.hpp"
#include <ctime>

oatpp::Object<CatalogResponseDto> CatalogService::queryCatalog(const oatpp::String& deviceId) {
    // 检查设备是否存在
    auto device = m_deviceDb->getDeviceByDeviceId(deviceId);
    if (!device) {
        return nullptr;
    }
    
    // 从数据库获取通道列表
    auto channels = m_channelDb->getChannelsByDeviceId(deviceId);
    
    // 构建响应
    auto response = CatalogResponseDto::createShared();
    response->deviceId = deviceId;
    response->sn = 0; // 从数据库查询，没有SN
    response->sumNum = channels->size();
    response->channels = oatpp::Vector<oatpp::Object<ChannelDto>>::createShared();
    
    for (const auto& row : *channels) {
        auto channel = ChannelDto::createShared();
        channel->id = row->getValueByName("id", 0);
        channel->channelId = row->getValueByName("channel_id", "");
        channel->deviceId = row->getValueByName("device_id", "");
        channel->name = row->getValueByName("channel_name", "");
        channel->manufacturer = row->getValueByName("manufacturer", "");
        channel->model = row->getValueByName("model", "");
        channel->owner = row->getValueByName("owner", "");
        channel->civilCode = row->getValueByName("civil_code", "");
        channel->address = row->getValueByName("address", "");
        channel->parental = row->getValueByName("parental", 0);
        channel->parentId = row->getValueByName("parent_id", "");
        channel->safetyWay = row->getValueByName("safety_way", 0);
        channel->registerWay = row->getValueByName("register_way", 1);
        channel->secrecy = row->getValueByName("secrecy", 0);
        channel->status = row->getValueByName("status", "OFF");
        channel->longitude = row->getValueByName("longitude", 0.0);
        channel->latitude = row->getValueByName("latitude", 0.0);
        channel->ptzType = row->getValueByName("ptz_type", 0);
        channel->createTime = row->getValueByName("create_time", 0);
        channel->updateTime = row->getValueByName("update_time", 0);
        
        response->channels->push_back(channel);
    }
    
    // TODO: 向设备发送目录查询请求，更新数据库
    // 这里需要调用SipServer发送MESSAGE消息
    
    return response;
}

oatpp::Object<StatusDto> CatalogService::subscribeCatalog(
    const oatpp::Object<CatalogSubscribeDto>& dto) {
    
    auto status = StatusDto::createShared();
    
    // 检查设备是否存在
    auto device = m_deviceDb->getDeviceByDeviceId(dto->deviceId);
    if (!device) {
        status->code = 404;
        status->status = "ERROR";
        status->message = "Device not found";
        return status;
    }
    
    // TODO: 向设备发送SUBSCRIBE消息
    // 这里需要调用SipServer发送SUBSCRIBE消息
    
    status->code = 200;
    status->status = "OK";
    status->message = "Catalog subscribe request sent";
    return status;
}

oatpp::Object<StatusDto> CatalogService::unsubscribeCatalog(const oatpp::String& deviceId) {
    auto status = StatusDto::createShared();
    
    // 检查设备是否存在
    auto device = m_deviceDb->getDeviceByDeviceId(deviceId);
    if (!device) {
        status->code = 404;
        status->status = "ERROR";
        status->message = "Device not found";
        return status;
    }
    
    // TODO: 向设备发送SUBSCRIBE消息（expires=0）
    
    status->code = 200;
    status->status = "OK";
    status->message = "Catalog unsubscribe request sent";
    return status;
}

oatpp::Object<StatusDto> CatalogService::handleCatalogNotify(
    const oatpp::String& deviceId,
    const oatpp::Vector<oatpp::Object<ChannelDto>>& channels) {
    
    auto status = StatusDto::createShared();
    
    try {
        // 保存或更新通道信息
        for (const auto& channel : *channels) {
            channel->deviceId = deviceId;
            channel->updateTime = getCurrentTimestamp();
            if (!channel->createTime) {
                channel->createTime = channel->updateTime;
            }
            saveOrUpdateChannel(channel);
        }
        
        status->code = 200;
        status->status = "OK";
        status->message = "Catalog notify processed";
    } catch (const std::exception& e) {
        status->code = 500;
        status->status = "ERROR";
        status->message = e.what();
    }
    
    return status;
}

oatpp::Vector<oatpp::Object<ChannelDto>> CatalogService::getChannelsByDeviceId(
    const oatpp::String& deviceId) {
    
    auto channels = m_channelDb->getChannelsByDeviceId(deviceId);
    auto result = oatpp::Vector<oatpp::Object<ChannelDto>>::createShared();
    
    for (const auto& row : *channels) {
        auto channel = ChannelDto::createShared();
        channel->id = row->getValueByName("id", 0);
        channel->channelId = row->getValueByName("channel_id", "");
        channel->deviceId = row->getValueByName("device_id", "");
        channel->name = row->getValueByName("channel_name", "");
        channel->manufacturer = row->getValueByName("manufacturer", "");
        channel->model = row->getValueByName("model", "");
        channel->owner = row->getValueByName("owner", "");
        channel->civilCode = row->getValueByName("civil_code", "");
        channel->address = row->getValueByName("address", "");
        channel->parental = row->getValueByName("parental", 0);
        channel->parentId = row->getValueByName("parent_id", "");
        channel->safetyWay = row->getValueByName("safety_way", 0);
        channel->registerWay = row->getValueByName("register_way", 1);
        channel->secrecy = row->getValueByName("secrecy", 0);
        channel->status = row->getValueByName("status", "OFF");
        channel->longitude = row->getValueByName("longitude", 0.0);
        channel->latitude = row->getValueByName("latitude", 0.0);
        channel->ptzType = row->getValueByName("ptz_type", 0);
        channel->createTime = row->getValueByName("create_time", 0);
        channel->updateTime = row->getValueByName("update_time", 0);
        
        result->push_back(channel);
    }
    
    return result;
}

oatpp::Object<ChannelDto> CatalogService::getChannelById(const oatpp::String& channelId) {
    auto result = m_channelDb->getChannelById(channelId);
    
    if (result->isSuccess() && result->hasMoreToFetch()) {
        auto row = result->fetch<oatpp::Vector<oatpp::Object<oatpp::Fields<oatpp::Any>>>>()->at(0);
        
        auto channel = ChannelDto::createShared();
        channel->id = row->getValueByName("id", 0);
        channel->channelId = row->getValueByName("channel_id", "");
        channel->deviceId = row->getValueByName("device_id", "");
        channel->name = row->getValueByName("channel_name", "");
        channel->manufacturer = row->getValueByName("manufacturer", "");
        channel->model = row->getValueByName("model", "");
        channel->owner = row->getValueByName("owner", "");
        channel->civilCode = row->getValueByName("civil_code", "");
        channel->address = row->getValueByName("address", "");
        channel->parental = row->getValueByName("parental", 0);
        channel->parentId = row->getValueByName("parent_id", "");
        channel->safetyWay = row->getValueByName("safety_way", 0);
        channel->registerWay = row->getValueByName("register_way", 1);
        channel->secrecy = row->getValueByName("secrecy", 0);
        channel->status = row->getValueByName("status", "OFF");
        channel->longitude = row->getValueByName("longitude", 0.0);
        channel->latitude = row->getValueByName("latitude", 0.0);
        channel->ptzType = row->getValueByName("ptz_type", 0);
        channel->createTime = row->getValueByName("create_time", 0);
        channel->updateTime = row->getValueByName("update_time", 0);
        
        return channel;
    }
    
    return nullptr;
}

oatpp::Object<StatusDto> CatalogService::updateChannel(const oatpp::Object<ChannelDto>& channel) {
    auto status = StatusDto::createShared();
    
    try {
        channel->updateTime = getCurrentTimestamp();
        m_channelDb->updateChannel(channel);
        
        status->code = 200;
        status->status = "OK";
        status->message = "Channel updated successfully";
    } catch (const std::exception& e) {
        status->code = 500;
        status->status = "ERROR";
        status->message = e.what();
    }
    
    return status;
}

oatpp::Object<StatusDto> CatalogService::deleteChannel(const oatpp::String& channelId) {
    auto status = StatusDto::createShared();
    
    try {
        m_channelDb->deleteChannelById(channelId);
        
        status->code = 200;
        status->status = "OK";
        status->message = "Channel deleted successfully";
    } catch (const std::exception& e) {
        status->code = 500;
        status->status = "ERROR";
        status->message = e.what();
    }
    
    return status;
}

void CatalogService::saveOrUpdateChannel(const oatpp::Object<ChannelDto>& channel) {
    // 检查通道是否存在
    auto existing = m_channelDb->getChannelById(channel->channelId);
    
    if (existing->isSuccess() && existing->hasMoreToFetch()) {
        // 更新
        m_channelDb->updateChannel(channel);
    } else {
        // 创建
        m_channelDb->createChannel(channel);
    }
}

oatpp::Int64 CatalogService::getCurrentTimestamp() {
    return static_cast<oatpp::Int64>(std::time(nullptr));
}
