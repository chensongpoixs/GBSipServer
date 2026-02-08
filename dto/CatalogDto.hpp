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
				  date:  2026-02-09
				  
				  GB28181 目录查询数据传输对象
******************************************************************************/

#ifndef _C_CATALOG_DTO_HPP
#define _C_CATALOG_DTO_HPP

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"
#include "ChannelDto.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * 目录查询请求DTO
 */
class CatalogQueryDto : public oatpp::DTO {
  
  DTO_INIT(CatalogQueryDto, DTO)
  
  DTO_FIELD_INFO(deviceId) {
    info->description = "设备编号";
    info->required = true;
  }
  DTO_FIELD(String, deviceId, "device_id");
  
  DTO_FIELD_INFO(sn) {
    info->description = "命令序列号";
  }
  DTO_FIELD(Int32, sn, "sn");

};

/**
 * 目录查询响应DTO
 */
class CatalogResponseDto : public oatpp::DTO {
  
  DTO_INIT(CatalogResponseDto, DTO)
  
  DTO_FIELD_INFO(deviceId) {
    info->description = "设备编号";
  }
  DTO_FIELD(String, deviceId, "device_id");
  
  DTO_FIELD_INFO(sn) {
    info->description = "命令序列号";
  }
  DTO_FIELD(Int32, sn, "sn");
  
  DTO_FIELD_INFO(sumNum) {
    info->description = "通道总数";
  }
  DTO_FIELD(Int32, sumNum, "sum_num");
  
  DTO_FIELD_INFO(channels) {
    info->description = "通道列表";
  }
  DTO_FIELD(Vector<Object<ChannelDto>>, channels);

};

/**
 * 目录订阅请求DTO
 */
class CatalogSubscribeDto : public oatpp::DTO {
  
  DTO_INIT(CatalogSubscribeDto, DTO)
  
  DTO_FIELD_INFO(deviceId) {
    info->description = "设备编号";
    info->required = true;
  }
  DTO_FIELD(String, deviceId, "device_id");
  
  DTO_FIELD_INFO(expires) {
    info->description = "订阅过期时间（秒），0表示取消订阅";
  }
  DTO_FIELD(Int32, expires, "expires") = 3600;

};

#include OATPP_CODEGEN_END(DTO)

#endif /* _C_CATALOG_DTO_HPP */
