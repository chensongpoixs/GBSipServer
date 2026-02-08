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

#include "DeviceInfoService.hpp"
#include "sip_server.h"
#include "sip/DeviceInfoHandler.hpp"
#include "gbsip_server_log.h"
#include <future>
#include <ctime>

oatpp::Object<DeviceInfoDto> DeviceInfoService::queryDeviceInfo(
    const oatpp::String& deviceId) {
    
    // 检查设备是否存在
    auto device = m_deviceDb->getDeviceByDeviceId(deviceId);
    if (!device) {
        SIPSERVER_LOG(LS_WARNING) << "Device not found: " << deviceId->c_str();
        return nullptr;
    }
    
    // 获取SipServer实例
    auto& sipServer = gbsip_server::SipServer::GetInstance();
    auto deviceInfoHandler = sipServer.getDeviceInfoHandler();
    if (!deviceInfoHandler) {
        SIPSERVER_LOG(LS_ERROR) << "DeviceInfoHandler not initialized";
        return nullptr;
    }
    
    // 获取设备信息
    auto deviceInfo = sipServer.getDeviceInfo(deviceId->c_str());
    if (!deviceInfo) {
        SIPSERVER_LOG(LS_WARNING) << "Device not online: " << deviceId->c_str();
        return nullptr;
    }
    
    // 创建Promise用于同步等待
    auto promise = std::make_shared<std::promise<gbsip_server::DeviceInfo>>();
    auto future = promise->get_future();
    
    // 调用DeviceInfoHandler查询
    deviceInfoHandler->queryDeviceInfo(
        deviceId->c_str(),
        deviceInfo->ip,
        deviceInfo->port,
        [promise](const gbsip_server::DeviceInfo& info) {
            promise->set_value(info);
        },
        [promise](int code, const std::string& message) {
            promise->set_exception(
                std::make_exception_ptr(std::runtime_error(message))
            );
        }
    );
    
    // 等待响应（超时30秒）
    auto status = future.wait_for(std::chrono::seconds(30));
    if (status == std::future_status::timeout) {
        SIPSERVER_LOG(LS_ERROR) << "Query device info timeout: " << deviceId->c_str();
        return nullptr;
    }
    
    try {
        auto info = future.get();
        
        // 构建响应DTO
        auto dto = DeviceInfoDto::createShared();
        dto->deviceId = deviceId;
        dto->deviceName = info.deviceName;
        dto->manufacturer = info.manufacturer;
        dto->model = info.model;
        dto->firmware = info.firmware;
        dto->channel = info.channel;
        dto->sn = std::stoi(info.sn);
        
        SIPSERVER_LOG(LS_INFO) << "Query device info success: " << deviceId->c_str();
        
        return dto;
    } catch (const std::exception& e) {
        SIPSERVER_LOG(LS_ERROR) << "Query device info failed: " << e.what();
        return nullptr;
    }
}

oatpp::Object<DeviceStatusDto> DeviceInfoService::queryDeviceStatus(
    const oatpp::String& deviceId) {
    
    // 检查设备是否存在
    auto device = m_deviceDb->getDeviceByDeviceId(deviceId);
    if (!device) {
        SIPSERVER_LOG(LS_WARNING) << "Device not found: " << deviceId->c_str();
        return nullptr;
    }
    
    // 获取SipServer实例
    auto& sipServer = gbsip_server::SipServer::GetInstance();
    auto deviceInfoHandler = sipServer.getDeviceInfoHandler();
    if (!deviceInfoHandler) {
        SIPSERVER_LOG(LS_ERROR) << "DeviceInfoHandler not initialized";
        return nullptr;
    }
    
    // 获取设备信息
    auto deviceInfo = sipServer.getDeviceInfo(deviceId->c_str());
    if (!deviceInfo) {
        SIPSERVER_LOG(LS_WARNING) << "Device not online: " << deviceId->c_str();
        return nullptr;
    }
    
    // 创建Promise用于同步等待
    auto promise = std::make_shared<std::promise<gbsip_server::DeviceStatus>>();
    auto future = promise->get_future();
    
    // 调用DeviceInfoHandler查询
    deviceInfoHandler->queryDeviceStatus(
        deviceId->c_str(),
        deviceInfo->ip,
        deviceInfo->port,
        [promise](const gbsip_server::DeviceStatus& status) {
            promise->set_value(status);
        },
        [promise](int code, const std::string& message) {
            promise->set_exception(
                std::make_exception_ptr(std::runtime_error(message))
            );
        }
    );
    
    // 等待响应（超时30秒）
    auto waitStatus = future.wait_for(std::chrono::seconds(30));
    if (waitStatus == std::future_status::timeout) {
        SIPSERVER_LOG(LS_ERROR) << "Query device status timeout: " << deviceId->c_str();
        return nullptr;
    }
    
    try {
        auto status = future.get();
        
        // 构建响应DTO
        auto dto = DeviceStatusDto::createShared();
        dto->deviceId = deviceId;
        dto->online = status.online;
        dto->status = status.status;
        dto->encode = status.encode;
        dto->record = status.record;
        dto->deviceTime = status.deviceTime;
        dto->sn = std::stoi(status.sn);
        
        SIPSERVER_LOG(LS_INFO) << "Query device status success: " << deviceId->c_str();
        
        return dto;
    } catch (const std::exception& e) {
        SIPSERVER_LOG(LS_ERROR) << "Query device status failed: " << e.what();
        return nullptr;
    }
}

oatpp::Object<StatusDto> DeviceInfoService::queryDeviceConfig(
    const oatpp::String& deviceId) {
    
    auto status = StatusDto::createShared();
    status->code = 501;
    status->status = "NOT_IMPLEMENTED";
    status->message = "Device config query not implemented yet";
    
    return status;
}

oatpp::Int64 DeviceInfoService::getCurrentTimestamp() {
    return static_cast<oatpp::Int64>(std::time(nullptr));
}
