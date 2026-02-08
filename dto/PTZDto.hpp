/*
*  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source tree.
*/
/*****************************************************************************
				  Author: chensong
				  date:  2026-02-09
				  
				  PTZ控制数据传输对象
******************************************************************************/

#ifndef _C_PTZ_DTO_HPP
#define _C_PTZ_DTO_HPP

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * PTZ方向枚举
 */
class PTZDirection {
public:
    static constexpr const char* STOP = "STOP";
    static constexpr const char* UP = "UP";
    static constexpr const char* DOWN = "DOWN";
    static constexpr const char* LEFT = "LEFT";
    static constexpr const char* RIGHT = "RIGHT";
    static constexpr const char* LEFT_UP = "LEFT_UP";
    static constexpr const char* LEFT_DOWN = "LEFT_DOWN";
    static constexpr const char* RIGHT_UP = "RIGHT_UP";
    static constexpr const char* RIGHT_DOWN = "RIGHT_DOWN";
};

/**
 * PTZ方向控制请求DTO
 */
class PTZControlDto : public oatpp::DTO {
    DTO_INIT(PTZControlDto, DTO)
    
    DTO_FIELD_INFO(deviceId) {
        info->description = "Device ID";
        info->required = true;
    }
    DTO_FIELD(String, deviceId, "device_id");
    
    DTO_FIELD_INFO(channelId) {
        info->description = "Channel ID";
        info->required = true;
    }
    DTO_FIELD(String, channelId, "channel_id");
    
    DTO_FIELD_INFO(direction) {
        info->description = "PTZ direction (STOP/UP/DOWN/LEFT/RIGHT/LEFT_UP/LEFT_DOWN/RIGHT_UP/RIGHT_DOWN)";
        info->required = true;
    }
    DTO_FIELD(String, direction);
    
    DTO_FIELD_INFO(hSpeed) {
        info->description = "Horizontal speed (0-255)";
        info->required = false;
    }
    DTO_FIELD(Int32, hSpeed, "h_speed") = 128;
    
    DTO_FIELD_INFO(vSpeed) {
        info->description = "Vertical speed (0-255)";
        info->required = false;
    }
    DTO_FIELD(Int32, vSpeed, "v_speed") = 128;
    
    DTO_FIELD_INFO(duration) {
        info->description = "Duration in milliseconds (0 for continuous)";
        info->required = false;
    }
    DTO_FIELD(Int32, duration) = 0;
};

/**
 * PTZ变倍控制请求DTO
 */
class PTZZoomDto : public oatpp::DTO {
    DTO_INIT(PTZZoomDto, DTO)
    
    DTO_FIELD_INFO(deviceId) {
        info->description = "Device ID";
        info->required = true;
    }
    DTO_FIELD(String, deviceId, "device_id");
    
    DTO_FIELD_INFO(channelId) {
        info->description = "Channel ID";
        info->required = true;
    }
    DTO_FIELD(String, channelId, "channel_id");
    
    DTO_FIELD_INFO(zoomIn) {
        info->description = "Zoom in (true) or zoom out (false)";
        info->required = true;
    }
    DTO_FIELD(Boolean, zoomIn, "zoom_in");
    
    DTO_FIELD_INFO(speed) {
        info->description = "Zoom speed (0-255)";
        info->required = false;
    }
    DTO_FIELD(Int32, speed) = 128;
};

/**
 * PTZ变焦控制请求DTO
 */
class PTZFocusDto : public oatpp::DTO {
    DTO_INIT(PTZFocusDto, DTO)
    
    DTO_FIELD_INFO(deviceId) {
        info->description = "Device ID";
        info->required = true;
    }
    DTO_FIELD(String, deviceId, "device_id");
    
    DTO_FIELD_INFO(channelId) {
        info->description = "Channel ID";
        info->required = true;
    }
    DTO_FIELD(String, channelId, "channel_id");
    
    DTO_FIELD_INFO(focusNear) {
        info->description = "Focus near (true) or focus far (false)";
        info->required = true;
    }
    DTO_FIELD(Boolean, focusNear, "focus_near");
    
    DTO_FIELD_INFO(speed) {
        info->description = "Focus speed (0-255)";
        info->required = false;
    }
    DTO_FIELD(Int32, speed) = 128;
};

/**
 * PTZ光圈控制请求DTO
 */
class PTZIrisDto : public oatpp::DTO {
    DTO_INIT(PTZIrisDto, DTO)
    
    DTO_FIELD_INFO(deviceId) {
        info->description = "Device ID";
        info->required = true;
    }
    DTO_FIELD(String, deviceId, "device_id");
    
    DTO_FIELD_INFO(channelId) {
        info->description = "Channel ID";
        info->required = true;
    }
    DTO_FIELD(String, channelId, "channel_id");
    
    DTO_FIELD_INFO(irisOpen) {
        info->description = "Iris open (true) or iris close (false)";
        info->required = true;
    }
    DTO_FIELD(Boolean, irisOpen, "iris_open");
    
    DTO_FIELD_INFO(speed) {
        info->description = "Iris speed (0-255)";
        info->required = false;
    }
    DTO_FIELD(Int32, speed) = 128;
};

/**
 * PTZ预置位操作请求DTO
 */
class PTZPresetDto : public oatpp::DTO {
    DTO_INIT(PTZPresetDto, DTO)
    
    DTO_FIELD_INFO(deviceId) {
        info->description = "Device ID";
        info->required = true;
    }
    DTO_FIELD(String, deviceId, "device_id");
    
    DTO_FIELD_INFO(channelId) {
        info->description = "Channel ID";
        info->required = true;
    }
    DTO_FIELD(String, channelId, "channel_id");
    
    DTO_FIELD_INFO(presetId) {
        info->description = "Preset ID (1-255)";
        info->required = true;
    }
    DTO_FIELD(Int32, presetId, "preset_id");
    
    DTO_FIELD_INFO(presetName) {
        info->description = "Preset name (for set operation)";
        info->required = false;
    }
    DTO_FIELD(String, presetName, "preset_name");
};

/**
 * PTZ巡航控制请求DTO
 */
class PTZCruiseDto : public oatpp::DTO {
    DTO_INIT(PTZCruiseDto, DTO)
    
    DTO_FIELD_INFO(deviceId) {
        info->description = "Device ID";
        info->required = true;
    }
    DTO_FIELD(String, deviceId, "device_id");
    
    DTO_FIELD_INFO(channelId) {
        info->description = "Channel ID";
        info->required = true;
    }
    DTO_FIELD(String, channelId, "channel_id");
    
    DTO_FIELD_INFO(cruiseId) {
        info->description = "Cruise group ID (1-255)";
        info->required = true;
    }
    DTO_FIELD(Int32, cruiseId, "cruise_id");
    
    DTO_FIELD_INFO(speed) {
        info->description = "Cruise speed (0-255)";
        info->required = false;
    }
    DTO_FIELD(Int32, speed) = 128;
};

/**
 * PTZ看守位设置请求DTO
 */
class PTZGuardDto : public oatpp::DTO {
    DTO_INIT(PTZGuardDto, DTO)
    
    DTO_FIELD_INFO(deviceId) {
        info->description = "Device ID";
        info->required = true;
    }
    DTO_FIELD(String, deviceId, "device_id");
    
    DTO_FIELD_INFO(channelId) {
        info->description = "Channel ID";
        info->required = true;
    }
    DTO_FIELD(String, channelId, "channel_id");
    
    DTO_FIELD_INFO(enabled) {
        info->description = "Enable guard position";
        info->required = true;
    }
    DTO_FIELD(Boolean, enabled);
    
    DTO_FIELD_INFO(timeout) {
        info->description = "Timeout in seconds before returning to guard position";
        info->required = false;
    }
    DTO_FIELD(Int32, timeout) = 60;
};

/**
 * PTZ控制响应DTO
 */
class PTZResponseDto : public oatpp::DTO {
    DTO_INIT(PTZResponseDto, DTO)
    
    DTO_FIELD_INFO(code) {
        info->description = "Response code";
    }
    DTO_FIELD(Int32, code);
    
    DTO_FIELD_INFO(status) {
        info->description = "Response status (OK/ERROR)";
    }
    DTO_FIELD(String, status);
    
    DTO_FIELD_INFO(message) {
        info->description = "Response message";
    }
    DTO_FIELD(String, message);
};

#include OATPP_CODEGEN_END(DTO)

#endif /* _C_PTZ_DTO_HPP */
