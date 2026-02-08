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


#ifndef _C_GB_MEDIA_SERVER_CONFIG_H_H_
#define _C_GB_MEDIA_SERVER_CONFIG_H_H_
  

#include "rtc_base/logging.h" 



namespace  gbsip_server
{


	struct HttpServerConfig
	{
		std::string ip{ "127.0.0.1" };
		uint16_t port{ 8002 };
	};
	struct SipServerConfig
	{
		 
		std::string ua = "GbSipServer";
		std::string nonce = "4101050000"; //sip服务的随机数字
		std::string ip = "192.168.9.174"; // sip 
		uint16_t   port = 15060; // sip 
		std::string  sipServerId = "41010500002000000001"; //  sip服务的ID
		std::string  SipServerRealm = "4101050000"; //sip服务域名
		std::string  SipSeverPass = "12345678"; // sip password 
		int32_t      SipTimeout = 1800;
		int32_t      SipExpiry = 3600; //sip


		std::string ToString()
		{
			std::ostringstream cmd;
			cmd << " ua : " << ua << ", nonce: " << nonce << ", ip: " << ip;
			cmd << ", port:" << port << ", sipserverid: " << sipServerId << ", SipServerRealm:" << SipServerRealm;
			cmd << ", SipSeverPass:" << SipSeverPass;
			return cmd.str();
		}
	};

	struct MediaConfig
	{
		std::string ip;
		uint16_t port;
	};
	class YamlConfig
	{
	public:
		explicit YamlConfig() 
			: http_server_config_()
			, sip_server_config_()
			, media_config_()
		{}
		virtual ~YamlConfig(){}



	public:

		static YamlConfig& GetInstance()
		{
			static YamlConfig instance;
			return instance;
		}
	public:
		bool LoadFile(const char* config_file);




		const HttpServerConfig& GetHttpServerConfig() const { return http_server_config_; }
		const SipServerConfig& GetSipServerConfig() const { return sip_server_config_; }
		
		const MediaConfig& GetMediaConfig() const { return media_config_; }
	public:


		HttpServerConfig     http_server_config_;
		SipServerConfig      sip_server_config_;

		MediaConfig          media_config_;

	};
}


#endif // _C_GB_MEDIA_SERVER_STRING_UTILS_H_