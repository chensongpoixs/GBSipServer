/*
*  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source tree.
*/
/*****************************************************************************
				  Author: chensong
				  date:  2026-02-09
				  
				  PTZ控制服务
******************************************************************************/

#ifndef _C_PTZ_SERVICE_HPP
#define _C_PTZ_SERVICE_HPP

#include "dto/PTZDto.hpp"
#include "sip/PTZHandler.hpp"
#include "db/DeviceDb.hpp"
#include "oatpp/macro/component.hpp"
#include <memory>

/**
 * PTZ控制服务
 * 提供PTZ控制的业务逻辑
 */
class PTZService {
public:
    /**
     * 构造函数
     */
    PTZService();
    
    /**
     * 析构函数
     */
    ~PTZService();
    
    /**
     * PTZ方向控制
     * @param dto 控制请求
     * @return 响应结果
     */
    oatpp::Object<PTZResponseDto> controlDirection(
        const oatpp::Object<PTZControlDto>& dto);
    
    /**
     * 变倍控制
     * @param dto 控制请求
     * @return 响应结果
     */
    oatpp::Object<PTZResponseDto> controlZoom(
        const oatpp::Object<PTZZoomDto>& dto);
    
    /**
     * 变焦控制
     * @param dto 控制请求
     * @return 响应结果
     */
    oatpp::Object<PTZResponseDto> controlFocus(
        const oatpp::Object<PTZFocusDto>& dto);
    
    /**
     * 光圈控制
     * @param dto 控制请求
     * @return 响应结果
     */
    oatpp::Object<PTZResponseDto> controlIris(
        const oatpp::Object<PTZIrisDto>& dto);
    
    /**
     * 设置预置位
     * @param dto 预置位请求
     * @return 响应结果
     */
    oatpp::Object<PTZResponseDto> setPreset(
        const oatpp::Object<PTZPresetDto>& dto);
    
    /**
     * 调用预置位
     * @param dto 预置位请求
     * @return 响应结果
     */
    oatpp::Object<PTZResponseDto> callPreset(
        const oatpp::Object<PTZPresetDto>& dto);
    
    /**
     * 删除预置位
     * @param dto 预置位请求
     * @return 响应结果
     */
    oatpp::Object<PTZResponseDto> deletePreset(
        const oatpp::Object<PTZPresetDto>& dto);
    
    /**
     * 启动巡航
     * @param dto 巡航请求
     * @return 响应结果
     */
    oatpp::Object<PTZResponseDto> startCruise(
        const oatpp::Object<PTZCruiseDto>& dto);
    
    /**
     * 停止巡航
     * @param dto 巡航请求
     * @return 响应结果
     */
    oatpp::Object<PTZResponseDto> stopCruise(
        const oatpp::Object<PTZCruiseDto>& dto);
    
    /**
     * 设置看守位
     * @param dto 看守位请求
     * @return 响应结果
     */
    oatpp::Object<PTZResponseDto> setGuard(
        const oatpp::Object<PTZGuardDto>& dto);
    
    /**
     * 设置PTZHandler
     * @param handler PTZ处理器
     */
    void setPTZHandler(std::shared_ptr<gbsip_server::PTZHandler> handler);

private:
    /**
     * 创建成功响应
     * @param message 消息
     * @return 响应DTO
     */
    oatpp::Object<PTZResponseDto> createSuccessResponse(const std::string& message);
    
    /**
     * 创建错误响应
     * @param code 错误码
     * @param message 错误消息
     * @return 响应DTO
     */
    oatpp::Object<PTZResponseDto> createErrorResponse(int code, const std::string& message);
    
    /**
     * 获取设备信息
     * @param deviceId 设备ID
     * @param ip 输出参数：设备IP
     * @param port 输出参数：设备端口
     * @return true=成功，false=失败
     */
    bool getDeviceInfo(const std::string& deviceId, std::string& ip, int& port);

private:
    OATPP_COMPONENT(std::shared_ptr<DeviceDb>, device_db_);
    std::shared_ptr<gbsip_server::PTZHandler> ptz_handler_;
};

#endif /* _C_PTZ_SERVICE_HPP */
