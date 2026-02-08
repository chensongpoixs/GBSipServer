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
#include "service/StreamService.hpp" 
#include "sip_server.h"
#include "utils/chttplib.h"
//#include "json.hpp"
//#include "json.hpp"
#include "json.hpp"
//static const char *  http_media_api = "http://192.168.9.174:8001";
//static const uint16_t media_port = 8001;
#include "gbsip_server_log.h"
oatpp::Object<StreamDto> StreamService::startStream(const oatpp::Object<StreamDto>& dto)
{


	auto dbResult = m_database->getDeviceByDeviceId(dto->deviceID);
	OATPP_ASSERT_HTTP(dbResult->isSuccess(), Status::CODE_500, dbResult->getErrorMessage());

	auto items = dbResult->fetch<oatpp::Vector<oatpp::Object<DeviceDto>>>();
	SIPSERVER_LOG(LS_INFO) << "items size:" << items->size();
	OATPP_ASSERT_HTTP(items->size() == 1, Status::CODE_404, "Device not found");
	 

	// rquest  --> GbMediaServer open Rtp Server --> 
	/// workthread-> 
	return gbsip_server::SipServer::GetInstance().worker_thread()->Invoke<oatpp::Object<StreamDto>>(RTC_FROM_HERE, [&]() {
	
		auto streamdto = StreamDto::createShared();
		//if (dto->)
	
		nlohmann::json    request;
		request["port"] = 0;
		request["tcpmode"] = 1;
		request["streamid"] = dto->deviceID;
		streamdto->deviceID = dto->deviceID;
		nlohmann::json response;
		std::string result;
		static std::string http_media_api = "http://" + gbsip_server::YamlConfig::GetInstance().GetMediaConfig().ip
			+ ":" + std::to_string(gbsip_server::YamlConfig::GetInstance().GetMediaConfig().port);
		bool ret = httplib::PostReuest(http_media_api.c_str(), "/api/openRtpServer", request.dump().c_str(), result);
		//
		if (!ret)
		{
			SIPSERVER_LOG(LS_WARNING) << "Request media api failed !!! open rtp server !!!";
			return streamdto;
		}
		//OATPP_ASSERT_HTTP(ret == true, Status::CODE_404, " rquest Media open Rtp failed !!! ");
		//streamdto->deviceID = dto->deviceID;
		try
			{
				response = nlohmann::json::parse(result);
			}
			catch (const std::exception&)
			{
				//printf("websocket protoo [msg = %s] json parse failed !!!", result.c_str());
				SIPSERVER_LOG(LS_WARNING) << "websocket protoo [msg = "<< result <<"] json parse failed !!!";
				return streamdto;
			}
			
			if (response.find("port") != response.end())
			{
				uint16_t tcp_port = response["port"].get<uint16_t>();
				 
				/// network--> post  sip invtie

				gbsip_server::SipServer::GetInstance().network_thread()->Invoke<void>(RTC_FROM_HERE, [&]() {
				

					gbsip_server::SipServer::GetInstance().request_invite(dto->deviceID, 
						items[0]->RemoteIP, items[0]->RemotePort, tcp_port);
		   
				});



				return streamdto;
			}

		return streamdto;
	});
 
	//return oatpp::Object<StreamDto>();
}

oatpp::Object<StreamDto> StreamService::stopStream(const oatpp::Object<StreamDto>& dto)
{
	auto dbResult = m_database->getDeviceByDeviceId(dto->deviceID);
	OATPP_ASSERT_HTTP(dbResult->isSuccess(), Status::CODE_500, dbResult->getErrorMessage());

	auto items = dbResult->fetch<oatpp::Vector<oatpp::Object<DeviceDto>>>();
	OATPP_ASSERT_HTTP(items->size() == 1, Status::CODE_404, "Device not found");


	// rquest  --> GbMediaServer open Rtp Server --> 
	/// workthread-> 
	return gbsip_server::SipServer::GetInstance().worker_thread()->Invoke<oatpp::Object<StreamDto>>(RTC_FROM_HERE, [&]() {

		auto streamdto = StreamDto::createShared();
		//if (dto->)

		nlohmann::json    request;
		request["port"] = 0;
		request["tcpmode"] = 1;
		request["streamid"] = dto->deviceID;

		nlohmann::json response;
		std::string result;
		static std::string http_media_api = "http://" + gbsip_server::YamlConfig::GetInstance().GetMediaConfig().ip
			+ ":" + std::to_string(gbsip_server::YamlConfig::GetInstance().GetMediaConfig().port);
		bool ret = httplib::PostReuest(http_media_api.c_str(), "/api/closeRtpServer", request.dump().c_str(), result);
		// 
		if (!ret)
		{
			SIPSERVER_LOG(LS_WARNING) << "Request media api failed !!! close rtp server !!!";
			return streamdto;
		}
		try
		{
			response = nlohmann::json::parse(result);
		}
		catch (const std::exception&)
		{
			//printf("websocket protoo [msg = %s] json parse failed !!!", result.c_str());
			SIPSERVER_LOG(LS_WARNING) << "websocket protoo [msg = " << result << "] json parse failed !!!";
			return streamdto;
		}
		streamdto->deviceID = dto->deviceID;
		if (response.find("port") != response.end())
		{
			uint16_t tcp_port = response["port"].get<uint16_t>();

			/// network--> post  sip invtie

			//gbsip_server::SipServer::GetInstance().network_thread()->Invoke<void>(RTC_FROM_HERE, [&]() {
			//
			//
			//	gbsip_server::SipServer::GetInstance().request_invite(dto->deviceID,
			//		items[0]->RemoteIP, items[0]->RemotePort, tcp_port);
			//
			//});



			return streamdto;
		}

		return streamdto;
	});

	return oatpp::Object<StreamDto>();
}
