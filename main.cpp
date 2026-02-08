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
				   date:  2025-09-17



 ******************************************************************************/

//#include "libmedia_transfer_protocol/libsip/udp_sip_server.h"
//#include "libmedia_transfer_protocol/libsip/sip_server.h"
#include <thread>
#include <chrono>
#include "sip_server.h"
 
//#pragma comment(lib, "mil.lib")


#include "AppComponent.hpp"

#include "controller/UserController.hpp"
#include "controller/DeviceController.hpp"
#include "controller/StaticController.hpp"
#include "controller/DeviceInfoController.hpp"
#include "controller/PTZController.hpp"
#include "controller/RecordController.hpp"
#include "oatpp/web/server/interceptor/AllowCorsGlobal.hpp"
#include"controller/StreamController.hpp"
#include "oatpp-swagger/Controller.hpp"

#include "oatpp/network/Server.hpp"

#include <iostream>
#include "rtc_base/logging.h"
#include "utils/yaml_config.h"

#include "gbsip_server_log.h"



//std::shared_ptr<gbsip_server::SipServer> sip_server;
void run() {

	
	AppComponent components; // Create scope Environment components
	 
	/* Get router component */
	OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);

	oatpp::web::server::api::Endpoints docEndpoints;

	docEndpoints.append(router->addController(UserController::createShared())->getEndpoints());
//	oatpp::web::server::api::Endpoints DevicedocEndpoints;
	docEndpoints.append(router->addController(DeviceController::createShared())->getEndpoints());
	docEndpoints.append(router->addController(StreamController::createShared())->getEndpoints());
	docEndpoints.append(router->addController(DeviceInfoController::createShared())->getEndpoints());
	
	// 注册PTZController并设置Handler
	auto ptzController = PTZController::createShared();
	ptzController->setPTZHandler(gbsip_server::SipServer::GetInstance().getPTZHandler());
	docEndpoints.append(router->addController(ptzController)->getEndpoints());
	
	// 注册RecordController并设置RecordService
	auto recordController = RecordController::createShared();
	auto recordService = std::make_shared<gbsip_server::RecordService>(
		gbsip_server::SipServer::GetInstance().getRecordHandler()
	);
	recordController->setRecordService(recordService);
	docEndpoints.append(router->addController(recordController)->getEndpoints());
	
	router->addController(oatpp::swagger::Controller::createShared(docEndpoints));
	//router->addController(oatpp::swagger::Controller::createShared(docEndpoints));
	router->addController(StaticController::createShared());
	
	/* Get connection handler component */
	OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, connectionHandler);

	/* Create a CORSInterceptor instance */
	auto corsInterceptor = std::make_shared<oatpp::web::server::interceptor::AllowCorsGlobal>();
	auto AllowOptionsGlobal_ = std::make_shared<oatpp::web::server::interceptor::AllowOptionsGlobal>();

	/* Add CORSInterceptor to the connection handler */
	auto httpConnectionHandler = std::static_pointer_cast<oatpp::web::server::HttpConnectionHandler>(connectionHandler);
	httpConnectionHandler->addRequestInterceptor(AllowOptionsGlobal_);
	httpConnectionHandler->addResponseInterceptor(corsInterceptor);
	
	/* Get connection provider component */
	OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, connectionProvider);
	//connectionProvider->
	/* create server */
	oatpp::network::Server server(connectionProvider,
		connectionHandler);
	 
	gbsip_server::SipServer::GetInstance().init(
		gbsip_server::YamlConfig::GetInstance().GetSipServerConfig());
	gbsip_server::SipServer::GetInstance().Start();
	//connectionProvider->
	
	///OATPP_LOGd("Server", "Running on port {}...", connectionProvider->getProperty("port").toString())
	SIPSERVER_LOG(LS_INFO) << "Web Server  run port:" 
		<< connectionProvider->getProperty("port").std_str() ;
		server.run();

	/* stop db connection pool */
	OATPP_COMPONENT(std::shared_ptr<oatpp::provider::Provider<oatpp::sqlite::Connection>>, dbConnectionProvider);
	dbConnectionProvider->stop();

}


int main(int argc, char *argv[])
{
#ifdef WIN32
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{

		return -1;
	}
#endif // WIN32

	const char* config_file = "gbsip_server.yaml";


	bool init = gbsip_server::YamlConfig::GetInstance().LoadFile(config_file);
	if (!init)
	{
		return -1;
	}

	oatpp::Environment::init();
	
	run();

	/* Print how many objects were created during app running, and what have left-probably leaked */
	/* Disable object counting for release builds using '-D OATPP_DISABLE_ENV_OBJECT_COUNTERS' flag for better performance */
	std::cout << "\nEnvironment:\n";
	std::cout << "objectsCount = " << oatpp::Environment::getObjectsCount() << "\n";
	std::cout << "objectsCreated = " << oatpp::Environment::getObjectsCreated() << "\n\n";

	oatpp::Environment::destroy();
	
	

	return 0;
}