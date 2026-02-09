/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Author: chensong
 *  Date: 2026-02-09
 *  Description: 报警REST控制器
 ******************************************************************************/

#ifndef GBSIPSERVER_CONTROLLER_ALARMCONTROLLER_HPP
#define GBSIPSERVER_CONTROLLER_ALARMCONTROLLER_HPP

#include "service/AlarmService.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

namespace gbsip_server {

/**
 * 报警REST控制器
 * 提供报警管理的HTTP API接口
 */
class AlarmController : public oatpp::web::server::api::ApiController {
public:
    AlarmController(const std::shared_ptr<ObjectMapper>& objectMapper)
        : oatpp::web::server::api::ApiController(objectMapper)
    {
    }

    static std::shared_ptr<AlarmController> createShared(
        OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
    {
        return std::make_shared<AlarmController>(objectMapper);
    }

    /**
     * 设置AlarmService
     */
    void setAlarmService(std::shared_ptr<AlarmService> service) {
        alarm_service_ = service;
    }

    /**
     * 订阅报警
     */
    ENDPOINT_INFO(subscribeAlarm) {
        info->summary = "Subscribe alarm";
        info->description = "Subscribe device alarm notifications";
        info->addConsumes<Object<AlarmSubscribeDto>>("application/json");
        info->addResponse<Object<AlarmResponseDto>>(Status::CODE_200, "application/json");
        info->addTag("Alarm Management");
    }
    ENDPOINT("POST", "/api/v1/alarm/subscribe", subscribeAlarm,
             BODY_DTO(Object<AlarmSubscribeDto>, dto))
    {
        if (!alarm_service_) {
            auto response = AlarmResponseDto::createShared();
            response->code = 500;
            response->status = "ERROR";
            response->message = "AlarmService not initialized";
            return createDtoResponse(Status::CODE_500, response);
        }
        
        auto result = alarm_service_->subscribeAlarm(dto);
        return createDtoResponse(Status::CODE_200, result);
    }

    /**
     * 取消订阅报警
     */
    ENDPOINT_INFO(unsubscribeAlarm) {
        info->summary = "Unsubscribe alarm";
        info->description = "Unsubscribe device alarm notifications";
        info->addConsumes<Object<AlarmSubscribeDto>>("application/json");
        info->addResponse<Object<AlarmResponseDto>>(Status::CODE_200, "application/json");
        info->addTag("Alarm Management");
    }
    ENDPOINT("POST", "/api/v1/alarm/unsubscribe", unsubscribeAlarm,
             BODY_DTO(Object<AlarmSubscribeDto>, dto))
    {
        if (!alarm_service_) {
            auto response = AlarmResponseDto::createShared();
            response->code = 500;
            response->status = "ERROR";
            response->message = "AlarmService not initialized";
            return createDtoResponse(Status::CODE_500, response);
        }
        
        auto result = alarm_service_->unsubscribeAlarm(dto->device_id);
        return createDtoResponse(Status::CODE_200, result);
    }

    /**
     * 查询报警列表
     */
    ENDPOINT_INFO(queryAlarms) {
        info->summary = "Query alarm list";
        info->description = "Query device alarm records";
        info->addConsumes<Object<AlarmQueryDto>>("application/json");
        info->addResponse<Object<AlarmListDto>>(Status::CODE_200, "application/json");
        info->addTag("Alarm Management");
    }
    ENDPOINT("POST", "/api/v1/alarm/query", queryAlarms,
             BODY_DTO(Object<AlarmQueryDto>, dto))
    {
        if (!alarm_service_) {
            auto response = AlarmListDto::createShared();
            response->code = 500;
            response->status = "ERROR";
            response->message = "AlarmService not initialized";
            return createDtoResponse(Status::CODE_500, response);
        }
        
        auto result = alarm_service_->queryAlarms(dto);
        return createDtoResponse(Status::CODE_200, result);
    }

    /**
     * 获取报警列表（分页）
     */
    ENDPOINT_INFO(getAlarmList) {
        info->summary = "Get alarm list";
        info->description = "Get device alarm list with pagination";
        info->addResponse<Object<AlarmListDto>>(Status::CODE_200, "application/json");
        info->addTag("Alarm Management");
    }
    ENDPOINT("GET", "/api/v1/alarm/list", getAlarmList,
             QUERY(String, device_id, "device_id"),
             QUERY(Int32, offset, "offset", "0"),
             QUERY(Int32, limit, "limit", "20"))
    {
        if (!alarm_service_) {
            auto response = AlarmListDto::createShared();
            response->code = 500;
            response->status = "ERROR";
            response->message = "AlarmService not initialized";
            return createDtoResponse(Status::CODE_500, response);
        }
        
        auto result = alarm_service_->getAlarmList(device_id, offset, limit);
        return createDtoResponse(Status::CODE_200, result);
    }

    /**
     * 获取报警详情
     */
    ENDPOINT_INFO(getAlarmById) {
        info->summary = "Get alarm detail";
        info->description = "Get alarm detail by ID";
        info->addResponse<Object<AlarmItemDto>>(Status::CODE_200, "application/json");
        info->addTag("Alarm Management");
    }
    ENDPOINT("GET", "/api/v1/alarm/{id}", getAlarmById,
             PATH(Int32, id))
    {
        if (!alarm_service_) {
            auto response = AlarmItemDto::createShared();
            return createDtoResponse(Status::CODE_500, response);
        }
        
        auto result = alarm_service_->getAlarmById(id);
        if (!result) {
            return createResponse(Status::CODE_404, "Alarm not found");
        }
        
        return createDtoResponse(Status::CODE_200, result);
    }

    /**
     * 确认报警
     */
    ENDPOINT_INFO(confirmAlarm) {
        info->summary = "Confirm alarm";
        info->description = "Confirm alarm has been handled";
        info->addConsumes<Object<AlarmConfirmDto>>("application/json");
        info->addResponse<Object<AlarmResponseDto>>(Status::CODE_200, "application/json");
        info->addTag("Alarm Management");
    }
    ENDPOINT("POST", "/api/v1/alarm/confirm", confirmAlarm,
             BODY_DTO(Object<AlarmConfirmDto>, dto))
    {
        if (!alarm_service_) {
            auto response = AlarmResponseDto::createShared();
            response->code = 500;
            response->status = "ERROR";
            response->message = "AlarmService not initialized";
            return createDtoResponse(Status::CODE_500, response);
        }
        
        auto result = alarm_service_->confirmAlarm(dto);
        return createDtoResponse(Status::CODE_200, result);
    }

    /**
     * 复位报警
     */
    ENDPOINT_INFO(resetAlarm) {
        info->summary = "Reset alarm";
        info->description = "Reset alarm to normal state";
        info->addConsumes<Object<AlarmResetDto>>("application/json");
        info->addResponse<Object<AlarmResponseDto>>(Status::CODE_200, "application/json");
        info->addTag("Alarm Management");
    }
    ENDPOINT("POST", "/api/v1/alarm/reset", resetAlarm,
             BODY_DTO(Object<AlarmResetDto>, dto))
    {
        if (!alarm_service_) {
            auto response = AlarmResponseDto::createShared();
            response->code = 500;
            response->status = "ERROR";
            response->message = "AlarmService not initialized";
            return createDtoResponse(Status::CODE_500, response);
        }
        
        auto result = alarm_service_->resetAlarm(dto);
        return createDtoResponse(Status::CODE_200, result);
    }

private:
    std::shared_ptr<AlarmService> alarm_service_;
};

} // namespace gbsip_server

#include OATPP_CODEGEN_END(ApiController)

#endif // GBSIPSERVER_CONTROLLER_ALARMCONTROLLER_HPP
