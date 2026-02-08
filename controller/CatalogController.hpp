/*
*  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source tree.
*/
/*****************************************************************************
				  Author: chensong
				  date:  2026-02-09
				  
				  目录管理控制器
******************************************************************************/

#ifndef _C_CATALOG_CONTROLLER_HPP
#define _C_CATALOG_CONTROLLER_HPP

#include "service/CatalogService.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/web/mime/ContentMappers.hpp"
#include "oatpp/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

/**
 * 目录管理REST控制器
 * 提供设备目录查询、订阅等HTTP API接口
 */
class CatalogController : public oatpp::web::server::api::ApiController {
public:
    CatalogController(OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers))
        : oatpp::web::server::api::ApiController(apiContentMappers)
    {}
    
private:
    CatalogService m_catalogService;
    
public:
    static std::shared_ptr<CatalogController> createShared(
        OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers)
    ) {
        return std::make_shared<CatalogController>(apiContentMappers);
    }
    
    ENDPOINT_INFO(queryCatalog) {
        info->summary = "Query device catalog";
        info->description = "Query all channels of a device";
        
        info->addResponse<Object<CatalogResponseDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");
        
        info->pathParams["deviceId"].description = "Device ID";
    }
    ENDPOINT("GET", "/api/v1/device/{deviceId}/catalog", queryCatalog,
             PATH(String, deviceId))
    {
        auto result = m_catalogService.queryCatalog(deviceId);
        if (result) {
            return createDtoResponse(Status::CODE_200, result);
        } else {
            auto status = StatusDto::createShared();
            status->code = 404;
            status->status = "ERROR";
            status->message = "Device not found";
            return createDtoResponse(Status::CODE_404, status);
        }
    }
    
    ENDPOINT_INFO(getChannelsByDeviceId) {
        info->summary = "Get device channels";
        info->description = "Get all channels of a device from database";
        
        info->addResponse<Vector<Object<ChannelDto>>>(Status::CODE_200, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
        
        info->pathParams["deviceId"].description = "Device ID";
    }
    ENDPOINT("GET", "/api/v1/device/{deviceId}/channels", getChannelsByDeviceId,
             PATH(String, deviceId))
    {
        auto channels = m_catalogService.getChannelsByDeviceId(deviceId);
        return createDtoResponse(Status::CODE_200, channels);
    }
    
    ENDPOINT_INFO(getChannelById) {
        info->summary = "Get channel by ID";
        info->description = "Get single channel information";
        
        info->addResponse<Object<ChannelDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
        
        info->pathParams["channelId"].description = "Channel ID";
    }
    ENDPOINT("GET", "/api/v1/channel/{channelId}", getChannelById,
             PATH(String, channelId))
    {
        auto channel = m_catalogService.getChannelById(channelId);
        if (channel) {
            return createDtoResponse(Status::CODE_200, channel);
        } else {
            auto status = StatusDto::createShared();
            status->code = 404;
            status->status = "ERROR";
            status->message = "Channel not found";
            return createDtoResponse(Status::CODE_404, status);
        }
    }
    
    ENDPOINT_INFO(updateChannel) {
        info->summary = "Update channel";
        info->description = "Update channel information";
        
        info->addConsumes<Object<ChannelDto>>("application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");
        
        info->pathParams["channelId"].description = "Channel ID";
    }
    ENDPOINT("PUT", "/api/v1/channel/{channelId}", updateChannel,
             PATH(String, channelId),
             BODY_DTO(Object<ChannelDto>, channelDto))
    {
        channelDto->channelId = channelId;
        return createDtoResponse(Status::CODE_200, m_catalogService.updateChannel(channelDto));
    }
    
    ENDPOINT_INFO(deleteChannel) {
        info->summary = "Delete channel";
        info->description = "Delete channel by ID";
        
        info->addResponse<Object<StatusDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");
        
        info->pathParams["channelId"].description = "Channel ID";
    }
    ENDPOINT("DELETE", "/api/v1/channel/{channelId}", deleteChannel,
             PATH(String, channelId))
    {
        return createDtoResponse(Status::CODE_200, m_catalogService.deleteChannel(channelId));
    }
    
    ENDPOINT_INFO(subscribeCatalog) {
        info->summary = "Subscribe device catalog";
        info->description = "Subscribe to device catalog changes";
        
        info->addConsumes<Object<CatalogSubscribeDto>>("application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");
    }
    ENDPOINT("POST", "/api/v1/catalog/subscribe", subscribeCatalog,
             BODY_DTO(Object<CatalogSubscribeDto>, subscribeDto))
    {
        return createDtoResponse(Status::CODE_200, m_catalogService.subscribeCatalog(subscribeDto));
    }
    
    ENDPOINT_INFO(unsubscribeCatalog) {
        info->summary = "Unsubscribe device catalog";
        info->description = "Unsubscribe from device catalog changes";
        
        info->addResponse<Object<StatusDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
        
        info->pathParams["deviceId"].description = "Device ID";
    }
    ENDPOINT("POST", "/api/v1/catalog/unsubscribe/{deviceId}", unsubscribeCatalog,
             PATH(String, deviceId))
    {
        return createDtoResponse(Status::CODE_200, m_catalogService.unsubscribeCatalog(deviceId));
    }
};

#include OATPP_CODEGEN_END(ApiController)

#endif /* _C_CATALOG_CONTROLLER_HPP */
