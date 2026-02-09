/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Please visit https://chensongpoixs.github.io for detail
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 *
 *
 *
 *				   Author: chensong
 *				   date:  2025-10-12
 *
 * ��Ӯ����Ҫ���𰸶�������ʲô�������Ҫ��
 *
 * �����ߣ��ٴ�֮����Ҳ��Ψ�з������ܣ�����������ʱ����ʱ����Ӣ�ۣ�Ӣ�۴�����ʱ�����������˵�����񣬿��㱾�����ٰ��� �����ã��Լ�������ͼ��顣
 *
 *
 *�ҿ��ܻ������ܶ���ˣ������ǽ���2��Ĺ��£�����д�ɹ��»��ɸ裬����ѧ���ĸ���������������
 *Ȼ�󻹿�����һ����������һ�������ҵĹ�������ܻᱻ��˧������������ֻᱻ��������ں���������ĳ�������ҹ��������ȫ����͸Ȼ��Ҫ������С��ס�������ϵ�ʪ�·���
 *3Сʱ���������������ʵ��������ҵĹ�������Ը���򸸻���顣�Ҳ����Ѹ��������Ǵ�Խ�����������ǣ���ʼ��Ҫ�ص��Լ������硣
 *Ȼ���ҵ���Ӱ������ʧ���ҿ������������ˮ������ȴû���κ�ʹ�࣬�Ҳ�֪����ԭ���ҵ��ı������ˣ�������ȫ�����ԭ�򣬾���Ҫ�һ��Լ��ı��ġ�
 *�����ҿ�ʼ����Ѱ�Ҹ��ָ���ʧȥ�ĵ��ˣ��ұ��һ��שͷ��һ������һ��ˮ��һ����ƣ�ȥ�����Ϊʲô��ʧȥ�Լ��ı��ġ�
 *�ҷ��֣��ճ����ı��������Ļ��ڣ������ģ����ǵı��ľͻ���ʧ���յ��˸��ֺڰ�֮�����ʴ��
 *��һ�����ۣ������ʺͱ��ߣ�����ί����ʹ�࣬�ҿ���һֻֻ���ε��֣������ǵı��ĳ��飬�ɱΣ�͵�ߣ���Ҳ�ز������˶����ߡ�
 *�ҽ����������֡��������Ǻ�����ͬ�ڵļ��� �����Ҳ������£�����ϸ�����Լ�ƽ����һ�� Ѱ�ұ������ֵĺۼ���
 *�����Լ��Ļ��䣬һ�����ĳ�����������������֣��ҵı��ģ�����д�����ʱ�򣬻������
 *��������Ȼ����������ҵ�һ�У�д��������ұ��Ļع����÷�ʽ���һ�û�ҵ��������֣��������ţ�˳�������������һ����˳�����ϣ�������������
 *
 *******************************************************************************/
#ifndef _C__SIP_SERVER_H_
#define _C__SIP_SERVER_H_

#include <string>
#include <atomic>
extern "C" {
#include <osip2/osip_mt.h>
#include <eXosip2/eXosip.h>
}
#include <unordered_map>
#include <memory>
#include <memory>
#include "db/DeviceDb.hpp"
#include "oatpp/network/Server.hpp"
#include "AppComponent.hpp"
#include "libp2p_peerconnection/connection_context.h"
#include "utils/yaml_config.h"
#include "sip/DeviceInfoHandler.hpp"
#include "sip/PTZHandler.hpp"
#include "sip/RecordHandler.hpp"
#include "sip/AlarmHandler.hpp"
namespace gbsip_server
{
	/**
	*  @author chensong
	*  @date 2025-10-12
	*  @brief SIP�ͻ�����Ϣ�ṹ�壨SIP Client Information��
	*  
	*  �ýṹ�����ڴ洢��ע���SIP�ͻ��ˣ�ͨ����GB28181�豸������Ϣ��
	*  �����豸�������ַ���豸ID��ע��״̬��RTP�˿ڵ���Ϣ��
	*  
	*  �ṹ���Ա˵����
	*  - ip: �ͻ��˵�IP��ַ�����ڷ���SIP��Ϣ��RTP����
	*  - port: �ͻ��˵�SIP�˿ڣ�ͨ��Ϊ5060
	*  - device: �豸ID������GB28181��׼��20λ����
	*  - isreg: ע��״̬��true��ʾ��ע�ᣬfalse��ʾδע��
	*  - rtp_port: RTP�˿ڣ����ڽ���ý��������
	*  
	*  GB28181�豸ID��ʽ��
	*  - ǰ8λ�����ı��루ʡ���أ�
	*  - ��9-10λ����ҵ����
	*  - ��11-13λ�����ͱ���
	*  - ��14-20λ���豸���
	*  
	*  @note �ýṹ�����ڹ�����ע����豸��Ϣ
	*  @note �豸ע��ɹ���ᴴ��SipClientʵ�����洢��client_map_��
	*  
	*  ʹ��ʾ����
	*  @code
	*  std::shared_ptr<SipClient> client = std::make_shared<SipClient>();
	*  client->ip = "192.168.1.100";
	*  client->port = 5060;
	*  client->device = "34020000001320000001";
	*  client->isreg = true;
	*  client->rtp_port = 10000;
	*  @endcode
	*/
	struct SipClient
	{
		std::string ip;        // �ͻ���IP��ַ
		uint16_t     port;     // �ͻ���SIP�˿ڣ�ͨ��Ϊ5060��
		std::string device;    // �豸ID��GB28181��׼20λ���룩
		bool    isreg;         // ע��״̬��true=��ע�ᣬfalse=δע�ᣩ
		uint16_t   rtp_port;   // RTP�˿ڣ����ڽ���ý������
	};


	/**
	*  @author chensong
	*  @date 2025-10-12
	*  @brief GB28181 SIP�������ࣨGB28181 SIP Server��
	*  
	*  SipServer��GB28181 SIP�������ĺ����࣬���õ���ģʽ��
	*  ��������SIPЭ���ע�ᡢ������Ŀ¼��ѯ��ý�������Ƶȹ��ܡ�
	*  
	*  ��Ҫ���ܣ�
	*  1. SIP��������ʼ��������
	*  2. �����豸ע�ᣨREGISTER��
	*  3. �����豸������Keepalive��
	*  4. ����Ŀ¼��ѯ��Catalog��
	*  5. ����ý��������INVITE��
	*  6. ����SIP�¼��ص�
	*  7. ������ע���豸
	*  
	*  GB28181Э��˵����
	*  - GB28181���й�������ȫ��Ƶ�������ϵͳ��Ϣ���䡢���������Ƽ���Ҫ��
	*  - ʹ��SIPЭ������������
	*  - ʹ��RTP/RTCPЭ�����ý�崫��
	*  - ֧���豸ע�ᡢ������Ŀ¼��ѯ��ʵʱ��Ƶ����ʷ�طŵȹ���
	*  
	*  �������̣�
	*  1. ��ʼ��eXosip�⣬����SIP������
	*  2. ��UDP�˿ڣ�����SIP��Ϣ
	*  3. ѭ������SIP�¼���ע�ᡢ������INVITE�ȣ�
	*  4. �����¼����͵��ö�Ӧ�Ĵ�������
	*  5. ������ע���豸��ά���豸�б�
	*  
	*  @note SipServer���õ���ģʽ��ȷ��ȫ��ֻ��һ��ʵ��
	*  @note ʹ��eXosip2�⴦��SIPЭ��
	*  @note ʹ��oatpp����ṩHTTP API�ӿ�
	*  @note ʹ��SQLite���ݿ�洢�豸��Ϣ
	*  
	*  ʹ��ʾ����
	*  @code
	*  // ��ȡ����ʵ��
	*  SipServer& server = SipServer::GetInstance();
	*  
	*  // ��ʼ��������
	*  SipServerConfig config;
	*  config.ip = "192.168.1.1";
	*  config.port = 5060;
	*  server.init(config);
	*  
	*  // ����������
	*  server.Start();
	*  @endcode
	*/
	class SipServer
	{
	public:
		/**
		*  @brief SIP�¼��ص�����ָ�����Ͷ��壨SIP Event Callback Function Pointer��
		*  
		*  �����Ͷ�����SIP�¼��ص�������ǩ��������SIP�¼�����������������ϴ�ǩ����
		*  
		*  @param sip_event eXosip�¼�ָ�룬����SIP��Ϣ���¼���Ϣ
		*/
		typedef  void (SipServer::*SipEventCallback)(eXosip_event_t * sip_event)    ;
		
	public:
		/**
		*  @author chensong
		*  @date 2025-10-12
		*  @brief ���캯����Constructor��
		*  
		*  �ù��캯�����ڳ�ʼ��SipServerʵ�������ᴴ�����������ģ�
		*  ��ʼ��SIP�¼��ص�ӳ�������ע���豸���ݿ������
		*  
		*  ��ʼ�����̣�
		*  1. ����ConnectionContext���ṩ�����̡߳������̺߳������߳�
		*  2. ��ʼ��stoped_��־Ϊtrue
		*  3. ��ʼ��sip_context_Ϊnullptr
		*  4. ע������SIP�¼��ص�������ӳ���
		*  5. ע��DeviceDb�����ͨ��oatpp����ע�룩
		*  
		*  @note ���캯����˽�еģ�ֻ��ͨ��GetInstance������ȡʵ��������ģʽ��
		*/
		SipServer();
		
		/**
		*  @author chensong
		*  @date 2025-10-12
		*  @brief ����������Destructor��
		*  
		*  ������������������SipServerʵ���������ͷ�eXosip�����ĺ������Դ��
		*  
		*  �������̣�
		*  1. ���sip_context_�Ƿ����
		*  2. ����eXosip_quit�ͷ�eXosip������
		*  3. ����������Դ
		*  
		*  @note �����������Զ����ã�����Ҫ�ֶ��ͷ���Դ
		*/
		~SipServer();

		/**
		*  @author chensong
		*  @date 2025-10-12
		*  @brief ��ȡ����ʵ����Get Instance��
		*  
		*  �÷������ڻ�ȡSipServer�ĵ���ʵ���������̰߳�ȫ�ĵ���ģʽʵ�֣�
		*  ȷ������ϵͳ��ֻ��һ��SIP������ʵ����
		*  
		*  @return ����SipServer����ʵ��������
		*  @note �÷����̰߳�ȫ��ʹ��C++11�ľ�̬�ֲ�����ʵ�ֵ���ģʽ
		*  @note ����ʵ���ڵ�һ�ε���ʱ�������������ʱ�Զ�����
		*/
		static SipServer & GetInstance()
		{
			static SipServer   instance;
			return instance;
		}
	public:
		OATPP_COMPONENT(std::shared_ptr<DeviceDb>,  device_db_);
	public:
		bool init(const SipServerConfig& info);
		bool Start();


		void request_invite() { request_invite_ = true; };



		rtc::Thread* signaling_thread() { return context_->signaling_thread(); }
		const rtc::Thread* signaling_thread() const { return context_->signaling_thread(); }
		rtc::Thread* worker_thread() { return context_->worker_thread(); }
		const rtc::Thread* worker_thread() const { return context_->worker_thread(); }
		rtc::Thread* network_thread() { return context_->network_thread(); }
		const rtc::Thread* network_thread() const { return context_->network_thread(); }


		void LoopSip();

	public:

		void HandlerSipcallMessageNew(eXosip_event_t * sip_event);
		void HandlerSipCallClosed(eXosip_event_t * sip_event);
		void HandlerSipCallReleased(eXosip_event_t * sip_event);
		void HandlerSipCallInvite(eXosip_event_t * sip_event);
		void HandlerSipCallProceeding(eXosip_event_t * sip_event);
		void HandlerSipCallAnswered(eXosip_event_t * sip_event);
		void HandlerSipCallServerFailure(eXosip_event_t * sip_event);

		void HandlerSipMessageNew(eXosip_event_t * sip_event);
		void HandlerSipMessageAnswerd(eXosip_event_t * sip_event);
		void HandlerSipMessageRequestFailure(eXosip_event_t * sip_event);
		
		
		void HandlerSipInSubscriptionNew(eXosip_event_t * sip_event);
		 
	public:


		void  request_info(eXosip_event_t * sip_event);
		void  response_info(eXosip_event_t * sip_event);

		void  response_register(eXosip_event_t *sip_event);
		void   response_message_answer(eXosip_event_t * sip_event, int32_t code);


		int32_t   request_invite(const std::string&  device, const std::string& remote_ip, uint16_t remote_port, uint16_t rtp_port);
		void     response_message(eXosip_event_t * sip_event);
		void response_register_401unauthorized(eXosip_event_t * sip_event);
	private:
	 	webrtc::ScopedTaskSafety task_safety_;
		rtc::scoped_refptr<libp2p_peerconnection::ConnectionContext>	context_;
		std::atomic<bool>  stoped_;
		SipServerConfig    sip_server_info_;
		struct eXosip_t *sip_context_;

		
		
		std::unordered_map<int32_t, SipEventCallback>        sip_event_callback_map_;


		std::unordered_map<std::string, std::shared_ptr<SipClient>>        client_map_;
		bool request_invite_ = false;
		int32_t  delay_ = 10;
		
		// Handler��Ա
		std::shared_ptr<gbsip_server::DeviceInfoHandler> device_info_handler_;
		std::shared_ptr<gbsip_server::PTZHandler> ptz_handler_;
	std::shared_ptr<gbsip_server::RecordHandler> record_handler_;
	std::shared_ptr<gbsip_server::AlarmHandler> alarm_handler_;
public:
	// 获取Handler的方法
	std::shared_ptr<gbsip_server::DeviceInfoHandler> getDeviceInfoHandler() {
		return device_info_handler_;
	}
	std::shared_ptr<gbsip_server::PTZHandler> getPTZHandler() {
		return ptz_handler_;
	}
	std::shared_ptr<gbsip_server::RecordHandler> getRecordHandler() {
		return record_handler_;
	}
	std::shared_ptr<gbsip_server::AlarmHandler> getAlarmHandler() {
		return alarm_handler_;
	}
};
}



#endif // _C__SIP_SERVER_H_

