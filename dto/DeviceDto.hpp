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

#ifndef _C_DEVICE_DTO_hpp
#define _C_DEVICE_DTO_hpp

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

//ENUM(Role, v_int32,
//     VALUE(GUEST, 0, "ROLE_GUEST"),
//     VALUE(ADMIN, 1, "ROLE_ADMIN")
//)

class DeviceDto : public oatpp::DTO {
  
  DTO_INIT(DeviceDto, DTO)
	  /*
	  
CREATE TABLE t_device (
    id          INTEGER PRIMARY KEY,
    device_id    VARCHAR UNIQUE,
	type        INTEGER,
    device_name        VARCHAR ,
	channel_count INTEGER,
    password    VARCHAR,
	catalog_subscribe  INTEGER,
    online        INTEGER,
	sip_transport  INTEGER,
	media_transport  INTEGER,
	media_transport_mode INTEGER,
	last_register_at INTEGER,
	last_keepalive_at INTEGER,
	update_at INTEGER,
	create_at INTEGER,
	role        VARCHAR
);
	  */
  DTO_FIELD(Int32, id);
  DTO_FIELD(String, deviceID, "deviceid") = "21321321321321";  //设备编号
  DTO_FIELD(String, deviceName, "devicename") = "hik"; //设备名称
  DTO_FIELD(Int32, Type, "type") = 0; //设备类型, "GB" - 国标设备, "Decode" - 解码器
  DTO_FIELD(Int32, ChannelCount, "channelcount") = 0; // 通道数
  DTO_FIELD(String, password, "password") = "12345678";
  DTO_FIELD(Int32, CatalogSubscribe, "catalogsubscribe") = 0;// 目录订阅是否开启
  DTO_FIELD(Int32, online, "online") = 0; //是否在线
  DTO_FIELD(Int32, SipTransport, "siptransport") = 0;//信令传输模式 允许值: UDP, TCP
  DTO_FIELD(Int32, MediaTransport, "mediatransport")  =0;//流传输模式(TCP/UDP)
  DTO_FIELD(Int32, MediaTransportMode, "mediatransportmode") = 0;//流传输模式(主动/被动)允许值: passive, active
  DTO_FIELD(Int32, LastRegisterAt, "lastregisterat") = 0;// 最近注册
  DTO_FIELD(Int32, LastKeepaliveAt, "lastkeepaliveat") = 30;//最近心跳
  DTO_FIELD(Int32, UpdatedAt, "updatedat") = 0; //更新时间
  DTO_FIELD(Int32, CreatedAt, "createdat") = 0; //	创建时间
  DTO_FIELD(Int32, Longitude, "longitude" ) = 0; // 经度 默认值: 0
  DTO_FIELD(Int32, Latitude, "latitude") = 0; // 纬度
  DTO_FIELD(String, RemoteIP, "remoteip") = "192.168.3.4"; // 出口IP
  DTO_FIELD(Int32, RemotePort, "remoteport") = 15060; // 端口
 //CatalogSubscribe	Boolean	 目录订阅是否开启

  //AlarmSubscribe	Boolean	 报警订阅是否开启
  //PositionSubscribe	Boolean	 位置订阅是否开启
  // PTZSubscribe	Boolean	 PTZ订阅是否开启(2022)
 
  DTO_FIELD(Enum<Role>::AsString, role, "role");

};

#include OATPP_CODEGEN_END(DTO)

#endif /* UserDto_hpp */
