/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Author: chensong
 *  Date: 2026-02-09
 *  Description: 报警服务层
 ******************************************************************************/

#ifndef GBSIPSERVER_SERVICE_ALARMSERVICE_HPP
#define GBSIPSERVER_SERVICE_ALARMSERVICE_HPP

#include "dto/AlarmDto.hpp"
#include "db/AlarmDb.hpp"
#include "db/DeviceDb.hpp"
#include "sip/AlarmHandler.hpp"
#include "oatpp/core/macro/component.hpp"
#include <memory>

namespace gbsip_server {

/**
 * 报警服务
 * 负责报警管理的业务逻辑
 */
class AlarmService {
public:
    /**
     * 构造函数
     */
    AlarmService(std::shared_ptr<AlarmHandler> alarmHandler);

    /**
     * 析构函数
     */
    ~AlarmService();

    /**
     * 订阅报警
     */
    oatpp::Object<AlarmResponseDto> subscribeAlarm(
        const oatpp::Object<AlarmSubscribeDto>& dto);

    /**
     * 取消订阅报警
     */
    oatpp::Object<AlarmResponseDto> unsubscribeAlarm(
        const oatpp::String& deviceId);

    /**
     * 查询报警列表
     */
    oatpp::Object<AlarmListDto> queryAlarms(
        const oatpp::Object<AlarmQueryDto>& dto);

    /**
     * 获取报警列表（分页）
     */
    oatpp::Object<AlarmListDto> getAlarmList(
        const oatpp::String& deviceId,
        oatpp::Int32 offset,
        oatpp::Int32 limit);

    /**
     * 获取报警详情
     */
    oatpp::Object<AlarmItemDto> getAlarmById(oatpp::Int32 id);

    /**
     * 确认报警
     */
    oatpp::Object<AlarmResponseDto> confirmAlarm(
        const oatpp::Object<AlarmConfirmDto>& dto);

    /**
     * 复位报警
     */
    oatpp::Object<AlarmResponseDto> resetAlarm(
        const oatpp::Object<AlarmResetDto>& dto);

    /**
     * 处理报警通知
     */
    void handleAlarmNotify(const oatpp::Object<AlarmNotifyDto>& alarm);

private:
    /**
     * 创建错误响应
     */
    oatpp::Object<AlarmListDto> createErrorResponse(
        const std::string& message, int code = 500);

    /**
     * 创建成功响应
     */
    oatpp::Object<AlarmResponseDto> createSuccessResponse(
        const std::string& message, 
        const std::string& dialogId = "");

    /**
     * 创建失败响应
     */
    oatpp::Object<AlarmResponseDto> createFailureResponse(
        const std::string& message, int code = 500);

    /**
     * 保存报警到数据库
     */
    void saveAlarmsToDb(const std::vector<oatpp::Object<AlarmItemDto>>& alarms);

private:
    OATPP_COMPONENT(std::shared_ptr<AlarmDb>, alarm_db_);
    OATPP_COMPONENT(std::shared_ptr<DeviceDb>, device_db_);
    std::shared_ptr<AlarmHandler> alarm_handler_;
};

} // namespace gbsip_server

#endif // GBSIPSERVER_SERVICE_ALARMSERVICE_HPP
