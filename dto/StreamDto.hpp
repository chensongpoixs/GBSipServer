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

#ifndef _C_STREAM_DTO_hpp
#define _C_STREAM_DTO_hpp

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

//ENUM(Role, v_int32,
//     VALUE(GUEST, 0, "ROLE_GUEST"),
//     VALUE(ADMIN, 1, "ROLE_ADMIN")
//)

class StreamDto : public oatpp::DTO {
  
  DTO_INIT(StreamDto, DTO)
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
  DTO_FIELD(String, deviceID, "serial") = "21321321321321";  //设备编号 
  DTO_FIELD(Int32, channel, "channel") = 0; // 通道序号 默认值: 1  // 
   
  DTO_FIELD(Int32, timeOut, "timeout") = 300; // 拉流超时(秒), 默认使用 livecms.ini > sip > ack_timeout
 

};

#include OATPP_CODEGEN_END(DTO)

#endif /* UserDto_hpp */
