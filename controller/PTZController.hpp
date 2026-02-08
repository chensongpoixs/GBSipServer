/*
*  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source tree.
*/
/*****************************************************************************
				  Author: chensong
				  date:  2026-02-09
				  
				  PTZ控制REST控制器
******************************************************************************/

#ifndef _C_PTZ_CONTROLLER_HPP
#define _C_PTZ_CONTROLLER_HPP

#include "service/PTZService.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/web/mime/ContentMappers.hpp"
#include "oatpp/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

/**
 * PTZ控制REST控制器
 * 提供PTZ控制的HTTP API接口
 */
class PTZController : public oatpp::web::server::api::ApiController {
public:
    PTZController(OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers))
        : oatpp::web::server::api::ApiController(apiContentMappers)
    {}
    
private:
    PTZService m_ptzService;
    
public:
    static std::shared_ptr<PTZController> createShared(
        OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers)
    ) {
        return std::make_shared<PTZController>(apiContentMappers);
    }
    
    /**
     * 设置PTZHandler
     */
    void setPTZHandler(std::shared_ptr<gbsip_server::PTZHandler> handler) {
        m_ptzService.setPTZHandler(handler);
    }
    
    ENDPOINT_INFO(controlDirection) {
        info->summary = "PTZ direction control";
        info->description = "Control PTZ direction (UP/DOWN/LEFT/RIGHT/etc)";
        
        info->addResponse<Object<PTZResponseDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<PTZResponseDto>>(Status::CODE_400, "application/json");
        info->addResponse<Object<PTZResponseDto>>(Status::CODE_404, "application/json");
        info->addResponse<Object<PTZResponseDto>>(Status::CODE_500, "application/json");
    }
    ENDPOINT("POST", "/api/v1/ptz/control", controlDirection,
             BODY_DTO(Object<PTZControlDto>, dto))
    {
        auto result = m_ptzService.controlDirection(dto);
        auto statusCode = (result->code == 200) ? Status::CODE_200 :
                          (result->code == 400) ? Status::CODE_400 :
                          (result->code == 404) ? Status::CODE_404 : Status::CODE_500;
        return createDtoResponse(statusCode, result);
    }
    
    ENDPOINT_INFO(controlZoom) {
        info->summary = "PTZ zoom control";
        info->description = "Control PTZ zoom (zoom in/out)";
        
        info->addResponse<Object<PTZResponseDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<PTZResponseDto>>(Status::CODE_400, "application/json");
        info->addResponse<Object<PTZResponseDto>>(Status::CODE_500, "application/json");
    }
    ENDPOINT("POST", "/api/v1/ptz/zoom", controlZoom,
             BODY_DTO(Object<PTZZoomDto>, dto))
    {
        auto result = m_ptzService.controlZoom(dto);
        auto statusCode = (result->code == 200) ? Status::CODE_200 :
                          (result->code == 400) ? Status::CODE_400 : Status::CODE_500;
        return createDtoResponse(statusCode, result);
    }
    
    ENDPOINT_INFO(controlFocus) {
        info->summary = "PTZ focus control";
        info->description = "Control PTZ focus (near/far)";
        
        info->addResponse<Object<PTZResponseDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<PTZResponseDto>>(Status::CODE_400, "application/json");
        info->addResponse<Object<PTZResponseDto>>(Status::CODE_500, "application/json");
    }
    ENDPOINT("POST", "/api/v1/ptz/focus", controlFocus,
             BODY_DTO(Object<PTZFocusDto>, dto))
    {
        auto result = m_ptzService.controlFocus(dto);
        auto statusCode = (result->code == 200) ? Status::CODE_200 :
                          (result->code == 400) ? Status::CODE_400 : Status::CODE_500;
        return createDtoResponse(statusCode, result);
    }
    
    ENDPOINT_INFO(controlIris) {
        info->summary = "PTZ iris control";
        info->description = "Control PTZ iris (open/close)";
        
        info->addResponse<Object<PTZResponseDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<PTZResponseDto>>(Status::CODE_400, "application/json");
        info->addResponse<Object<PTZResponseDto>>(Status::CODE_500, "application/json");
    }
    ENDPOINT("POST", "/api/v1/ptz/iris", controlIris,
             BODY_DTO(Object<PTZIrisDto>, dto))
    {
        auto result = m_ptzService.controlIris(dto);
        auto statusCode = (result->code == 200) ? Status::CODE_200 :
                          (result->code == 400) ? Status::CODE_400 : Status::CODE_500;
        return createDtoResponse(statusCode, result);
    }
    
    ENDPOINT_INFO(setPreset) {
        info->summary = "Set PTZ preset";
        info->description = "Set a PTZ preset position";
        
        info->addResponse<Object<PTZResponseDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<PTZResponseDto>>(Status::CODE_400, "application/json");
        info->addResponse<Object<PTZResponseDto>>(Status::CODE_500, "application/json");
    }
    ENDPOINT("POST", "/api/v1/ptz/preset/set", setPreset,
             BODY_DTO(Object<PTZPresetDto>, dto))
    {
        auto result = m_ptzService.setPreset(dto);
        auto statusCode = (result->code == 200) ? Status::CODE_200 :
                          (result->code == 400) ? Status::CODE_400 : Status::CODE_500;
        return createDtoResponse(statusCode, result);
    }
    
    ENDPOINT_INFO(callPreset) {
        info->summary = "Call PTZ preset";
        info->description = "Call a PTZ preset position";
        
        info->addResponse<Object<PTZResponseDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<PTZResponseDto>>(Status::CODE_400, "application/json");
        info->addResponse<Object<PTZResponseDto>>(Status::CODE_500, "application/json");
    }
    ENDPOINT("POST", "/api/v1/ptz/preset/call", callPreset,
             BODY_DTO(Object<PTZPresetDto>, dto))
    {
        auto result = m_ptzService.callPreset(dto);
        auto statusCode = (result->code == 200) ? Status::CODE_200 :
                          (result->code == 400) ? Status::CODE_400 : Status::CODE_500;
        return createDtoResponse(statusCode, result);
    }
    
    ENDPOINT_INFO(deletePreset) {
        info->summary = "Delete PTZ preset";
        info->description = "Delete a PTZ preset position";
        
        info->addResponse<Object<PTZResponseDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<PTZResponseDto>>(Status::CODE_400, "application/json");
        info->addResponse<Object<PTZResponseDto>>(Status::CODE_500, "application/json");
    }
    ENDPOINT("POST", "/api/v1/ptz/preset/delete", deletePreset,
             BODY_DTO(Object<PTZPresetDto>, dto))
    {
        auto result = m_ptzService.deletePreset(dto);
        auto statusCode = (result->code == 200) ? Status::CODE_200 :
                          (result->code == 400) ? Status::CODE_400 : Status::CODE_500;
        return createDtoResponse(statusCode, result);
    }
    
    ENDPOINT_INFO(startCruise) {
        info->summary = "Start PTZ cruise";
        info->description = "Start PTZ cruise patrol";
        
        info->addResponse<Object<PTZResponseDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<PTZResponseDto>>(Status::CODE_400, "application/json");
        info->addResponse<Object<PTZResponseDto>>(Status::CODE_500, "application/json");
    }
    ENDPOINT("POST", "/api/v1/ptz/cruise/start", startCruise,
             BODY_DTO(Object<PTZCruiseDto>, dto))
    {
        auto result = m_ptzService.startCruise(dto);
        auto statusCode = (result->code == 200) ? Status::CODE_200 :
                          (result->code == 400) ? Status::CODE_400 : Status::CODE_500;
        return createDtoResponse(statusCode, result);
    }
    
    ENDPOINT_INFO(stopCruise) {
        info->summary = "Stop PTZ cruise";
        info->description = "Stop PTZ cruise patrol";
        
        info->addResponse<Object<PTZResponseDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<PTZResponseDto>>(Status::CODE_400, "application/json");
        info->addResponse<Object<PTZResponseDto>>(Status::CODE_500, "application/json");
    }
    ENDPOINT("POST", "/api/v1/ptz/cruise/stop", stopCruise,
             BODY_DTO(Object<PTZCruiseDto>, dto))
    {
        auto result = m_ptzService.stopCruise(dto);
        auto statusCode = (result->code == 200) ? Status::CODE_200 :
                          (result->code == 400) ? Status::CODE_400 : Status::CODE_500;
        return createDtoResponse(statusCode, result);
    }
    
    ENDPOINT_INFO(setGuard) {
        info->summary = "Set PTZ guard position";
        info->description = "Set PTZ guard position (home position)";
        
        info->addResponse<Object<PTZResponseDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<PTZResponseDto>>(Status::CODE_400, "application/json");
        info->addResponse<Object<PTZResponseDto>>(Status::CODE_500, "application/json");
    }
    ENDPOINT("POST", "/api/v1/ptz/guard", setGuard,
             BODY_DTO(Object<PTZGuardDto>, dto))
    {
        auto result = m_ptzService.setGuard(dto);
        auto statusCode = (result->code == 200) ? Status::CODE_200 :
                          (result->code == 400) ? Status::CODE_400 : Status::CODE_500;
        return createDtoResponse(statusCode, result);
    }
};

#include OATPP_CODEGEN_END(ApiController)

#endif /* _C_PTZ_CONTROLLER_HPP */
