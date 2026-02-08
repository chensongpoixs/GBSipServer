/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Author: chensong
 *  Date: 2026-02-09
 *  Description: 录像管理相关DTO定义
 ******************************************************************************/

#ifndef GBSIPSERVER_DTO_RECORDDTO_HPP
#define GBSIPSERVER_DTO_RECORDDTO_HPP

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

namespace gbsip_server {

/**
 * 录像文件信息DTO
 */
class RecordItemDto : public oatpp::DTO {
    DTO_INIT(RecordItemDto, DTO)
    
    DTO_FIELD(Int32, id);                      // 记录ID
    DTO_FIELD(String, device_id);              // 设备ID
    DTO_FIELD(String, channel_id);             // 通道ID
    DTO_FIELD(String, name);                   // 录像名称
    DTO_FIELD(String, file_path);              // 文件路径
    DTO_FIELD(String, address);                // 存储地址
    DTO_FIELD(String, start_time);             // 开始时间
    DTO_FIELD(String, end_time);               // 结束时间
    DTO_FIELD(Int32, secrecy) = 0;             // 保密属性
    DTO_FIELD(String, type);                   // 录像类型
    DTO_FIELD(String, recorder_id);            // 录像机ID
    DTO_FIELD(Int64, file_size);               // 文件大小
};

/**
 * 录像查询请求DTO
 */
class RecordQueryDto : public oatpp::DTO {
    DTO_INIT(RecordQueryDto, DTO)
    
    DTO_FIELD(String, device_id);              // 设备ID
    DTO_FIELD(String, channel_id);             // 通道ID
    DTO_FIELD(String, start_time);             // 开始时间
    DTO_FIELD(String, end_time);               // 结束时间
    DTO_FIELD(String, type) = "all";           // 录像类型(all/time/alarm/manual)
};

/**
 * 录像列表响应DTO
 */
class RecordListDto : public oatpp::DTO {
    DTO_INIT(RecordListDto, DTO)
    
    DTO_FIELD(Int32, code);                    // 响应码
    DTO_FIELD(String, status);                 // 状态
    DTO_FIELD(String, message);                // 消息
    DTO_FIELD(Int32, sum_num);                 // 总数
    DTO_FIELD(Vector<Object<RecordItemDto>>, records);  // 录像列表
};

/**
 * 回放开始请求DTO
 */
class PlaybackStartDto : public oatpp::DTO {
    DTO_INIT(PlaybackStartDto, DTO)
    
    DTO_FIELD(String, device_id);              // 设备ID
    DTO_FIELD(String, channel_id);             // 通道ID
    DTO_FIELD(String, start_time);             // 开始时间
    DTO_FIELD(String, end_time);               // 结束时间
    DTO_FIELD(String, local_ip);               // 本地IP
    DTO_FIELD(Int32, local_port);              // 本地端口
};

/**
 * 回放控制请求DTO
 */
class PlaybackControlDto : public oatpp::DTO {
    DTO_INIT(PlaybackControlDto, DTO)
    
    DTO_FIELD(String, device_id);              // 设备ID
    DTO_FIELD(String, channel_id);             // 通道ID
    DTO_FIELD(String, call_id);                // 会话ID
};

/**
 * 回放跳转请求DTO
 */
class PlaybackSeekDto : public oatpp::DTO {
    DTO_INIT(PlaybackSeekDto, DTO)
    
    DTO_FIELD(String, device_id);              // 设备ID
    DTO_FIELD(String, channel_id);             // 通道ID
    DTO_FIELD(String, call_id);                // 会话ID
    DTO_FIELD(String, seek_time);              // 跳转时间
};

/**
 * 回放速度请求DTO
 */
class PlaybackSpeedDto : public oatpp::DTO {
    DTO_INIT(PlaybackSpeedDto, DTO)
    
    DTO_FIELD(String, device_id);              // 设备ID
    DTO_FIELD(String, channel_id);             // 通道ID
    DTO_FIELD(String, call_id);                // 会话ID
    DTO_FIELD(Float32, speed) = 1.0f;          // 播放速度
};

/**
 * 下载开始请求DTO
 */
class DownloadStartDto : public oatpp::DTO {
    DTO_INIT(DownloadStartDto, DTO)
    
    DTO_FIELD(String, device_id);              // 设备ID
    DTO_FIELD(String, channel_id);             // 通道ID
    DTO_FIELD(String, start_time);             // 开始时间
    DTO_FIELD(String, end_time);               // 结束时间
    DTO_FIELD(String, local_ip);               // 本地IP
    DTO_FIELD(Int32, local_port);              // 本地端口
};

/**
 * 通用响应DTO
 */
class RecordResponseDto : public oatpp::DTO {
    DTO_INIT(RecordResponseDto, DTO)
    
    DTO_FIELD(Int32, code);                    // 响应码
    DTO_FIELD(String, status);                 // 状态
    DTO_FIELD(String, message);                // 消息
    DTO_FIELD(String, call_id);                // 会话ID(可选)
    DTO_FIELD(String, sdp);                    // SDP信息(可选)
};

} // namespace gbsip_server

#include OATPP_CODEGEN_END(DTO)

#endif // GBSIPSERVER_DTO_RECORDDTO_HPP
