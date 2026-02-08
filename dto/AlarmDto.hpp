/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Author: chensong
 *  Date: 2026-02-09
 *  Description: 报警管理相关DTO定义
 ******************************************************************************/

#ifndef GBSIPSERVER_DTO_ALARMDTO_HPP
#define GBSIPSERVER_DTO_ALARMDTO_HPP

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

namespace gbsip_server {

/**
 * 报警信息DTO
 */
class AlarmItemDto : public oatpp::DTO {
    DTO_INIT(AlarmItemDto, DTO)
    
    DTO_FIELD(Int32, id);                      // 记录ID
    DTO_FIELD(String, device_id);              // 设备ID
    DTO_FIELD(String, channel_id);             // 通道ID
    DTO_FIELD(Int32, alarm_priority);          // 报警级别(1-4)
    DTO_FIELD(Int32, alarm_method);            // 报警方式
    DTO_FIELD(String, alarm_time);             // 报警时间
    DTO_FIELD(String, alarm_description);      // 报警描述
    DTO_FIELD(Float64, longitude);             // 经度
    DTO_FIELD(Float64, latitude);              // 纬度
    DTO_FIELD(Int32, status) = 0;              // 状态(0-未处理,1-已确认,2-已复位)
    DTO_FIELD(String, confirmed_at);           // 确认时间
    DTO_FIELD(String, reset_at);               // 复位时间
};

/**
 * 报警订阅请求DTO
 */
class AlarmSubscribeDto : public oatpp::DTO {
    DTO_INIT(AlarmSubscribeDto, DTO)
    
    DTO_FIELD(String, device_id);              // 设备ID
    DTO_FIELD(Int32, start_priority) = 1;      // 起始报警级别
    DTO_FIELD(Int32, end_priority) = 4;        // 结束报警级别
    DTO_FIELD(Int32, alarm_method) = 1;        // 报警方式
    DTO_FIELD(Int32, expires) = 3600;          // 订阅有效期(秒)
};

/**
 * 报警查询请求DTO
 */
class AlarmQueryDto : public oatpp::DTO {
    DTO_INIT(AlarmQueryDto, DTO)
    
    DTO_FIELD(String, device_id);              // 设备ID
    DTO_FIELD(String, start_time);             // 开始时间
    DTO_FIELD(String, end_time);               // 结束时间
    DTO_FIELD(Int32, start_priority) = 1;      // 起始报警级别
    DTO_FIELD(Int32, end_priority) = 4;        // 结束报警级别
    DTO_FIELD(Int32, alarm_method);            // 报警方式
};

/**
 * 报警列表响应DTO
 */
class AlarmListDto : public oatpp::DTO {
    DTO_INIT(AlarmListDto, DTO)
    
    DTO_FIELD(Int32, code);                    // 响应码
    DTO_FIELD(String, status);                 // 状态
    DTO_FIELD(String, message);                // 消息
    DTO_FIELD(Int32, sum_num);                 // 总数
    DTO_FIELD(Vector<Object<AlarmItemDto>>, alarms);  // 报警列表
};

/**
 * 报警通知DTO
 */
class AlarmNotifyDto : public oatpp::DTO {
    DTO_INIT(AlarmNotifyDto, DTO)
    
    DTO_FIELD(String, device_id);              // 设备ID
    DTO_FIELD(Int32, alarm_priority);          // 报警级别
    DTO_FIELD(Int32, alarm_method);            // 报警方式
    DTO_FIELD(String, alarm_time);             // 报警时间
    DTO_FIELD(String, alarm_description);      // 报警描述
    DTO_FIELD(Float64, longitude);             // 经度
    DTO_FIELD(Float64, latitude);              // 纬度
};

/**
 * 报警确认请求DTO
 */
class AlarmConfirmDto : public oatpp::DTO {
    DTO_INIT(AlarmConfirmDto, DTO)
    
    DTO_FIELD(Int32, alarm_id);                // 报警ID
};

/**
 * 报警复位请求DTO
 */
class AlarmResetDto : public oatpp::DTO {
    DTO_INIT(AlarmResetDto, DTO)
    
    DTO_FIELD(Int32, alarm_id);                // 报警ID
};

/**
 * 通用响应DTO
 */
class AlarmResponseDto : public oatpp::DTO {
    DTO_INIT(AlarmResponseDto, DTO)
    
    DTO_FIELD(Int32, code);                    // 响应码
    DTO_FIELD(String, status);                 // 状态
    DTO_FIELD(String, message);                // 消息
    DTO_FIELD(String, dialog_id);              // 对话ID(可选)
};

} // namespace gbsip_server

#include OATPP_CODEGEN_END(DTO)

#endif // GBSIPSERVER_DTO_ALARMDTO_HPP
