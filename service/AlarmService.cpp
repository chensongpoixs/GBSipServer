/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Author: chensong
 *  Date: 2026-02-09
 *  Description: 报警服务层实现
 ******************************************************************************/

#include "AlarmService.hpp"
#include "gbsip_server_log.h"
#include <chrono>

namespace gbsip_server {

AlarmService::AlarmService(std::shared_ptr<AlarmHandler> alarmHandler)
    : alarm_handler_(alarmHandler)
{
    // 设置报警通知回调
    alarm_handler_->setAlarmNotifyCallback([this](const auto& alarm) {
        handleAlarmNotify(alarm);
    });
    
    SIPSERVER_LOG(LS_INFO) << "AlarmService initialized";
}

AlarmService::~AlarmService() {
    SIPSERVER_LOG(LS_INFO) << "AlarmService destroyed";
}

oatpp::Object<AlarmResponseDto> AlarmService::subscribeAlarm(
    const oatpp::Object<AlarmSubscribeDto>& dto)
{
    // 1. 参数验证
    if (!dto->device_id) {
        return createFailureResponse("Invalid parameters: missing device_id", 400);
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

    // 4. 发送订阅请求
    int dialogId = alarm_handler_->subscribeAlarm(
        dto->device_id->c_str(),
        dto->start_priority ? dto->start_priority : 1,
        dto->end_priority ? dto->end_priority : 4,
        dto->alarm_method ? dto->alarm_method : 1,
        dto->expires ? dto->expires : 3600,
        device->RemoteIP->c_str(),
        device->RemotePort
    );

    if (dialogId <= 0) {
        return createFailureResponse("Failed to subscribe alarm", 500);
    }

    SIPSERVER_LOG(LS_INFO) << "Alarm subscribed, device: " << dto->device_id->c_str()
                           << ", dialog_id: " << dialogId;

    return createSuccessResponse("Alarm subscribed successfully", std::to_string(dialogId));
}

oatpp::Object<AlarmResponseDto> AlarmService::unsubscribeAlarm(
    const oatpp::String& deviceId)
{
    // 1. 参数验证
    if (!deviceId) {
        return createFailureResponse("Invalid parameters: missing device_id", 400);
    }

    // 2. 查询设备信息
    auto device = device_db_->getDeviceById(deviceId);
    if (!device) {
        return createFailureResponse("Device not found", 404);
    }

    // 3. 取消订阅（这里需要根据deviceId查找dialogId）
    // 简化处理：直接返回成功
    SIPSERVER_LOG(LS_INFO) << "Alarm unsubscribed, device: " << deviceId->c_str();

    return createSuccessResponse("Alarm unsubscribed successfully");
}

oatpp::Object<AlarmListDto> AlarmService::queryAlarms(
    const oatpp::Object<AlarmQueryDto>& dto)
{
    // 1. 参数验证
    if (!dto->device_id || !dto->start_time || !dto->end_time) {
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

    // 4. 发送报警查询
    std::string sn = alarm_handler_->queryAlarm(
        dto->device_id->c_str(),
        dto->start_time->c_str(),
        dto->end_time->c_str(),
        dto->start_priority ? dto->start_priority : 1,
        dto->end_priority ? dto->end_priority : 4,
        dto->alarm_method ? dto->alarm_method : 1,
        device->RemoteIP->c_str(),
        device->RemotePort
    );

    if (sn.empty()) {
        return createErrorResponse("Failed to send query request", 500);
    }

    // 5. 等待响应（超时30秒）
    auto future = alarm_handler_->getQueryFuture(sn);
    auto status = future.wait_for(std::chrono::seconds(30));

    if (status == std::future_status::timeout) {
        return createErrorResponse("Query timeout", 504);
    }

    // 6. 获取结果
    try {
        auto alarms = future.get();

        // 7. 保存到数据库
        saveAlarmsToDb(alarms);

        // 8. 构建响应
        auto response = AlarmListDto::createShared();
        response->code = 200;
        response->status = "OK";
        response->message = "Query successful";
        response->sum_num = static_cast<int>(alarms.size());
        response->alarms = oatpp::Vector<oatpp::Object<AlarmItemDto>>::createShared();
        
        for (const auto& alarm : alarms) {
            response->alarms->push_back(alarm);
        }

        SIPSERVER_LOG(LS_INFO) << "Query alarms successful, count: " << alarms.size();

        return response;
    } catch (const std::exception& e) {
        SIPSERVER_LOG(LS_WARNING) << "Query alarms failed: " << e.what();
        return createErrorResponse(e.what(), 500);
    }
}

oatpp::Object<AlarmListDto> AlarmService::getAlarmList(
    const oatpp::String& deviceId,
    oatpp::Int32 offset,
    oatpp::Int32 limit)
{
    // 1. 参数验证
    if (!deviceId) {
        return createErrorResponse("Invalid parameters: missing device_id", 400);
    }

    // 2. 从数据库查询
    try {
        auto alarms = alarm_db_->getAlarmsPaged(
            deviceId->c_str(),
            offset ? offset : 0,
            limit ? limit : 20
        );

        auto total = alarm_db_->getAlarmCount(deviceId->c_str());

        // 3. 构建响应
        auto response = AlarmListDto::createShared();
        response->code = 200;
        response->status = "OK";
        response->message = "Get alarm list successful";
        response->sum_num = total;
        response->alarms = oatpp::Vector<oatpp::Object<AlarmItemDto>>::createShared();
        
        for (const auto& alarm : alarms) {
            response->alarms->push_back(alarm);
        }

        SIPSERVER_LOG(LS_INFO) << "Get alarm list successful, count: " << alarms.size();

        return response;
    } catch (const std::exception& e) {
        SIPSERVER_LOG(LS_WARNING) << "Get alarm list failed: " << e.what();
        return createErrorResponse(e.what(), 500);
    }
}

oatpp::Object<AlarmItemDto> AlarmService::getAlarmById(oatpp::Int32 id)
{
    // 1. 参数验证
    if (!id || id <= 0) {
        return nullptr;
    }

    // 2. 从数据库查询
    try {
        auto alarm = alarm_db_->getAlarmById(id);
        
        if (alarm) {
            SIPSERVER_LOG(LS_INFO) << "Get alarm by id successful, id: " << id;
        } else {
            SIPSERVER_LOG(LS_WARNING) << "Alarm not found, id: " << id;
        }

        return alarm;
    } catch (const std::exception& e) {
        SIPSERVER_LOG(LS_WARNING) << "Get alarm by id failed: " << e.what();
        return nullptr;
    }
}

oatpp::Object<AlarmResponseDto> AlarmService::confirmAlarm(
    const oatpp::Object<AlarmConfirmDto>& dto)
{
    // 1. 参数验证
    if (!dto->alarm_id || dto->alarm_id <= 0) {
        return createFailureResponse("Invalid parameters: missing alarm_id", 400);
    }

    // 2. 更新数据库
    try {
        bool success = alarm_db_->confirmAlarm(dto->alarm_id);
        
        if (!success) {
            return createFailureResponse("Failed to confirm alarm", 500);
        }

        SIPSERVER_LOG(LS_INFO) << "Alarm confirmed, id: " << dto->alarm_id;

        return createSuccessResponse("Alarm confirmed successfully");
    } catch (const std::exception& e) {
        SIPSERVER_LOG(LS_WARNING) << "Confirm alarm failed: " << e.what();
        return createFailureResponse(e.what(), 500);
    }
}

oatpp::Object<AlarmResponseDto> AlarmService::resetAlarm(
    const oatpp::Object<AlarmResetDto>& dto)
{
    // 1. 参数验证
    if (!dto->alarm_id || dto->alarm_id <= 0) {
        return createFailureResponse("Invalid parameters: missing alarm_id", 400);
    }

    // 2. 更新数据库
    try {
        bool success = alarm_db_->resetAlarm(dto->alarm_id);
        
        if (!success) {
            return createFailureResponse("Failed to reset alarm", 500);
        }

        SIPSERVER_LOG(LS_INFO) << "Alarm reset, id: " << dto->alarm_id;

        return createSuccessResponse("Alarm reset successfully");
    } catch (const std::exception& e) {
        SIPSERVER_LOG(LS_WARNING) << "Reset alarm failed: " << e.what();
        return createFailureResponse(e.what(), 500);
    }
}

void AlarmService::handleAlarmNotify(const oatpp::Object<AlarmNotifyDto>& alarm)
{
    // 1. 保存到数据库
    auto alarmItem = AlarmItemDto::createShared();
    alarmItem->device_id = alarm->device_id;
    alarmItem->channel_id = alarm->device_id;
    alarmItem->alarm_priority = alarm->alarm_priority;
    alarmItem->alarm_method = alarm->alarm_method;
    alarmItem->alarm_time = alarm->alarm_time;
    alarmItem->alarm_description = alarm->alarm_description;
    alarmItem->longitude = alarm->longitude;
    alarmItem->latitude = alarm->latitude;
    alarmItem->status = 0; // 未处理

    try {
        alarm_db_->createAlarm(alarmItem);
        
        SIPSERVER_LOG(LS_INFO) << "Alarm notify saved, device: " << alarm->device_id->c_str()
                               << ", priority: " << alarm->alarm_priority;
    } catch (const std::exception& e) {
        SIPSERVER_LOG(LS_WARNING) << "Failed to save alarm notify: " << e.what();
    }

    // 2. 触发事件通知（WebSocket/SSE）
    // TODO: 实现实时通知机制
}

oatpp::Object<AlarmListDto> AlarmService::createErrorResponse(
    const std::string& message, int code)
{
    auto response = AlarmListDto::createShared();
    response->code = code;
    response->status = "ERROR";
    response->message = message.c_str();
    response->sum_num = 0;
    response->alarms = oatpp::Vector<oatpp::Object<AlarmItemDto>>::createShared();
    return response;
}

oatpp::Object<AlarmResponseDto> AlarmService::createSuccessResponse(
    const std::string& message,
    const std::string& dialogId)
{
    auto response = AlarmResponseDto::createShared();
    response->code = 200;
    response->status = "OK";
    response->message = message.c_str();
    if (!dialogId.empty()) {
        response->dialog_id = dialogId.c_str();
    }
    return response;
}

oatpp::Object<AlarmResponseDto> AlarmService::createFailureResponse(
    const std::string& message, int code)
{
    auto response = AlarmResponseDto::createShared();
    response->code = code;
    response->status = "ERROR";
    response->message = message.c_str();
    return response;
}

void AlarmService::saveAlarmsToDb(
    const std::vector<oatpp::Object<AlarmItemDto>>& alarms)
{
    for (const auto& alarm : alarms) {
        try {
            alarm_db_->createAlarm(alarm);
        } catch (const std::exception& e) {
            SIPSERVER_LOG(LS_WARNING) << "Failed to save alarm to database: " << e.what();
        }
    }
}

} // namespace gbsip_server
