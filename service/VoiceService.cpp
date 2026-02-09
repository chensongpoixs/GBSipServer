/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Author: chensong
 *  Date: 2026-02-09
 *  Description: 语音对讲服务层实现
 ******************************************************************************/

#include "VoiceService.hpp"
#include "gbsip_server_log.h"

namespace gbsip_server {

VoiceService::VoiceService(std::shared_ptr<VoiceHandler> voiceHandler)
    : voice_handler_(voiceHandler)
{
    SIPSERVER_LOG(LS_INFO) << "VoiceService initialized";
}

VoiceService::~VoiceService() {
    SIPSERVER_LOG(LS_INFO) << "VoiceService destroyed";
}

oatpp::Object<VoiceResponseDto> VoiceService::startBroadcast(
    const oatpp::Object<VoiceBroadcastStartDto>& dto)
{
    // 1. 参数验证
    if (!dto->device_id || !dto->local_ip || !dto->local_port) {
        return createFailureResponse("Invalid parameters: missing required fields", 400);
    }

    // 2. 查询设备信息
    auto device = device_db_->getDeviceById(dto->device_id);
    if (!device) {
        return createFailureResponse("Device not found", 404);
    }

    // 3. 检查设备是否在线
    if (!device->online) {
        return createFailureResponse("Device is offline", 503);
    }

    // 4. 开始语音广播
    int callId = voice_handler_->startBroadcast(
        dto->device_id->c_str(),
        dto->local_ip->c_str(),
        dto->local_port,
        device->RemoteIP->c_str(),
        device->RemotePort
    );

    if (callId <= 0) {
        return createFailureResponse("Failed to start broadcast", 500);
    }

    SIPSERVER_LOG(LS_INFO) << "Broadcast started, device: " << dto->device_id->c_str()
                           << ", call_id: " << callId;

    return createSuccessResponse("Broadcast started", std::to_string(callId));
}

oatpp::Object<VoiceResponseDto> VoiceService::stopBroadcast(
    const oatpp::Object<VoiceControlDto>& dto)
{
    // 1. 参数验证
    if (!dto->call_id) {
        return createFailureResponse("Invalid parameters: missing call_id", 400);
    }

    // 2. 停止语音广播
    int callId = std::stoi(dto->call_id->c_str());
    bool success = voice_handler_->stopBroadcast(callId);

    if (!success) {
        return createFailureResponse("Failed to stop broadcast", 500);
    }

    SIPSERVER_LOG(LS_INFO) << "Broadcast stopped, call_id: " << callId;

    return createSuccessResponse("Broadcast stopped", dto->call_id);
}

oatpp::Object<VoiceResponseDto> VoiceService::startTalk(
    const oatpp::Object<VoiceTalkStartDto>& dto)
{
    // 1. 参数验证
    if (!dto->device_id || !dto->local_ip || !dto->local_port) {
        return createFailureResponse("Invalid parameters: missing required fields", 400);
    }

    // 2. 查询设备信息
    auto device = device_db_->getDeviceById(dto->device_id);
    if (!device) {
        return createFailureResponse("Device not found", 404);
    }

    // 3. 检查设备是否在线
    if (!device->online) {
        return createFailureResponse("Device is offline", 503);
    }

    // 4. 开始双向对讲
    int callId = voice_handler_->startTalk(
        dto->device_id->c_str(),
        dto->local_ip->c_str(),
        dto->local_port,
        device->RemoteIP->c_str(),
        device->RemotePort
    );

    if (callId <= 0) {
        return createFailureResponse("Failed to start talk", 500);
    }

    SIPSERVER_LOG(LS_INFO) << "Talk started, device: " << dto->device_id->c_str()
                           << ", call_id: " << callId;

    return createSuccessResponse("Talk started", std::to_string(callId));
}

oatpp::Object<VoiceResponseDto> VoiceService::stopTalk(
    const oatpp::Object<VoiceControlDto>& dto)
{
    // 1. 参数验证
    if (!dto->call_id) {
        return createFailureResponse("Invalid parameters: missing call_id", 400);
    }

    // 2. 停止双向对讲
    int callId = std::stoi(dto->call_id->c_str());
    bool success = voice_handler_->stopTalk(callId);

    if (!success) {
        return createFailureResponse("Failed to stop talk", 500);
    }

    SIPSERVER_LOG(LS_INFO) << "Talk stopped, call_id: " << callId;

    return createSuccessResponse("Talk stopped", dto->call_id);
}

oatpp::Object<VoiceResponseDto> VoiceService::createSuccessResponse(
    const std::string& message,
    const std::string& callId,
    const std::string& sdp)
{
    auto response = VoiceResponseDto::createShared();
    response->code = 200;
    response->status = "OK";
    response->message = message.c_str();
    if (!callId.empty()) {
        response->call_id = callId.c_str();
    }
    if (!sdp.empty()) {
        response->sdp = sdp.c_str();
    }
    return response;
}

oatpp::Object<VoiceResponseDto> VoiceService::createFailureResponse(
    const std::string& message, int code)
{
    auto response = VoiceResponseDto::createShared();
    response->code = code;
    response->status = "ERROR";
    response->message = message.c_str();
    return response;
}

} // namespace gbsip_server
