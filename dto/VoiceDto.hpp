/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Author: chensong
 *  Date: 2026-02-09
 *  Description: 语音对讲DTO定义
 ******************************************************************************/

#ifndef GBSIPSERVER_DTO_VOICEDTO_HPP
#define GBSIPSERVER_DTO_VOICEDTO_HPP

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

namespace gbsip_server {

/**
 * 语音广播开始请求DTO
 */
class VoiceBroadcastStartDto : public oatpp::DTO {
    DTO_INIT(VoiceBroadcastStartDto, DTO)
    
    DTO_FIELD(String, device_id, "device_id");      // 设备ID
    DTO_FIELD(String, local_ip, "local_ip");        // 本地IP
    DTO_FIELD(Int32, local_port, "local_port");     // 本地端口
};

/**
 * 双向对讲开始请求DTO
 */
class VoiceTalkStartDto : public oatpp::DTO {
    DTO_INIT(VoiceTalkStartDto, DTO)
    
    DTO_FIELD(String, device_id, "device_id");      // 设备ID
    DTO_FIELD(String, local_ip, "local_ip");        // 本地IP
    DTO_FIELD(Int32, local_port, "local_port");     // 本地端口
};

/**
 * 语音控制请求DTO
 */
class VoiceControlDto : public oatpp::DTO {
    DTO_INIT(VoiceControlDto, DTO)
    
    DTO_FIELD(String, call_id, "call_id");          // 会话ID
};

/**
 * 语音响应DTO
 */
class VoiceResponseDto : public oatpp::DTO {
    DTO_INIT(VoiceResponseDto, DTO)
    
    DTO_FIELD(Int32, code, "code");                 // 响应码
    DTO_FIELD(String, status, "status");            // 状态
    DTO_FIELD(String, message, "message");          // 消息
    DTO_FIELD(String, call_id, "call_id");          // 会话ID
    DTO_FIELD(String, sdp, "sdp");                  // SDP信息
};

} // namespace gbsip_server

#include OATPP_CODEGEN_END(DTO)

#endif // GBSIPSERVER_DTO_VOICEDTO_HPP
