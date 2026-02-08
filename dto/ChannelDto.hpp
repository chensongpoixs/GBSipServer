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
				  
				  GB28181 通道数据传输对象
******************************************************************************/

#ifndef _C_CHANNEL_DTO_HPP
#define _C_CHANNEL_DTO_HPP

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * 通道信息DTO
 * 对应GB28181标准中的设备通道信息
 */
class ChannelDto : public oatpp::DTO {
  
  DTO_INIT(ChannelDto, DTO)
  
  DTO_FIELD_INFO(id) {
    info->description = "通道数据库ID";
  }
  DTO_FIELD(Int32, id);
  
  DTO_FIELD_INFO(channelId) {
    info->description = "通道编号（20位国标编码）";
    info->required = true;
  }
  DTO_FIELD(String, channelId, "channel_id");
  
  DTO_FIELD_INFO(deviceId) {
    info->description = "所属设备编号";
    info->required = true;
  }
  DTO_FIELD(String, deviceId, "device_id");
  
  DTO_FIELD_INFO(name) {
    info->description = "通道名称";
  }
  DTO_FIELD(String, name, "name");
  
  DTO_FIELD_INFO(manufacturer) {
    info->description = "厂商名称";
  }
  DTO_FIELD(String, manufacturer, "manufacturer");
  
  DTO_FIELD_INFO(model) {
    info->description = "设备型号";
  }
  DTO_FIELD(String, model, "model");
  
  DTO_FIELD_INFO(owner) {
    info->description = "设备归属";
  }
  DTO_FIELD(String, owner, "owner");
  
  DTO_FIELD_INFO(civilCode) {
    info->description = "行政区划代码";
  }
  DTO_FIELD(String, civilCode, "civil_code");
  
  DTO_FIELD_INFO(address) {
    info->description = "安装地址";
  }
  DTO_FIELD(String, address, "address");
  
  DTO_FIELD_INFO(parental) {
    info->description = "是否有子设备（0-无，1-有）";
  }
  DTO_FIELD(Int32, parental, "parental") = 0;
  
  DTO_FIELD_INFO(parentId) {
    info->description = "父设备/区域/系统ID";
  }
  DTO_FIELD(String, parentId, "parent_id");
  
  DTO_FIELD_INFO(safetyWay) {
    info->description = "信令安全模式（0-不采用，2-S/MIME，3-S/MIME+数字摘要，4-数字摘要）";
  }
  DTO_FIELD(Int32, safetyWay, "safety_way") = 0;
  
  DTO_FIELD_INFO(registerWay) {
    info->description = "注册方式（1-符合IETF RFC 3261，0-符合IETF RFC 2543）";
  }
  DTO_FIELD(Int32, registerWay, "register_way") = 1;
  
  DTO_FIELD_INFO(secrecy) {
    info->description = "保密属性（0-不涉密，1-涉密）";
  }
  DTO_FIELD(Int32, secrecy, "secrecy") = 0;
  
  DTO_FIELD_INFO(status) {
    info->description = "通道状态（ON-在线，OFF-离线）";
  }
  DTO_FIELD(String, status, "status") = "OFF";
  
  DTO_FIELD_INFO(longitude) {
    info->description = "经度";
  }
  DTO_FIELD(Float64, longitude, "longitude") = 0.0;
  
  DTO_FIELD_INFO(latitude) {
    info->description = "纬度";
  }
  DTO_FIELD(Float64, latitude, "latitude") = 0.0;
  
  DTO_FIELD_INFO(ptzType) {
    info->description = "云台类型（0-无云台，1-球机，2-半球，3-固定枪机，4-遥控枪机）";
  }
  DTO_FIELD(Int32, ptzType, "ptz_type") = 0;
  
  DTO_FIELD_INFO(createTime) {
    info->description = "创建时间（Unix时间戳）";
  }
  DTO_FIELD(Int64, createTime, "create_time");
  
  DTO_FIELD_INFO(updateTime) {
    info->description = "更新时间（Unix时间戳）";
  }
  DTO_FIELD(Int64, updateTime, "update_time");

};

/**
 * 通道列表DTO（分页）
 */
class ChannelPageDto : public oatpp::DTO {
  
  DTO_INIT(ChannelPageDto, DTO)
  
  DTO_FIELD_INFO(offset) {
    info->description = "偏移量";
  }
  DTO_FIELD(UInt32, offset);
  
  DTO_FIELD_INFO(limit) {
    info->description = "每页数量";
  }
  DTO_FIELD(UInt32, limit);
  
  DTO_FIELD_INFO(count) {
    info->description = "总数量";
  }
  DTO_FIELD(UInt32, count);
  
  DTO_FIELD_INFO(items) {
    info->description = "通道列表";
  }
  DTO_FIELD(Vector<Object<ChannelDto>>, items);

};

#include OATPP_CODEGEN_END(DTO)

#endif /* _C_CHANNEL_DTO_HPP */
