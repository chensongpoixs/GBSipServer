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
 * 输赢不重要，答案对你们有什么意义才重要。
 *
 * 光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。
 *
 *
 *我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
 *然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
 *3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
 *然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
 *于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
 *我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
 *从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
 *我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
 *沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
 *安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。
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
namespace gbsip_server
{
	/**
	*  @author chensong
	*  @date 2025-10-12
	*  @brief SIP客户端信息结构体（SIP Client Information）
	*  
	*  该结构体用于存储已注册的SIP客户端（通常是GB28181设备）的信息。
	*  包括设备的网络地址、设备ID、注册状态和RTP端口等信息。
	*  
	*  结构体成员说明：
	*  - ip: 客户端的IP地址，用于发送SIP消息和RTP数据
	*  - port: 客户端的SIP端口，通常为5060
	*  - device: 设备ID，符合GB28181标准的20位编码
	*  - isreg: 注册状态，true表示已注册，false表示未注册
	*  - rtp_port: RTP端口，用于接收媒体流数据
	*  
	*  GB28181设备ID格式：
	*  - 前8位：中心编码（省市县）
	*  - 第9-10位：行业编码
	*  - 第11-13位：类型编码
	*  - 第14-20位：设备序号
	*  
	*  @note 该结构体用于管理已注册的设备信息
	*  @note 设备注册成功后会创建SipClient实例并存储到client_map_中
	*  
	*  使用示例：
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
		std::string ip;        // 客户端IP地址
		uint16_t     port;     // 客户端SIP端口（通常为5060）
		std::string device;    // 设备ID（GB28181标准20位编码）
		bool    isreg;         // 注册状态（true=已注册，false=未注册）
		uint16_t   rtp_port;   // RTP端口（用于接收媒体流）
	};


	/**
	*  @author chensong
	*  @date 2025-10-12
	*  @brief GB28181 SIP服务器类（GB28181 SIP Server）
	*  
	*  SipServer是GB28181 SIP服务器的核心类，采用单例模式。
	*  它负责处理SIP协议的注册、心跳、目录查询、媒体流控制等功能。
	*  
	*  主要功能：
	*  1. SIP服务器初始化和启动
	*  2. 处理设备注册（REGISTER）
	*  3. 处理设备心跳（Keepalive）
	*  4. 处理目录查询（Catalog）
	*  5. 发起媒体流请求（INVITE）
	*  6. 处理SIP事件回调
	*  7. 管理已注册设备
	*  
	*  GB28181协议说明：
	*  - GB28181是中国公共安全视频监控联网系统信息传输、交换、控制技术要求
	*  - 使用SIP协议进行信令控制
	*  - 使用RTP/RTCP协议进行媒体传输
	*  - 支持设备注册、心跳、目录查询、实时视频、历史回放等功能
	*  
	*  工作流程：
	*  1. 初始化eXosip库，创建SIP上下文
	*  2. 绑定UDP端口，监听SIP消息
	*  3. 循环处理SIP事件（注册、心跳、INVITE等）
	*  4. 根据事件类型调用对应的处理函数
	*  5. 管理已注册设备，维护设备列表
	*  
	*  @note SipServer采用单例模式，确保全局只有一个实例
	*  @note 使用eXosip2库处理SIP协议
	*  @note 使用oatpp框架提供HTTP API接口
	*  @note 使用SQLite数据库存储设备信息
	*  
	*  使用示例：
	*  @code
	*  // 获取单例实例
	*  SipServer& server = SipServer::GetInstance();
	*  
	*  // 初始化服务器
	*  SipServerConfig config;
	*  config.ip = "192.168.1.1";
	*  config.port = 5060;
	*  server.init(config);
	*  
	*  // 启动服务器
	*  server.Start();
	*  @endcode
	*/
	class SipServer
	{
	public:
		/**
		*  @brief SIP事件回调函数指针类型定义（SIP Event Callback Function Pointer）
		*  
		*  该类型定义了SIP事件回调函数的签名。所有SIP事件处理函数都必须符合此签名。
		*  
		*  @param sip_event eXosip事件指针，包含SIP消息和事件信息
		*/
		typedef  void (SipServer::*SipEventCallback)(eXosip_event_t * sip_event)    ;
		
	public:
		/**
		*  @author chensong
		*  @date 2025-10-12
		*  @brief 构造函数（Constructor）
		*  
		*  该构造函数用于初始化SipServer实例。它会创建连接上下文，
		*  初始化SIP事件回调映射表，并注入设备数据库组件。
		*  
		*  初始化流程：
		*  1. 创建ConnectionContext，提供信令线程、工作线程和网络线程
		*  2. 初始化stoped_标志为true
		*  3. 初始化sip_context_为nullptr
		*  4. 注册所有SIP事件回调函数到映射表
		*  5. 注入DeviceDb组件（通过oatpp依赖注入）
		*  
		*  @note 构造函数是私有的，只能通过GetInstance方法获取实例（单例模式）
		*/
		SipServer();
		
		/**
		*  @author chensong
		*  @date 2025-10-12
		*  @brief 析构函数（Destructor）
		*  
		*  该析构函数用于清理SipServer实例。它会释放eXosip上下文和相关资源。
		*  
		*  清理流程：
		*  1. 检查sip_context_是否存在
		*  2. 调用eXosip_quit释放eXosip上下文
		*  3. 清理所有资源
		*  
		*  @note 析构函数会自动调用，不需要手动释放资源
		*/
		~SipServer();

		/**
		*  @author chensong
		*  @date 2025-10-12
		*  @brief 获取单例实例（Get Instance）
		*  
		*  该方法用于获取SipServer的单例实例。采用线程安全的单例模式实现，
		*  确保整个系统中只有一个SIP服务器实例。
		*  
		*  @return 返回SipServer单例实例的引用
		*  @note 该方法线程安全，使用C++11的静态局部变量实现单例模式
		*  @note 单例实例在第一次调用时创建，程序结束时自动销毁
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
		
		// Handler成员
		std::shared_ptr<gbsip_server::DeviceInfoHandler> device_info_handler_;
		std::shared_ptr<gbsip_server::PTZHandler> ptz_handler_;
	public:
		// 获取Handler的方法
		std::shared_ptr<gbsip_server::DeviceInfoHandler> getDeviceInfoHandler() {
			return device_info_handler_;
		}
		std::shared_ptr<gbsip_server::PTZHandler> getPTZHandler() {
			return ptz_handler_;
		}
	};
}



#endif // _C__SIP_SERVER_H_

