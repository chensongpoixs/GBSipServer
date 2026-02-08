/*
*  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source tree.
*/
/*****************************************************************************
				  Author: chensong
				  date:  2026-02-09
				  
				  设备信息数据传输对象
******************************************************************************/

#ifndef _C_DEVICE_INFO_DTO_HPP
#define _C_DEVICE_INFO_DTO_HPP

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * 设备信息DTO
 */
class DeviceInfoDto : public oatpp::DTO {
  
  DTO_INIT(DeviceInfoDto, DTO)
  
  DTO_FIELD_INFO(deviceId) {
    info->description = "设备编号";
  }
  DTO_FIELD(String, deviceId, "device_id");
  
  DTO_FIELD_INFO(deviceName) {
    info->description = "设备名称";
  }
  DTO_FIELD(String, deviceName, "device_name");
  
  DTO_FIELD_INFO(manufacturer) {
    info->description = "厂商名称";
  }
  DTO_FIELD(String, manufacturer, "manufacturer");
  
  DTO_FIELD_INFO(model) {
    info->description = "设备型号";
  }
  DTO_FIELD(String, model, "model");
  
  DTO_FIELD_INFO(firmware) {
    info->description = "固件版本";
  }
  DTO_FIELD(String, firmware, "firmware");
  
  DTO_FIELD_INFO(channel) {
    info->description = "通道数";
  }
  DTO_FIELD(Int32, channel, "channel");
  
  DTO_FIELD_INFO(sn) {
    info->description = "序列号";
  }
  DTO_FIELD(Int32, sn, "sn");

};

/**
 * 设备状态DTO
 */
class DeviceStatusDto : public oatpp::DTO {
  
  DTO_INIT(DeviceStatusDto, DTO)
  
  DTO_FIELD_INFO(deviceId) {
    info->description = "设备编号";
  }
  DTO_FIELD(String, deviceId, "device_id");
  
  DTO_FIELD_INFO(online) {
    info->description = "在线状态（ONLINE/OFFLINE）";
  }
  DTO_FIELD(String, online, "online");
  
  DTO_FIELD_INFO(status) {
    info->description = "设备状态（OK/ERROR）";
  }
  DTO_FIELD(String, status, "status");
  
  DTO_FIELD_INFO(encode) {
    info->description = "编码状态（ON/OFF）";
  }
  DTO_FIELD(String, encode, "encode");
  
  DTO_FIELD_INFO(record) {
    info->description = "录像状态（ON/OFF）";
  }
  DTO_FIELD(String, record, "record");
  
  DTO_FIELD_INFO(deviceTime) {
    info->description = "设备时间";
  }
  DTO_FIELD(String, deviceTime, "device_time");
  
  DTO_FIELD_INFO(sn) {
    info->description = "序列号";
  }
  DTO_FIELD(Int32, sn, "sn");

};

#include OATPP_CODEGEN_END(DTO)

#endif /* _C_DEVICE_INFO_DTO_HPP */
