/*
*  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source tree.
*/
/*****************************************************************************
				  Author: chensong
				  date:  2026-02-09
				  
				  设备信息服务类
******************************************************************************/

#ifndef _C_DEVICE_INFO_SERVICE_HPP
#define _C_DEVICE_INFO_SERVICE_HPP

#include "db/DeviceDb.hpp"
#include "dto/DeviceInfoDto.hpp"
#include "dto/StatusDto.hpp"
#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/macro/component.hpp"

/**
 * 设备信息服务类
 * 处理设备信息查询、设备状态查询等业务逻辑
 */
class DeviceInfoService {
private:
    typedef oatpp::web::protocol::http::Status Status;
    
private:
    OATPP_COMPONENT(std::shared_ptr<DeviceDb>, m_deviceDb);
    
public:
    /**
     * 查询设备信息
     * @param deviceId 设备ID
     * @return 设备信息DTO
     */
    oatpp::Object<DeviceInfoDto> queryDeviceInfo(const oatpp::String& deviceId);
    
    /**
     * 查询设备状态
     * @param deviceId 设备ID
     * @return 设备状态DTO
     */
    oatpp::Object<DeviceStatusDto> queryDeviceStatus(const oatpp::String& deviceId);
    
    /**
     * 查询设备配置（预留）
     * @param deviceId 设备ID
     * @return 状态DTO
     */
    oatpp::Object<StatusDto> queryDeviceConfig(const oatpp::String& deviceId);

private:
    /**
     * 获取当前时间戳
     */
    oatpp::Int64 getCurrentTimestamp();
};

#endif /* _C_DEVICE_INFO_SERVICE_HPP */
