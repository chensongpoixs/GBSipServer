/*
*  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source tree.
*/
/*****************************************************************************
				  Author: chensong
				  date:  2026-02-09
				  
				  设备信息管理控制器
******************************************************************************/

#ifndef _C_DEVICE_INFO_CONTROLLER_HPP
#define _C_DEVICE_INFO_CONTROLLER_HPP

#include "service/DeviceInfoService.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/web/mime/ContentMappers.hpp"
#include "oatpp/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

/**
 * 设备信息管理REST控制器
 * 提供设备信息查询、设备状态查询等HTTP API接口
 */
class DeviceInfoController : public oatpp::web::server::api::ApiController {
public:
    DeviceInfoController(OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers))
        : oatpp::web::server::api::ApiController(apiContentMappers)
    {}
    
private:
    DeviceInfoService m_deviceInfoService;
    
public:
    static std::shared_ptr<DeviceInfoController> createShared(
        OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers)
    ) {
        return std::make_shared<DeviceInfoController>(apiContentMappers);
    }
    
    ENDPOINT_INFO(queryDeviceInfo) {
        info->summary = "Query device information";
        info->description = "Query device basic information via GB28181 SIP MESSAGE";
        
        info->addResponse<Object<DeviceInfoDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");
        
        info->pathParams["deviceId"].description = "Device ID";
    }
    ENDPOINT("GET", "/api/v1/device/{deviceId}/info", queryDeviceInfo,
             PATH(String, deviceId))
    {
        auto result = m_deviceInfoService.queryDeviceInfo(deviceId);
        if (result) {
            return createDtoResponse(Status::CODE_200, result);
        } else {
            auto status = StatusDto::createShared();
            status->code = 404;
            status->status = "ERROR";
            status->message = "Device not found or query failed";
            return createDtoResponse(Status::CODE_404, status);
        }
    }
    
    ENDPOINT_INFO(queryDeviceStatus) {
        info->summary = "Query device status";
        info->description = "Query device running status via GB28181 SIP MESSAGE";
        
        info->addResponse<Object<DeviceStatusDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");
        
        info->pathParams["deviceId"].description = "Device ID";
    }
    ENDPOINT("GET", "/api/v1/device/{deviceId}/status", queryDeviceStatus,
             PATH(String, deviceId))
    {
        auto result = m_deviceInfoService.queryDeviceStatus(deviceId);
        if (result) {
            return createDtoResponse(Status::CODE_200, result);
        } else {
            auto status = StatusDto::createShared();
            status->code = 404;
            status->status = "ERROR";
            status->message = "Device not found or query failed";
            return createDtoResponse(Status::CODE_404, status);
        }
    }
    
    ENDPOINT_INFO(queryDeviceConfig) {
        info->summary = "Query device configuration";
        info->description = "Query device configuration (Not implemented yet)";
        
        info->addResponse<Object<StatusDto>>(Status::CODE_200, "application/json");
        info->addResponse<Object<StatusDto>>(Status::CODE_501, "application/json");
        
        info->pathParams["deviceId"].description = "Device ID";
    }
    ENDPOINT("GET", "/api/v1/device/{deviceId}/config", queryDeviceConfig,
             PATH(String, deviceId))
    {
        return createDtoResponse(Status::CODE_501, 
                                 m_deviceInfoService.queryDeviceConfig(deviceId));
    }
};

#include OATPP_CODEGEN_END(ApiController)

#endif /* _C_DEVICE_INFO_CONTROLLER_HPP */
