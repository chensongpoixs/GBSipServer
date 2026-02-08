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

#include "utils/yaml_config.h"
#include "yaml-cpp/yaml.h"
#include "rtc_base/logging.h"
#include "gbsip_server_log.h"
namespace   gbsip_server
{
	bool YamlConfig::LoadFile(const char* config_file)
	{
		try {
			// ���ļ�����YAML
			YAML::Node node = YAML::LoadFile(config_file);

			if (node["server"]) {
				http_server_config_.ip = node["server"]["ip"].as<std::string>();
				http_server_config_.port = node["server"]["port"].as<uint16_t>();

				SIPSERVER_LOG(LS_INFO) << "http server config  host:"<< http_server_config_.ip << ", port:" << http_server_config_.port;
			}

			if (node["sip"]) {
				sip_server_config_.ip = node["sip"]["ip"].as< std::string>();
				sip_server_config_.port = node["sip"] ["port"].as<uint16_t>();
				sip_server_config_.sipServerId = node["sip"]["id"] .as<std::string>();
				sip_server_config_.SipSeverPass = node["sip"]["password"].as<std::string>();;
				sip_server_config_.SipServerRealm = node["sip"]["domain"].as<std::string>();;

				 

				SIPSERVER_LOG(LS_INFO) << " sip server config : \n " << sip_server_config_.ToString();
			}
			if (node["media"])
			{
				media_config_.ip = node["media"]["ip"].as<std::string>();
				media_config_.port = node["media"]["http-port"].as<uint16_t>();
				 
				SIPSERVER_LOG(LS_INFO) << "media config info ��\n" << "ip : " << media_config_.ip
					<< ", http-port: " << media_config_.port;
			}
		}
		catch (const YAML::Exception& e) {
		//	std::cerr << "YAML Error: " << e.what() << "\n";
			SIPSERVER_LOG_T_F(LS_WARNING) << "config_file: "<< config_file
				<<" yaml parse failed !!!   error: " << e.what();
			return false;
			//return 1;
		}
		return true;
	}
}