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
				  date:  2025-10-17



******************************************************************************/
#ifndef _C_STREAM_Controller_hpp
#define _C_STREAM_Controller_hpp

#include "service/UserService.hpp"
#include "service/DeviceService.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/web/mime/ContentMappers.hpp"
#include "oatpp/macro/codegen.hpp"
#include "sip_server.h"
#include "service/StreamService.hpp"

//extern std::shared_ptr<gbsip_server::SipServer>  sip_server;;

#include OATPP_CODEGEN_BEGIN(ApiController) //<- Begin Codegen

/**
 * Device REST controller.
 */
class StreamController : public oatpp::web::server::api::ApiController {
public:
	StreamController(OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers))
    : oatpp::web::server::api::ApiController(apiContentMappers)
  {}
private:
	StreamService m_streamService; // Create stream service.
public:

  static std::shared_ptr<StreamController> createShared(
    OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers) // Inject ContentMappers
  ){
    return std::make_shared<StreamController>(apiContentMappers);
  }
  
  
  
  
	ENDPOINT_INFO(startStream) {
	info->summary = "Start Stream";
	  

	info->addConsumes<Object<StreamDto>>("application/json");

	info->addResponse<Object<StreamDto>>(Status::CODE_200, "application/json");
	info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
	info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");
  }
  ENDPOINT("POST", "/api/v1/stream/start", startStream,
	  BODY_DTO(Object<StreamDto>, streamDto))
  {
	  return createDtoResponse(Status::CODE_200, m_streamService.startStream(streamDto));
  }

  ENDPOINT_INFO(stopStream) {
	  info->summary = "Stop Stream";

	  info->addConsumes<Object<StreamDto>>("application/json");

	  info->addResponse<Object<StreamDto>>(Status::CODE_200, "application/json");
	  info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
	  info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");
  }
  ENDPOINT("POST", "/api/v1/stream/stop", stopStream,
	  BODY_DTO(Object<StreamDto>, streamDto))
  {
	  return createDtoResponse(Status::CODE_200, m_streamService.stopStream(streamDto));
  }
   

};

#include OATPP_CODEGEN_END(ApiController) //<- End Codegen

#endif /* UserController_hpp */
