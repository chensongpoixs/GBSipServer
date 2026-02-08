/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Author: chensong
 *  Date: 2026-02-09
 *  Description: 录像服务层实现
 ******************************************************************************/

#include "RecordService.hpp"
#include "gbsip_server_log.h"
#include <chrono>

namespace gbsip_server {

RecordService::RecordService(std::shared_ptr<RecordHandler> recordHandler)
    : record_handler_(recordHandler)
{
    SIPSERVER_LOG(LS_INFO) << "RecordService initialized";
}

RecordService::~RecordService() {
    SIPSERVER_LOG(LS_INFO) << "RecordService destroyed";
}

oatpp::Object<RecordListDto> RecordService::queryRecords(
    const oatpp::Object<RecordQueryDto>& dto)
{
    // 1. 参数验证
    if (!dto->device_id || !dto->channel_id || !dto->start_time || !dto->end_time) {
        return createErrorResponse("Invalid parameters: missing required fields", 400);
    }

    // 2. 查询设备信息
    auto device = device_db_->getDeviceById(dto->device_id);
    if (!device) {
        return createErrorResponse("Device not found", 404);
    }

    // 3. 检查设备是否在线
    if (!device->online) {
        return createErrorResponse("Device is offline", 503);
    }

    // 4. 发送录像查询
    std::string sn = record_handler_->queryRecordInfo(
        dto->device_id->c_str(),
        dto->channel_id->c_str(),
        dto->start_time->c_str(),
        dto->end_time->c_str(),
        dto->type ? dto->type->c_str() : "all",
        device->RemoteIP->c_str(),
        device->RemotePort
    );

    if (sn.empty()) {
        return createErrorResponse("Failed to send query request", 500);
    }

    // 5. 等待响应（超时30秒）
    auto future = record_handler_->getQueryFuture(sn);
    auto status = future.wait_for(std::chrono::seconds(30));

    if (status == std::future_status::timeout) {
        return createErrorResponse("Query timeout", 504);
    }

    // 6. 获取结果
    try {
        auto records = future.get();

        // 7. 保存到数据库
        saveRecordsToDb(records);

        // 8. 构建响应
        auto response = RecordListDto::createShared();
        response->code = 200;
        response->status = "OK";
        response->message = "Query successful";
        response->sum_num = static_cast<int>(records.size());
        response->records = oatpp::Vector<oatpp::Object<RecordItemDto>>::createShared();
        
        for (const auto& record : records) {
            response->records->push_back(record);
        }

        SIPSERVER_LOG(LS_INFO) << "Query records successful, count: " << records.size();

        return response;
    } catch (const std::exception& e) {
        SIPSERVER_LOG(LS_WARNING) << "Query records failed: " << e.what();
        return createErrorResponse(e.what(), 500);
    }
}

oatpp::Object<RecordResponseDto> RecordService::startPlayback(
    const oatpp::Object<PlaybackStartDto>& dto)
{
    // 1. 参数验证
    if (!dto->device_id || !dto->channel_id || !dto->start_time || 
        !dto->end_time || !dto->local_ip || !dto->local_port) {
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

    // 4. 发送回放INVITE
    int callId = record_handler_->startPlayback(
        dto->device_id->c_str(),
        dto->channel_id->c_str(),
        dto->start_time->c_str(),
        dto->end_time->c_str(),
        dto->local_ip->c_str(),
        dto->local_port,
        device->RemoteIP->c_str(),
        device->RemotePort
    );

    if (callId <= 0) {
        return createFailureResponse("Failed to start playback", 500);
    }

    SIPSERVER_LOG(LS_INFO) << "Playback started, call_id: " << callId;

    return createSuccessResponse("Playback started", std::to_string(callId));
}

oatpp::Object<RecordResponseDto> RecordService::pausePlayback(
    const oatpp::Object<PlaybackControlDto>& dto)
{
    // 1. 参数验证
    if (!dto->device_id || !dto->channel_id || !dto->call_id) {
        return createFailureResponse("Invalid parameters: missing required fields", 400);
    }

    // 2. 查询设备信息
    auto device = device_db_->getDeviceById(dto->device_id);
    if (!device) {
        return createFailureResponse("Device not found", 404);
    }

    // 3. 发送暂停控制
    int callId = std::stoi(dto->call_id->c_str());
    bool success = record_handler_->pausePlayback(
        dto->device_id->c_str(),
        dto->channel_id->c_str(),
        callId,
        device->RemoteIP->c_str(),
        device->RemotePort
    );

    if (!success) {
        return createFailureResponse("Failed to pause playback", 500);
    }

    SIPSERVER_LOG(LS_INFO) << "Playback paused, call_id: " << callId;

    return createSuccessResponse("Playback paused", dto->call_id);
}

oatpp::Object<RecordResponseDto> RecordService::resumePlayback(
    const oatpp::Object<PlaybackControlDto>& dto)
{
    // 1. 参数验证
    if (!dto->device_id || !dto->channel_id || !dto->call_id) {
        return createFailureResponse("Invalid parameters: missing required fields", 400);
    }

    // 2. 查询设备信息
    auto device = device_db_->getDeviceById(dto->device_id);
    if (!device) {
        return createFailureResponse("Device not found", 404);
    }

    // 3. 发送继续控制
    int callId = std::stoi(dto->call_id->c_str());
    bool success = record_handler_->resumePlayback(
        dto->device_id->c_str(),
        dto->channel_id->c_str(),
        callId,
        device->RemoteIP->c_str(),
        device->RemotePort
    );

    if (!success) {
        return createFailureResponse("Failed to resume playback", 500);
    }

    SIPSERVER_LOG(LS_INFO) << "Playback resumed, call_id: " << callId;

    return createSuccessResponse("Playback resumed", dto->call_id);
}

oatpp::Object<RecordResponseDto> RecordService::stopPlayback(
    const oatpp::Object<PlaybackControlDto>& dto)
{
    // 1. 参数验证
    if (!dto->call_id) {
        return createFailureResponse("Invalid parameters: missing call_id", 400);
    }

    // 2. 发送停止控制
    int callId = std::stoi(dto->call_id->c_str());
    bool success = record_handler_->stopPlayback(callId);

    if (!success) {
        return createFailureResponse("Failed to stop playback", 500);
    }

    SIPSERVER_LOG(LS_INFO) << "Playback stopped, call_id: " << callId;

    return createSuccessResponse("Playback stopped", dto->call_id);
}

oatpp::Object<RecordResponseDto> RecordService::seekPlayback(
    const oatpp::Object<PlaybackSeekDto>& dto)
{
    // 1. 参数验证
    if (!dto->device_id || !dto->channel_id || !dto->call_id || !dto->seek_time) {
        return createFailureResponse("Invalid parameters: missing required fields", 400);
    }

    // 2. 查询设备信息
    auto device = device_db_->getDeviceById(dto->device_id);
    if (!device) {
        return createFailureResponse("Device not found", 404);
    }

    // 3. 发送跳转控制
    int callId = std::stoi(dto->call_id->c_str());
    bool success = record_handler_->seekPlayback(
        dto->device_id->c_str(),
        dto->channel_id->c_str(),
        callId,
        dto->seek_time->c_str(),
        device->RemoteIP->c_str(),
        device->RemotePort
    );

    if (!success) {
        return createFailureResponse("Failed to seek playback", 500);
    }

    SIPSERVER_LOG(LS_INFO) << "Playback seeked, call_id: " << callId 
                           << ", seek_time: " << dto->seek_time->c_str();

    return createSuccessResponse("Playback seeked", dto->call_id);
}

oatpp::Object<RecordResponseDto> RecordService::setPlaybackSpeed(
    const oatpp::Object<PlaybackSpeedDto>& dto)
{
    // 1. 参数验证
    if (!dto->device_id || !dto->channel_id || !dto->call_id) {
        return createFailureResponse("Invalid parameters: missing required fields", 400);
    }

    // 2. 查询设备信息
    auto device = device_db_->getDeviceById(dto->device_id);
    if (!device) {
        return createFailureResponse("Device not found", 404);
    }

    // 3. 发送速度控制
    int callId = std::stoi(dto->call_id->c_str());
    bool success = record_handler_->setPlaybackSpeed(
        dto->device_id->c_str(),
        dto->channel_id->c_str(),
        callId,
        dto->speed,
        device->RemoteIP->c_str(),
        device->RemotePort
    );

    if (!success) {
        return createFailureResponse("Failed to set playback speed", 500);
    }

    SIPSERVER_LOG(LS_INFO) << "Playback speed set, call_id: " << callId 
                           << ", speed: " << dto->speed;

    return createSuccessResponse("Playback speed set", dto->call_id);
}

oatpp::Object<RecordResponseDto> RecordService::startDownload(
    const oatpp::Object<DownloadStartDto>& dto)
{
    // 1. 参数验证
    if (!dto->device_id || !dto->channel_id || !dto->start_time || 
        !dto->end_time || !dto->local_ip || !dto->local_port) {
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

    // 4. 发送下载INVITE
    int callId = record_handler_->startDownload(
        dto->device_id->c_str(),
        dto->channel_id->c_str(),
        dto->start_time->c_str(),
        dto->end_time->c_str(),
        dto->local_ip->c_str(),
        dto->local_port,
        device->RemoteIP->c_str(),
        device->RemotePort
    );

    if (callId <= 0) {
        return createFailureResponse("Failed to start download", 500);
    }

    SIPSERVER_LOG(LS_INFO) << "Download started, call_id: " << callId;

    return createSuccessResponse("Download started", std::to_string(callId));
}

oatpp::Object<RecordResponseDto> RecordService::stopDownload(
    const oatpp::Object<PlaybackControlDto>& dto)
{
    // 1. 参数验证
    if (!dto->call_id) {
        return createFailureResponse("Invalid parameters: missing call_id", 400);
    }

    // 2. 发送停止控制
    int callId = std::stoi(dto->call_id->c_str());
    bool success = record_handler_->stopDownload(callId);

    if (!success) {
        return createFailureResponse("Failed to stop download", 500);
    }

    SIPSERVER_LOG(LS_INFO) << "Download stopped, call_id: " << callId;

    return createSuccessResponse("Download stopped", dto->call_id);
}

oatpp::Object<RecordListDto> RecordService::createErrorResponse(
    const std::string& message, int code)
{
    auto response = RecordListDto::createShared();
    response->code = code;
    response->status = "ERROR";
    response->message = message.c_str();
    response->sum_num = 0;
    response->records = oatpp::Vector<oatpp::Object<RecordItemDto>>::createShared();
    return response;
}

oatpp::Object<RecordResponseDto> RecordService::createSuccessResponse(
    const std::string& message,
    const std::string& callId,
    const std::string& sdp)
{
    auto response = RecordResponseDto::createShared();
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

oatpp::Object<RecordResponseDto> RecordService::createFailureResponse(
    const std::string& message, int code)
{
    auto response = RecordResponseDto::createShared();
    response->code = code;
    response->status = "ERROR";
    response->message = message.c_str();
    return response;
}

void RecordService::saveRecordsToDb(
    const std::vector<oatpp::Object<RecordItemDto>>& records)
{
    for (const auto& record : records) {
        try {
            record_db_->createRecord(record);
        } catch (const std::exception& e) {
            SIPSERVER_LOG(LS_WARNING) << "Failed to save record to database: " << e.what();
        }
    }
}

} // namespace gbsip_server
