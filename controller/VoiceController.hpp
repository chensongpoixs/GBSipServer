/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Author: chensong
 *  Date: 2026-02-09
 *  Description: 语音对讲REST控制器
 ******************************************************************************/

#ifndef GBSIPSERVER_CONTROLLER_VOICECONTROLLER_HPP
#define GBSIPSERVER_CONTROLLER_VOICECONTROLLER_HPP

#include "service/VoiceService.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

namespace gbsip_server {

/**
 * 语音对讲REST控制器
 * 提供语音对讲的HTTP API接口
 */
class VoiceController : public oatpp::web::server::api::ApiController {
public:
    VoiceController(const std::shared_ptr<ObjectMapper>& objectMapper)
        : oatpp::web::server::api::ApiController(objectMapper)
    {
    }

    static std::shared_ptr<VoiceController> createShared(
        OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
    {
        return std::make_shared<VoiceController>(objectMapper);
    }

    /**
     * 设置VoiceService
     */
    void setVoiceService(std::shared_ptr<VoiceService> service) {
        voice_service_ = service;
    }

    /**
     * 开始语音广播
     */
    ENDPOINT_INFO(startBroadcast) {
        info->summary = "Start voice broadcast";
        info->description = "Start voice broadcast to device";
        info->addConsumes<Object<VoiceBroadcastStartDto>>("application/json");
        info->addResponse<Object<VoiceResponseDto>>(Status::CODE_200, "application/json");
        info->addTag("Voice Management");
    }
    ENDPOINT("POST", "/api/v1/voice/broadcast/start", startBroadcast,
             BODY_DTO(Object<VoiceBroadcastStartDto>, dto))
    {
        if (!voice_service_) {
            auto response = VoiceResponseDto::createShared();
            response->code = 500;
            response->status = "ERROR";
            response->message = "VoiceService not initialized";
            return createDtoResponse(Status::CODE_500, response);
        }
        
        auto result = voice_service_->startBroadcast(dto);
        return createDtoResponse(Status::CODE_200, result);
    }

    /**
     * 停止语音广播
     */
    ENDPOINT_INFO(stopBroadcast) {
        info->summary = "Stop voice broadcast";
        info->description = "Stop voice broadcast";
        info->addConsumes<Object<VoiceControlDto>>("application/json");
        info->addResponse<Object<VoiceResponseDto>>(Status::CODE_200, "application/json");
        info->addTag("Voice Management");
    }
    ENDPOINT("POST", "/api/v1/voice/broadcast/stop", stopBroadcast,
             BODY_DTO(Object<VoiceControlDto>, dto))
    {
        if (!voice_service_) {
            auto response = VoiceResponseDto::createShared();
            response->code = 500;
            response->status = "ERROR";
            response->message = "VoiceService not initialized";
            return createDtoResponse(Status::CODE_500, response);
        }
        
        auto result = voice_service_->stopBroadcast(dto);
        return createDtoResponse(Status::CODE_200, result);
    }

    /**
     * 开始双向对讲
     */
    ENDPOINT_INFO(startTalk) {
        info->summary = "Start two-way talk";
        info->description = "Start two-way talk with device";
        info->addConsumes<Object<VoiceTalkStartDto>>("application/json");
        info->addResponse<Object<VoiceResponseDto>>(Status::CODE_200, "application/json");
        info->addTag("Voice Management");
    }
    ENDPOINT("POST", "/api/v1/voice/talk/start", startTalk,
             BODY_DTO(Object<VoiceTalkStartDto>, dto))
    {
        if (!voice_service_) {
            auto response = VoiceResponseDto::createShared();
            response->code = 500;
            response->status = "ERROR";
            response->message = "VoiceService not initialized";
            return createDtoResponse(Status::CODE_500, response);
        }
        
        auto result = voice_service_->startTalk(dto);
        return createDtoResponse(Status::CODE_200, result);
    }

    /**
     * 停止双向对讲
     */
    ENDPOINT_INFO(stopTalk) {
        info->summary = "Stop two-way talk";
        info->description = "Stop two-way talk";
        info->addConsumes<Object<VoiceControlDto>>("application/json");
        info->addResponse<Object<VoiceResponseDto>>(Status::CODE_200, "application/json");
        info->addTag("Voice Management");
    }
    ENDPOINT("POST", "/api/v1/voice/talk/stop", stopTalk,
             BODY_DTO(Object<VoiceControlDto>, dto))
    {
        if (!voice_service_) {
            auto response = VoiceResponseDto::createShared();
            response->code = 500;
            response->status = "ERROR";
            response->message = "VoiceService not initialized";
            return createDtoResponse(Status::CODE_500, response);
        }
        
        auto result = voice_service_->stopTalk(dto);
        return createDtoResponse(Status::CODE_200, result);
    }

private:
    std::shared_ptr<VoiceService> voice_service_;
};

} // namespace gbsip_server

#include OATPP_CODEGEN_END(ApiController)

#endif // GBSIPSERVER_CONTROLLER_VOICECONTROLLER_HPP
