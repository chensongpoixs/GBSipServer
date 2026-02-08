/*
*  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source tree.
*/
/*****************************************************************************
				  Author: chensong
				  date:  2026-02-09
				  
				  目录服务类
******************************************************************************/

#ifndef _C_CATALOG_SERVICE_HPP
#define _C_CATALOG_SERVICE_HPP

#include "db/ChannelDb.hpp"
#include "db/DeviceDb.hpp"
#include "dto/CatalogDto.hpp"
#include "dto/ChannelDto.hpp"
#include "dto/StatusDto.hpp"
#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/macro/component.hpp"

/**
 * 目录服务类
 * 处理设备目录查询、订阅等业务逻辑
 */
class CatalogService {
private:
    typedef oatpp::web::protocol::http::Status Status;
    
private:
    OATPP_COMPONENT(std::shared_ptr<ChannelDb>, m_channelDb);
    OATPP_COMPONENT(std::shared_ptr<DeviceDb>, m_deviceDb);
    
public:
    /**
     * 查询设备目录
     * @param deviceId 设备ID
     * @return 目录响应DTO
     */
    oatpp::Object<CatalogResponseDto> queryCatalog(const oatpp::String& deviceId);
    
    /**
     * 订阅设备目录
     * @param dto 订阅请求DTO
     * @return 状态DTO
     */
    oatpp::Object<StatusDto> subscribeCatalog(const oatpp::Object<CatalogSubscribeDto>& dto);
    
    /**
     * 取消订阅设备目录
     * @param deviceId 设备ID
     * @return 状态DTO
     */
    oatpp::Object<StatusDto> unsubscribeCatalog(const oatpp::String& deviceId);
    
    /**
     * 处理目录通知（设备主动上报）
     * @param deviceId 设备ID
     * @param channels 通道列表
     * @return 状态DTO
     */
    oatpp::Object<StatusDto> handleCatalogNotify(const oatpp::String& deviceId,
                                                   const oatpp::Vector<oatpp::Object<ChannelDto>>& channels);
    
    /**
     * 获取设备的通道列表
     * @param deviceId 设备ID
     * @return 通道列表
     */
    oatpp::Vector<oatpp::Object<ChannelDto>> getChannelsByDeviceId(const oatpp::String& deviceId);
    
    /**
     * 获取单个通道信息
     * @param channelId 通道ID
     * @return 通道DTO
     */
    oatpp::Object<ChannelDto> getChannelById(const oatpp::String& channelId);
    
    /**
     * 更新通道信息
     * @param channel 通道DTO
     * @return 状态DTO
     */
    oatpp::Object<StatusDto> updateChannel(const oatpp::Object<ChannelDto>& channel);
    
    /**
     * 删除通道
     * @param channelId 通道ID
     * @return 状态DTO
     */
    oatpp::Object<StatusDto> deleteChannel(const oatpp::String& channelId);
    
private:
    /**
     * 保存或更新通道信息
     * @param channel 通道DTO
     */
    void saveOrUpdateChannel(const oatpp::Object<ChannelDto>& channel);
    
    /**
     * 获取当前时间戳
     */
    oatpp::Int64 getCurrentTimestamp();
};

#endif /* _C_CATALOG_SERVICE_HPP */
