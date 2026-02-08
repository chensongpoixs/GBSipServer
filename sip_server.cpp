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
#include "sip_server.h"


#ifndef WIN32
 // Linux系统
#include <arpa/inet.h>
#else
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif // !WIN32


#include "rtc_base/logging.h"
#include <ctime>
extern "C"
{
#include "utils/HTTPDigest.h"
}
#include "rtc_base/task_utils/to_queued_task.h"

#include <cstring>
#include "gbsip_server_log.h"




namespace gbsip_server
{

	namespace {

		int32_t parse_xml(const char *data, const char *s_mark, bool with_s_make, const char *e_mark, bool with_e_make, char *dest) {
			const char* satrt = strstr(data, s_mark);

			if (satrt != NULL) {
				const char* end = strstr(satrt, e_mark);

				if (end != NULL) {
					int s_pos = with_s_make ? 0 : strlen(s_mark);
					int e_pos = with_e_make ? strlen(e_mark) : 0;

					strncpy(dest, satrt + s_pos, (end + e_pos) - (satrt + s_pos));
				}
				return 0;
			}
			return -1;

		}

	}
	SipServer::SipServer()
		: context_(  libp2p_peerconnection::ConnectionContext::Create())
		, stoped_(true)
		, sip_server_info_()
		, sip_context_(nullptr)
	{



		sip_event_callback_map_[EXOSIP_CALL_MESSAGE_NEW] = &SipServer::HandlerSipcallMessageNew;
		sip_event_callback_map_[EXOSIP_CALL_CLOSED] = &SipServer::HandlerSipCallClosed;
		sip_event_callback_map_[EXOSIP_CALL_RELEASED] = &SipServer::HandlerSipCallReleased;
		sip_event_callback_map_[EXOSIP_MESSAGE_NEW] = &SipServer::HandlerSipMessageNew;
		sip_event_callback_map_[EXOSIP_MESSAGE_ANSWERED] = &SipServer::HandlerSipMessageAnswerd;
		sip_event_callback_map_[EXOSIP_MESSAGE_REQUESTFAILURE] = &SipServer::HandlerSipMessageRequestFailure;
		sip_event_callback_map_[EXOSIP_CALL_INVITE] = &SipServer::HandlerSipCallInvite;
		sip_event_callback_map_[EXOSIP_CALL_PROCEEDING] = &SipServer::HandlerSipCallProceeding;
		sip_event_callback_map_[EXOSIP_CALL_ANSWERED] = &SipServer::HandlerSipCallAnswered;

		sip_event_callback_map_[EXOSIP_CALL_SERVERFAILURE] = &SipServer::HandlerSipCallServerFailure;
		sip_event_callback_map_[EXOSIP_IN_SUBSCRIPTION_NEW] = &SipServer::HandlerSipInSubscriptionNew;
		//sip_event_callback_map_[EXOSIP_CALL_ANSWERED] = &SipServer::HandlerSipCallAnswered;
		if (device_db_)
		{
			SIPSERVER_LOG(LS_INFO) << "device_db_  --> ok ";
		}
		else
		{
			SIPSERVER_LOG(LS_WARNING) << "device_db_  failed";
		}
	}
	SipServer::~SipServer()
	{
		if (sip_context_)
		{
			eXosip_quit(sip_context_);
			//eXosip_free
		}
	}
	bool SipServer::init(const SipServerConfig & info)
	{
		sip_server_info_ = std::move(info);

		sip_context_ = eXosip_malloc();
		if (!sip_context_) {
			SIPSERVER_LOG(LS_WARNING) << "eXosip_malloc error";
			return -1;
		}
		if (eXosip_init(sip_context_)) {
			SIPSERVER_LOG(LS_WARNING) << "eXosip_init error";
			return -1;
		}
		SIPSERVER_LOG(LS_INFO) << "udp bind :" << sip_server_info_.port;
		if (eXosip_listen_addr(sip_context_, IPPROTO_UDP, nullptr, sip_server_info_.port, AF_INET, 0)) {
			SIPSERVER_LOG(LS_WARNING) << "eXosip_listen_addr error";
			return -1;
		}
		SIPSERVER_LOG(LS_INFO) << "udp ua :" << sip_server_info_.ua << ", server_id : " << sip_server_info_.sipServerId;
		eXosip_set_user_agent(sip_context_, sip_server_info_.ua.c_str());
		if (eXosip_add_authentication_info(sip_context_, sip_server_info_.sipServerId.c_str(), 
			sip_server_info_.sipServerId.c_str(), sip_server_info_.SipSeverPass.c_str(), NULL,
			sip_server_info_.SipServerRealm.c_str())) {
			SIPSERVER_LOG(LS_WARNING) << "eXosip_add_authentication_info error";
			return -1;
		}
		
		// 初始化DeviceInfoHandler
		device_info_handler_ = std::make_shared<gbsip_server::DeviceInfoHandler>(
			sip_context_,
			sip_server_info_.sipServerId,
			sip_server_info_.ip,
			sip_server_info_.port
		);
		
		// 初始化PTZHandler
		ptz_handler_ = std::make_shared<gbsip_server::PTZHandler>(
			sip_context_,
			sip_server_info_.sipServerId,
			sip_server_info_.ip,
			sip_server_info_.port
		);
		
		// 初始化RecordHandler
		record_handler_ = std::make_shared<gbsip_server::RecordHandler>(
			sip_context_,
			sip_server_info_.sipServerId,
			sip_server_info_.ip,
			sip_server_info_.port
		);
		
		stoped_ = false;
		return true;
		//return false;
	}
	bool SipServer::Start()
	{
		if (!sip_context_)
		{
			return false;
		}
		SIPSERVER_LOG(LS_INFO) << "Sip ServerStart ... ";
		//while (!stoped_)
		//{
		//	eXosip_event_t *event_sip = eXosip_event_wait(sip_context_, 0, 20);
		//	if (!event_sip) {
		//		eXosip_automatic_action(sip_context_);
		//		osip_usleep(100000);
		//		continue;
		//	}
		//	 
		//	eXosip_automatic_action(sip_context_);
		//	auto event_iter = sip_event_callback_map_.find(event_sip->type);
		//	if (event_iter != sip_event_callback_map_.end())
		//	{
		//		(this->*(event_iter->second))(event_sip);
		//	}
		//	else
		//	{
		//		RTC_LOG(LS_WARNING) << "sip event callback type:" << event_sip->type;
		//	
		//	}
		//	//if (request_invite_)
		//	//{
		//	//	request_invite("41010500002000000003", "192.168.1.64", 5060);
		//	//	request_invite_ = false;
		//	//}
		//	//this->sip_event_handle(evtp);
		//	eXosip_event_free(event_sip);
		//}

	//	network_thread()->PostDelayedTask(
	//		webrtc::ToQueuedTask(task_safety_, [this]() { LoopSip(); }), delay_);
		network_thread()->PostDelayedTask(RTC_FROM_HERE, [this]() { LoopSip(); }, delay_);
			 		//webrtc::ToQueuedTask(task_safety_, [this]() { LoopSip(); }), delay_);
		return true;
	}
	void SipServer::LoopSip()
	{
		if (stoped_)
		{
			return;
		}

		while (!stoped_)
		{
			eXosip_event_t *event_sip = eXosip_event_wait(sip_context_, 0, 20);
			eXosip_automatic_action(sip_context_);
			if (event_sip)
			{
				auto event_iter = sip_event_callback_map_.find(event_sip->type);
				if (event_iter != sip_event_callback_map_.end())
				{
					(this->*(event_iter->second))(event_sip);
				}
				else
				{
					SIPSERVER_LOG(LS_WARNING) << "sip event callback type:" << event_sip->type;

				}
				//if (request_invite_)
				//{
				//	request_invite("41010500002000000003", "192.168.1.64", 5060);
				//	request_invite_ = false;
				//}
				//this->sip_event_handle(evtp);
				eXosip_event_free(event_sip);
			}
			else
			{
				break;
			}
		}
		//network_thread()->PostDelayedTask(
	//		webrtc::ToQueuedTask(task_safety_, [this]() { LoopSip(); }), delay_);
		network_thread()->PostDelayedTask(RTC_FROM_HERE, [this]() { LoopSip(); }, delay_);
	}
	void SipServer::HandlerSipcallMessageNew(eXosip_event_t * sip_event)
	{
		SIPSERVER_LOG(LS_INFO) << "";
		request_info(sip_event);
		response_info(sip_event);
	}
	void SipServer::HandlerSipCallClosed(eXosip_event_t * sip_event)
	{
		SIPSERVER_LOG(LS_INFO) << "";
		request_info(sip_event);
		response_info(sip_event);
	}
	void SipServer::HandlerSipCallReleased(eXosip_event_t * sip_event)
	{
		SIPSERVER_LOG(LS_INFO) << "";

		request_info(sip_event);
		response_info(sip_event);
		client_map_.clear();
	}
	void SipServer::HandlerSipCallInvite(eXosip_event_t * sip_event)
	{
		SIPSERVER_LOG(LS_INFO) << "EXOSIP_CALL_INVITE type=: " <<sip_event->type <<" The server receives the Invite request actively sent by the client";
	}
	void SipServer::HandlerSipCallProceeding(eXosip_event_t * sip_event)
	{
		SIPSERVER_LOG(LS_INFO) << "EXOSIP_CALL_PROCEEDING type=: "<<sip_event->type <<" When the server receives the Invite (SDP) confirmation reply from the client";
		request_info(sip_event);
		response_info(sip_event);
	}
	void SipServer::HandlerSipCallAnswered(eXosip_event_t * sip_event)
	{
		SIPSERVER_LOG(LS_INFO) << "EXOSIP_CALL_ANSWERED type=:"<<sip_event->type <<" The server receives an invite (SDP) confirmation reply from the client";
		request_info(sip_event);
		response_info(sip_event);


		//返回ack 
		osip_message_t* msg = nullptr;
		int ret = eXosip_call_build_ack(sip_context_, sip_event->did, &msg);
		if (!ret && msg) {
			eXosip_call_send_ack(sip_context_, sip_event->did, msg);
			// ack 信息
			//sip_event->ack->req_uri->username;
		}
		else {
			SIPSERVER_LOG(LS_INFO) << "eXosip_call_send_ack  error:" << ret;
			 
		}
	}
	void SipServer::HandlerSipCallServerFailure(eXosip_event_t * sip_event)
	{
		SIPSERVER_LOG(LS_INFO) << "EXOSIP_CALL_SERVERFAILURE type:" << sip_event->type;
	}
	void SipServer::HandlerSipMessageNew(eXosip_event_t * sip_event)
	{
		SIPSERVER_LOG(LS_INFO) << "";
		if (MSG_IS_REGISTER(sip_event->request)) {
			response_register(sip_event);
		}
		else if (MSG_IS_MESSAGE(sip_event->request)) {
			response_message(sip_event);
		}
		else if (strncmp(sip_event->request->sip_method, "BYE", 3) != 0) {
			SIPSERVER_LOG(LS_WARNING) << "sip_method: " << sip_event->request->sip_method;
		}
		else {
			SIPSERVER_LOG(LS_WARNING) << "sip_method: " << sip_event->request->sip_method;
		}
	}
	void SipServer::HandlerSipMessageAnswerd(eXosip_event_t * sip_event)
	{
		SIPSERVER_LOG(LS_INFO) << "";
		request_info(sip_event);
		
		// 检查是否有响应消息体
		if (!sip_event || !sip_event->response) {
			return;
		}
		
		osip_body_t* body = nullptr;
		osip_message_get_body(sip_event->response, 0, &body);
		if (!body || !body->body) {
			return;
		}
		
		// 解析XML获取CmdType
		std::string xml(body->body);
		auto root = gbsip_server::XmlHelper::parseXml(xml);
		if (!root) {
			SIPSERVER_LOG(LS_WARNING) << "Failed to parse XML response";
			return;
		}
		
		std::string cmdType = gbsip_server::XmlHelper::getNodeValue(root, "Response/CmdType");
		SIPSERVER_LOG(LS_INFO) << "Received response CmdType: " << cmdType;
		
		// 根据CmdType分发到对应的Handler
		if (cmdType == "DeviceInfo") {
			if (device_info_handler_) {
				device_info_handler_->handleMessageResponse(sip_event);
			}
		} else if (cmdType == "DeviceStatus") {
			if (device_info_handler_) {
				device_info_handler_->handleMessageResponse(sip_event);
			}
		} else if (cmdType == "RecordInfo") {
			if (record_handler_) {
				record_handler_->handleMessageResponse(sip_event);
			}
		}
		// 其他CmdType（如Catalog）可以在这里添加
	}
	void SipServer::HandlerSipMessageRequestFailure(eXosip_event_t * sip_event)
	{
		SIPSERVER_LOG(LS_INFO) << "EXOSIP_MESSAGE_REQUESTFAILURE type=: " <<sip_event->type <<" Receive feedback on sending failure after actively sending a message";
		request_info(sip_event);
		response_info(sip_event);
	}
	void SipServer::HandlerSipInSubscriptionNew(eXosip_event_t * sip_event)
	{
		SIPSERVER_LOG(LS_INFO) << "EXOSIP_IN_SUBSCRIPTION_NEW type=" << sip_event->type;
	}
	void SipServer::request_info(eXosip_event_t * sip_event)
	{
		char *info;
		size_t len = 0;
		osip_message_to_str(sip_event->request, &info, &len);
		SIPSERVER_LOG(LS_INFO) << " request type:" << sip_event->type << ", " << info;
	}
	void SipServer::response_info(eXosip_event_t * sip_event)
	{
		char *info;
		size_t len =0 ;
		osip_message_to_str(sip_event->response, &info, &len);
		SIPSERVER_LOG(LS_INFO) << " response type:" << sip_event->type << ", " << info;
		 
	}
	void SipServer::response_register(eXosip_event_t * sip_event)
	{
		osip_authorization_t * auth = nullptr;
		osip_message_get_authorization(sip_event->request, 0, &auth);

		if (auth && auth->username) {

			char *method = NULL, // REGISTER
				*algorithm = NULL, // MD5
				*username = NULL,// 340200000013200000024
				*realm = NULL, // sip服务器传给客户端，客户端携带并提交上来的sip服务域
				*nonce = NULL, //sip服务器传给客户端，客户端携带并提交上来的nonce
				*nonce_count = NULL,
				*uri = NULL; // sip:34020000002000000001@3402000000

			osip_contact_t *contact = nullptr;
			osip_message_get_contact(sip_event->request, 0, &contact);

			method = sip_event->request->sip_method;
			char calc_response[HASHHEXLEN];
			HASHHEX HA1, HA2 = "", Response;

#define SIP_STRDUP(field) if (auth->field) (field) = osip_strdup_without_quote(auth->field)

			SIP_STRDUP(algorithm);
			SIP_STRDUP(username);
			SIP_STRDUP(realm);
			SIP_STRDUP(nonce);
			SIP_STRDUP(nonce_count);
			SIP_STRDUP(uri);


			DigestCalcHA1(algorithm, username, realm, sip_server_info_.SipSeverPass.c_str(),   nonce, nonce_count, HA1);
			DigestCalcResponse(HA1, nonce, nonce_count, auth->cnonce, auth->message_qop, 0, method, uri, HA2, Response);

			HASHHEX temp_HA1;
			HASHHEX temp_response;
			DigestCalcHA1("REGISTER", username, sip_server_info_.SipServerRealm.c_str(),  
				sip_server_info_.SipSeverPass.c_str(), sip_server_info_.nonce.c_str(),    NULL, temp_HA1);
			DigestCalcResponse(temp_HA1, sip_server_info_.nonce.c_str()  , NULL, NULL, NULL, 0, method, uri, NULL, temp_response);
			memcpy(calc_response, temp_response, HASHHEXLEN);

			std::shared_ptr<SipClient> sip_client = std::make_shared<SipClient>();
			 
			sip_client->ip = contact->url->host;
			sip_client->port = std::atoi(contact->url->port);
			sip_client->device = username;
			if (!memcmp(calc_response, Response, HASHHEXLEN)) {
				response_message_answer(sip_event, 200);
				 
				SIPSERVER_LOG(LS_INFO) << "Camera registration succee,ip:" << sip_client->ip << ", port:" << sip_client->port << ", device:" << sip_client->device;
				
				client_map_.insert(std::make_pair(sip_client->device, sip_client));
				//DeviceDto  ;
				auto devicedto = oatpp::Object<DeviceDto>::createShared();;

				//osip_message_get_user_agent(sip_context_, );

				devicedto->deviceID = sip_client->device;
				devicedto->UpdatedAt = std::time(NULL);
				devicedto->CreatedAt = std::time(NULL);
				devicedto->RemoteIP = sip_client->ip;
				devicedto->RemotePort = sip_client->port;
				devicedto->online = 1;
				device_db_->createDevice(std::move(devicedto));

				//request_invite(sip_client->device, sip_client->ip, sip_client->port);

			}
			else {
				response_message_answer(sip_event, 401); 
				RTC_LOG(LS_WARNING) << "Camera registration error, p:" << sip_client->ip << ", port:" << sip_client->port << ", device:" << sip_client->device;
				sip_client.reset();
			}

			osip_free(algorithm);
			osip_free(username);
			osip_free(realm);
			osip_free(nonce);
			osip_free(nonce_count);
			osip_free(uri);
		}
		else {
			response_register_401unauthorized(sip_event);
		}
	}
	void SipServer::response_message_answer(eXosip_event_t * sip_event, int32_t code)
	{
		int returnCode = 0;
		osip_message_t * pRegister = nullptr;
		returnCode = eXosip_message_build_answer(sip_context_, sip_event->tid, code, &pRegister);
		bool bRegister = false;
		if (pRegister) {
			bRegister = true;
		}
		if (returnCode == 0 && bRegister)
		{
			eXosip_lock(sip_context_);
			eXosip_message_send_answer(sip_context_, sip_event->tid, code, pRegister);
			eXosip_unlock(sip_context_);
		}
		else 
		{
			SIPSERVER_LOG(LS_INFO) << "code:" << code << ", returnCode:" << returnCode << ", regitster:" << bRegister;
			 
		}
	}
	int32_t SipServer::request_invite(const std::string & device, const std::string & remote_ip, uint16_t remote_port, uint16_t rtp_port)
	{
		int32_t ret = 0;
		{
			char session_exp[1024] = { 0 };
			osip_message_t *msg = nullptr;
			char from[1024] = { 0 };
			char to[1024] = { 0 };
			char contact[1024] = { 0 };
			char sdp[2048] = { 0 };
			char head[1024] = { 0 };


			sprintf(from, "sip:%s@%s:%d", sip_server_info_.sipServerId.c_str(), sip_server_info_.ip.c_str(), sip_server_info_.port);
			sprintf(contact, "sip:%s@%s:%d", sip_server_info_.sipServerId.c_str(), sip_server_info_.ip.c_str(), sip_server_info_.port);
			sprintf(to, "sip:%s@%s:%d", device.c_str(), remote_ip.c_str(), remote_port);
			snprintf(sdp, 2048,
				"v=0\r\n"
				"o=%s 0 0 IN IP4 %s\r\n"
				"s=Play\r\n"
				"c=IN IP4 %s\r\n"
				"t=0 0\r\n"
				"m=video %d TCP/RTP/AVP 96 98 97\r\n"
				"a=recvonly\r\n"
				"a=rtpmap:96 PS/90000\r\n"
				"a=rtpmap:98 H264/90000\r\n"
				"a=rtpmap:97 MPEG4/90000\r\n"
				"a=setup:passive\r\n"
				"a=connection:new\r\n"
				"y=0100000001\r\n"
				"f=\r\n", sip_server_info_.sipServerId.c_str(), sip_server_info_.ip.c_str(), sip_server_info_.ip.c_str(), rtp_port);

			ret = eXosip_call_build_initial_invite(sip_context_, &msg, to, from, nullptr, nullptr);
			if (ret) {
				SIPSERVER_LOG(LS_WARNING) << "eXosip_call_build_initial_invite error:" << from << ", " << to << ", ret:" << ret;
				return -1;
			}

			osip_message_set_body(msg, sdp, strlen(sdp));
			osip_message_set_content_type(msg, "application/sdp");
			snprintf(session_exp, sizeof(session_exp) - 1, "%i;refresher=uac", sip_server_info_.SipTimeout);
			osip_message_set_header(msg, "Session-Expires", session_exp);
			osip_message_set_supported(msg, "timer");

			int call_id = eXosip_call_send_initial_invite(sip_context_, msg);

			if (call_id > 0)
			{
				RTC_LOG(LS_INFO) << "eXosip_call_send_initial_invite success: call_id:" << call_id;
			}
			else {
				RTC_LOG(LS_WARNING) << "eXosip_call_send_initial_invite error: call_id:" << call_id;
			}
		}
		eXosip_event_t *event_sip = eXosip_event_wait(sip_context_, 0, 20);
		eXosip_automatic_action(sip_context_);
		if (event_sip)
		{
			auto event_iter = sip_event_callback_map_.find(event_sip->type);
			if (event_iter != sip_event_callback_map_.end())
			{
				(this->*(event_iter->second))(event_sip);
			}
			else
			{
				SIPSERVER_LOG(LS_WARNING) << "sip event callback type:" << event_sip->type;

			}
			
		
			//if (request_invite_)
			//{
			//	request_invite("41010500002000000003", "192.168.1.64", 5060);
			//	request_invite_ = false;
			//}
			//this->sip_event_handle(evtp);
			eXosip_event_free(event_sip);
		}
		
		return ret;
	}
	void SipServer::response_message(eXosip_event_t * sip_event)
	{
		osip_body_t* body = nullptr;
		char CmdType[64] = { 0 };
		char DeviceID[64] = { 0 };
		osip_message_get_body(sip_event->request, 0, &body);
		if (body) {
			parse_xml(body->body, "<CmdType>", false, "</CmdType>", false, CmdType);
			parse_xml(body->body, "<DeviceID>", false, "</DeviceID>", false, DeviceID);
		}

		//    Client *client = getClientByDevice(DeviceID);
		//    if(client){
		//        LOGI("response_message：%s 已注册",DeviceID);
		//    }else{
		//        LOGE("response_message：%s 未注册",DeviceID);
		//    }


		auto iter = client_map_.find(DeviceID);
		if (iter != client_map_.end())
		{
			SIPSERVER_LOG(LS_INFO) << " response mssage:  resis: " << DeviceID;
		 }
		else
		{
			SIPSERVER_LOG(LS_INFO) << " response mssage:  not resis: " << DeviceID;
		}
		SIPSERVER_LOG(LS_INFO) << "CmdType:" << CmdType << ", DeviceID:" << DeviceID;;
		if (!strcmp(CmdType, "Catalog")) {
			response_message_answer(sip_event, 200);
			// 需要根据对方的Catelog请求，做一些相应的应答请求
		}
		else if (!strcmp(CmdType, "Keepalive")) {
			response_message_answer(sip_event, 200);
		}
		else {
			response_message_answer(sip_event, 200);
		}
	}
	void SipServer::response_register_401unauthorized(eXosip_event_t * sip_event)
	{
		char *dest = nullptr;
		osip_message_t * reg = nullptr;
		osip_www_authenticate_t * header = nullptr;

		osip_www_authenticate_init(&header);
		osip_www_authenticate_set_auth_type(header, osip_strdup("Digest"));
		osip_www_authenticate_set_realm(header, osip_enquote( sip_server_info_.SipServerRealm.c_str()   ));
		osip_www_authenticate_set_nonce(header, osip_enquote(sip_server_info_.nonce.c_str()));
		osip_www_authenticate_to_str(header, &dest);
		int ret = eXosip_message_build_answer(sip_context_, sip_event->tid, 401, &reg);
		if (ret == 0 && reg != nullptr) {
			osip_message_set_www_authenticate(reg, dest);
			osip_message_set_content_type(reg, "Application/MANSCDP+xml");
			eXosip_lock(sip_context_);
			eXosip_message_send_answer(sip_context_, sip_event->tid, 401, reg);
			eXosip_unlock(sip_context_);
			SIPSERVER_LOG(LS_INFO) << "response_register_401unauthorized success";
		}
		else {
			SIPSERVER_LOG(LS_WARNING) << "response_register_401unauthorized error";
		}

		osip_www_authenticate_free(header);
		osip_free(dest);
	}
}