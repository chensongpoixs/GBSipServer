/*
*  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source tree.
*/
/*****************************************************************************
				  Author: chensong
				  date:  2026-02-09
				  
				  PTZ控制服务实现
******************************************************************************/

#include "PTZService.hpp"
#include "utils/PTZCommandEncoder.hpp"
#include "gbsip_server_log.h"

PTZService::PTZService() {
    SIPSERVER_LOG(LS_INFO) << "PTZService created";
}

PTZService::~PTZService() {
    SIPSERVER_LOG(LS_INFO) << "PTZService destroyed";
}

void PTZService::setPTZHandler(std::shared_ptr<gbsip_server::PTZHandler> handler) {
    ptz_handler_ = handler;
}

oatpp::Object<PTZResponseDto> PTZService::controlDirection(
    const oatpp::Object<PTZControlDto>& dto)
{
    // 参数验证
    if (!dto->deviceId || !dto->channelId || !dto->direction) {
        return createErrorResponse(400, "Missing required parameters");
    }
    
    if (!ptz_handler_) {
        return createErrorResponse(500, "PTZ handler not initialized");
    }
    
    // 获取设备信息
    std::string deviceIp;
    int devicePort;
    if (!getDeviceInfo(dto->deviceId, deviceIp, devicePort)) {
        return createErrorResponse(404, "Device not found or offline");
    }
    
    // 转换方向字符串为枚举
    auto direction = gbsip_server::PTZCommandEncoder::stringToDirection(dto->direction);
    
    // 编码PTZ命令
    std::string ptzCmd = gbsip_server::PTZCommandEncoder::encodeDirection(
        direction, dto->hSpeed, dto->vSpeed);
    
    SIPSERVER_LOG(LS_INFO) << "PTZ direction control: device=" << dto->deviceId 
                           << ", channel=" << dto->channelId 
                           << ", direction=" << dto->direction
                           << ", cmd=" << ptzCmd;
    
    // 发送控制命令
    bool success = ptz_handler_->sendPTZControl(
        dto->deviceId, dto->channelId, ptzCmd, deviceIp, devicePort);
    
    if (success) {
        return createSuccessResponse("PTZ control command sent successfully");
    } else {
        return createErrorResponse(500, "Failed to send PTZ control command");
    }
}

oatpp::Object<PTZResponseDto> PTZService::controlZoom(
    const oatpp::Object<PTZZoomDto>& dto)
{
    if (!dto->deviceId || !dto->channelId) {
        return createErrorResponse(400, "Missing required parameters");
    }
    
    if (!ptz_handler_) {
        return createErrorResponse(500, "PTZ handler not initialized");
    }
    
    std::string deviceIp;
    int devicePort;
    if (!getDeviceInfo(dto->deviceId, deviceIp, devicePort)) {
        return createErrorResponse(404, "Device not found or offline");
    }
    
    std::string ptzCmd = gbsip_server::PTZCommandEncoder::encodeZoom(
        dto->zoomIn, dto->speed);
    
    SIPSERVER_LOG(LS_INFO) << "PTZ zoom control: device=" << dto->deviceId 
                           << ", zoomIn=" << dto->zoomIn 
                           << ", cmd=" << ptzCmd;
    
    bool success = ptz_handler_->sendPTZControl(
        dto->deviceId, dto->channelId, ptzCmd, deviceIp, devicePort);
    
    return success ? createSuccessResponse("Zoom control sent successfully")
                   : createErrorResponse(500, "Failed to send zoom control");
}

oatpp::Object<PTZResponseDto> PTZService::controlFocus(
    const oatpp::Object<PTZFocusDto>& dto)
{
    if (!dto->deviceId || !dto->channelId) {
        return createErrorResponse(400, "Missing required parameters");
    }
    
    if (!ptz_handler_) {
        return createErrorResponse(500, "PTZ handler not initialized");
    }
    
    std::string deviceIp;
    int devicePort;
    if (!getDeviceInfo(dto->deviceId, deviceIp, devicePort)) {
        return createErrorResponse(404, "Device not found or offline");
    }
    
    std::string ptzCmd = gbsip_server::PTZCommandEncoder::encodeFocus(
        dto->focusNear, dto->speed);
    
    SIPSERVER_LOG(LS_INFO) << "PTZ focus control: device=" << dto->deviceId 
                           << ", focusNear=" << dto->focusNear 
                           << ", cmd=" << ptzCmd;
    
    bool success = ptz_handler_->sendPTZControl(
        dto->deviceId, dto->channelId, ptzCmd, deviceIp, devicePort);
    
    return success ? createSuccessResponse("Focus control sent successfully")
                   : createErrorResponse(500, "Failed to send focus control");
}

oatpp::Object<PTZResponseDto> PTZService::controlIris(
    const oatpp::Object<PTZIrisDto>& dto)
{
    if (!dto->deviceId || !dto->channelId) {
        return createErrorResponse(400, "Missing required parameters");
    }
    
    if (!ptz_handler_) {
        return createErrorResponse(500, "PTZ handler not initialized");
    }
    
    std::string deviceIp;
    int devicePort;
    if (!getDeviceInfo(dto->deviceId, deviceIp, devicePort)) {
        return createErrorResponse(404, "Device not found or offline");
    }
    
    std::string ptzCmd = gbsip_server::PTZCommandEncoder::encodeIris(
        dto->irisOpen, dto->speed);
    
    SIPSERVER_LOG(LS_INFO) << "PTZ iris control: device=" << dto->deviceId 
                           << ", irisOpen=" << dto->irisOpen 
                           << ", cmd=" << ptzCmd;
    
    bool success = ptz_handler_->sendPTZControl(
        dto->deviceId, dto->channelId, ptzCmd, deviceIp, devicePort);
    
    return success ? createSuccessResponse("Iris control sent successfully")
                   : createErrorResponse(500, "Failed to send iris control");
}

oatpp::Object<PTZResponseDto> PTZService::setPreset(
    const oatpp::Object<PTZPresetDto>& dto)
{
    if (!dto->deviceId || !dto->channelId || dto->presetId <= 0) {
        return createErrorResponse(400, "Missing required parameters");
    }
    
    if (!ptz_handler_) {
        return createErrorResponse(500, "PTZ handler not initialized");
    }
    
    std::string deviceIp;
    int devicePort;
    if (!getDeviceInfo(dto->deviceId, deviceIp, devicePort)) {
        return createErrorResponse(404, "Device not found or offline");
    }
    
    std::string ptzCmd = gbsip_server::PTZCommandEncoder::encodePresetSet(dto->presetId);
    
    SIPSERVER_LOG(LS_INFO) << "PTZ set preset: device=" << dto->deviceId 
                           << ", presetId=" << dto->presetId 
                           << ", cmd=" << ptzCmd;
    
    bool success = ptz_handler_->sendPTZControl(
        dto->deviceId, dto->channelId, ptzCmd, deviceIp, devicePort);
    
    return success ? createSuccessResponse("Preset set successfully")
                   : createErrorResponse(500, "Failed to set preset");
}

oatpp::Object<PTZResponseDto> PTZService::callPreset(
    const oatpp::Object<PTZPresetDto>& dto)
{
    if (!dto->deviceId || !dto->channelId || dto->presetId <= 0) {
        return createErrorResponse(400, "Missing required parameters");
    }
    
    if (!ptz_handler_) {
        return createErrorResponse(500, "PTZ handler not initialized");
    }
    
    std::string deviceIp;
    int devicePort;
    if (!getDeviceInfo(dto->deviceId, deviceIp, devicePort)) {
        return createErrorResponse(404, "Device not found or offline");
    }
    
    std::string ptzCmd = gbsip_server::PTZCommandEncoder::encodePresetCall(dto->presetId);
    
    SIPSERVER_LOG(LS_INFO) << "PTZ call preset: device=" << dto->deviceId 
                           << ", presetId=" << dto->presetId 
                           << ", cmd=" << ptzCmd;
    
    bool success = ptz_handler_->sendPTZControl(
        dto->deviceId, dto->channelId, ptzCmd, deviceIp, devicePort);
    
    return success ? createSuccessResponse("Preset called successfully")
                   : createErrorResponse(500, "Failed to call preset");
}

oatpp::Object<PTZResponseDto> PTZService::deletePreset(
    const oatpp::Object<PTZPresetDto>& dto)
{
    if (!dto->deviceId || !dto->channelId || dto->presetId <= 0) {
        return createErrorResponse(400, "Missing required parameters");
    }
    
    if (!ptz_handler_) {
        return createErrorResponse(500, "PTZ handler not initialized");
    }
    
    std::string deviceIp;
    int devicePort;
    if (!getDeviceInfo(dto->deviceId, deviceIp, devicePort)) {
        return createErrorResponse(404, "Device not found or offline");
    }
    
    std::string ptzCmd = gbsip_server::PTZCommandEncoder::encodePresetDelete(dto->presetId);
    
    SIPSERVER_LOG(LS_INFO) << "PTZ delete preset: device=" << dto->deviceId 
                           << ", presetId=" << dto->presetId 
                           << ", cmd=" << ptzCmd;
    
    bool success = ptz_handler_->sendPTZControl(
        dto->deviceId, dto->channelId, ptzCmd, deviceIp, devicePort);
    
    return success ? createSuccessResponse("Preset deleted successfully")
                   : createErrorResponse(500, "Failed to delete preset");
}

oatpp::Object<PTZResponseDto> PTZService::startCruise(
    const oatpp::Object<PTZCruiseDto>& dto)
{
    if (!dto->deviceId || !dto->channelId || dto->cruiseId <= 0) {
        return createErrorResponse(400, "Missing required parameters");
    }
    
    if (!ptz_handler_) {
        return createErrorResponse(500, "PTZ handler not initialized");
    }
    
    std::string deviceIp;
    int devicePort;
    if (!getDeviceInfo(dto->deviceId, deviceIp, devicePort)) {
        return createErrorResponse(404, "Device not found or offline");
    }
    
    std::string ptzCmd = gbsip_server::PTZCommandEncoder::encodeCruiseStart(
        dto->cruiseId, dto->speed);
    
    SIPSERVER_LOG(LS_INFO) << "PTZ start cruise: device=" << dto->deviceId 
                           << ", cruiseId=" << dto->cruiseId 
                           << ", cmd=" << ptzCmd;
    
    bool success = ptz_handler_->sendPTZControl(
        dto->deviceId, dto->channelId, ptzCmd, deviceIp, devicePort);
    
    return success ? createSuccessResponse("Cruise started successfully")
                   : createErrorResponse(500, "Failed to start cruise");
}

oatpp::Object<PTZResponseDto> PTZService::stopCruise(
    const oatpp::Object<PTZCruiseDto>& dto)
{
    if (!dto->deviceId || !dto->channelId || dto->cruiseId <= 0) {
        return createErrorResponse(400, "Missing required parameters");
    }
    
    if (!ptz_handler_) {
        return createErrorResponse(500, "PTZ handler not initialized");
    }
    
    std::string deviceIp;
    int devicePort;
    if (!getDeviceInfo(dto->deviceId, deviceIp, devicePort)) {
        return createErrorResponse(404, "Device not found or offline");
    }
    
    std::string ptzCmd = gbsip_server::PTZCommandEncoder::encodeCruiseStop(dto->cruiseId);
    
    SIPSERVER_LOG(LS_INFO) << "PTZ stop cruise: device=" << dto->deviceId 
                           << ", cruiseId=" << dto->cruiseId 
                           << ", cmd=" << ptzCmd;
    
    bool success = ptz_handler_->sendPTZControl(
        dto->deviceId, dto->channelId, ptzCmd, deviceIp, devicePort);
    
    return success ? createSuccessResponse("Cruise stopped successfully")
                   : createErrorResponse(500, "Failed to stop cruise");
}

oatpp::Object<PTZResponseDto> PTZService::setGuard(
    const oatpp::Object<PTZGuardDto>& dto)
{
    if (!dto->deviceId || !dto->channelId) {
        return createErrorResponse(400, "Missing required parameters");
    }
    
    if (!ptz_handler_) {
        return createErrorResponse(500, "PTZ handler not initialized");
    }
    
    std::string deviceIp;
    int devicePort;
    if (!getDeviceInfo(dto->deviceId, deviceIp, devicePort)) {
        return createErrorResponse(404, "Device not found or offline");
    }
    
    std::string ptzCmd = gbsip_server::PTZCommandEncoder::encodeGuard(
        dto->enabled, dto->timeout);
    
    SIPSERVER_LOG(LS_INFO) << "PTZ set guard: device=" << dto->deviceId 
                           << ", enabled=" << dto->enabled 
                           << ", cmd=" << ptzCmd;
    
    bool success = ptz_handler_->sendPTZControl(
        dto->deviceId, dto->channelId, ptzCmd, deviceIp, devicePort);
    
    return success ? createSuccessResponse("Guard position set successfully")
                   : createErrorResponse(500, "Failed to set guard position");
}

oatpp::Object<PTZResponseDto> PTZService::createSuccessResponse(const std::string& message) {
    auto response = PTZResponseDto::createShared();
    response->code = 200;
    response->status = "OK";
    response->message = message;
    return response;
}

oatpp::Object<PTZResponseDto> PTZService::createErrorResponse(int code, const std::string& message) {
    auto response = PTZResponseDto::createShared();
    response->code = code;
    response->status = "ERROR";
    response->message = message;
    return response;
}

bool PTZService::getDeviceInfo(const std::string& deviceId, std::string& ip, int& port) {
    if (!device_db_) {
        SIPSERVER_LOG(LS_WARNING) << "Device database not initialized";
        return false;
    }
    
    try {
        auto device = device_db_->getDeviceById(deviceId);
        if (!device) {
            SIPSERVER_LOG(LS_WARNING) << "Device not found: " << deviceId;
            return false;
        }
        
        if (!device->online) {
            SIPSERVER_LOG(LS_WARNING) << "Device offline: " << deviceId;
            return false;
        }
        
        ip = device->RemoteIP->std_str();
        port = device->RemotePort;
        return true;
    } catch (const std::exception& e) {
        SIPSERVER_LOG(LS_WARNING) << "Failed to get device info: " << e.what();
        return false;
    }
}
