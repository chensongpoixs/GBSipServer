/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Author: chensong
 *  Date: 2026-02-09
 *  Description: 语音对讲服务层
 ******************************************************************************/

#ifndef GBSIPSERVER_SERVICE_VOICESERVICE_HPP
#define GBSIPSERVER_SERVICE_VOICESERVICE_HPP

#include "dto/VoiceDto.hpp"
#include "db/DeviceDb.hpp"
#include "sip/VoiceHandler.hpp"
#include "oatpp/core/macro/component.hpp"
#include <memory>

namespace gbsip_server {

/**
 * 语音对讲服务
 * 负责语音对讲的业务逻辑
 */
class VoiceService {
public:
    /**
     * 构造函数
     */
    VoiceService(std::shared_ptr<VoiceHandler> voiceHandler);

    /**
     * 析构函数
     */
    ~VoiceService();

    /**
     * 开始语音广播
     */
    oatpp::Object<VoiceResponseDto> startBroadcast(
        const oatpp::Object<VoiceBroadcastStartDto>& dto);

    /**
     * 停止语音广播
     */
    oatpp::Object<VoiceResponseDto> stopBroadcast(
        const oatpp::Object<VoiceControlDto>& dto);

    /**
     * 开始双向对讲
     */
    oatpp::Object<VoiceResponseDto> startTalk(
        const oatpp::Object<VoiceTalkStartDto>& dto);

    /**
     * 停止双向对讲
     */
    oatpp::Object<VoiceResponseDto> stopTalk(
        const oatpp::Object<VoiceControlDto>& dto);

private:
    /**
     * 创建成功响应
     */
    oatpp::Object<VoiceResponseDto> createSuccessResponse(
        const std::string& message, 
        const std::string& callId = "",
        const std::string& sdp = "");

    /**
     * 创建失败响应
     */
    oatpp::Object<VoiceResponseDto> createFailureResponse(
        const std::string& message, int code = 500);

private:
    OATPP_COMPONENT(std::shared_ptr<DeviceDb>, device_db_);
    std::shared_ptr<VoiceHandler> voice_handler_;
};

} // namespace gbsip_server

#endif // GBSIPSERVER_SERVICE_VOICESERVICE_HPP
