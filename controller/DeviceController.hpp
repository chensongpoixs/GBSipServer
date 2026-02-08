/*
*  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
*
*  Please visit https://chensongpoixs.github.io for detail
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source
*  tree. An additional intellectual property rights grant can be found
*  in the file PATENTS.  All contributing project authors may
*  be found in the AUTHORS file in the root of the source tree.
*/
/*****************************************************************************
				  Author: chensong
				  date:  2025-10-12



******************************************************************************/
#ifndef _C_DEVICE_Controller_hpp
#define _C_DEVICE_Controller_hpp

#include "service/UserService.hpp"
#include "service/DeviceService.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/web/mime/ContentMappers.hpp"
#include "oatpp/macro/codegen.hpp"
#include "sip_server.h"

//extern std::shared_ptr<gbsip_server::SipServer>  sip_server;;

#include OATPP_CODEGEN_BEGIN(ApiController) //<- Begin Codegen

/**
 * Device REST controller.
 */
class DeviceController : public oatpp::web::server::api::ApiController {
public:
	DeviceController(OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers))
    : oatpp::web::server::api::ApiController(apiContentMappers)
  {}
private:
  DeviceService m_deviceService; // Create device service.
public:

  static std::shared_ptr<DeviceController> createShared(
    OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers) // Inject ContentMappers
  ){
    return std::make_shared<DeviceController>(apiContentMappers);
  }
  
  ENDPOINT_INFO(createDevice) {
    info->summary = "Create new Device";

    info->addConsumes<Object<DeviceDto>>("application/json");

    info->addResponse<Object<DeviceDto>>(Status::CODE_200, "application/json");
    info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
    info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");
  }
  ENDPOINT("POST", "devices", createDevice,
           BODY_DTO(Object<DeviceDto>, deviceDto))
  {
    return createDtoResponse(Status::CODE_200, m_deviceService.createDevice(deviceDto));
  }
  
  
  ENDPOINT_INFO(putDevice) {
    info->summary = "Update Device by deviceId";

    info->addConsumes<Object<DeviceDto>>("application/json");

    info->addResponse<Object<DeviceDto>>(Status::CODE_200, "application/json");
    info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
    info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");

    info->pathParams["id"].description = "Device id Identifier";
  }
  ENDPOINT("PUT", "device/{id}", putDevice,
           PATH(Int32, id),
           BODY_DTO(Object<DeviceDto>, deviceDto))
  {
	  deviceDto->id = id;
    return createDtoResponse(Status::CODE_200, m_deviceService.updateDevice(deviceDto));
  }
  
  
  ENDPOINT_INFO(getDeviceById) {
    info->summary = "Get one Device by userId";

    info->addResponse<Object<DeviceDto>>(Status::CODE_200, "application/json");
    info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
    info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");

    info->pathParams["id"].description = "Device  id Identifier";
  }
  ENDPOINT("GET", "devices/{id}", getDeviceById,
           PATH(Int32, id))
  {
	//  printf("[%s][%d] start \n", __FUNCTION__, __LINE__);
	//  std::this_thread::sleep_for(std::chrono::seconds(120));
	//  printf("[%s][%d] end \n", __FUNCTION__, __LINE__);
	//  sip_server->request_invite();
    return createDtoResponse(Status::CODE_200, m_deviceService.getDeviceById(id));
  }
  
  
  ENDPOINT_INFO(getDevices) {
    info->summary = "get all stored devices";

    info->addResponse<oatpp::Object<DevicesPageDto>>(Status::CODE_200, "application/json");
    info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");
  }
  ENDPOINT("GET", "devices/offset/{offset}/limit/{limit}", getDevices,
           PATH(UInt32, offset),
           PATH(UInt32, limit))
  {
    return createDtoResponse(Status::CODE_200, m_deviceService.getAllDevices(offset, limit));
  }
   


  
  ENDPOINT_INFO(deleteDevice) {
    info->summary = "Delete Device by deviceId";

    info->addResponse<Object<StatusDto>>(Status::CODE_200, "application/json");
    info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");

    info->pathParams["deviceId"].description = "Device Identifier";
  }
  ENDPOINT("DELETE", "devices/{id}", deleteDevice,
           PATH(Int32, id))
  {
    return createDtoResponse(Status::CODE_200, m_deviceService.deleteDeviceById(id));
  }

};

#include OATPP_CODEGEN_END(ApiController) //<- End Codegen

#endif /* UserController_hpp */
