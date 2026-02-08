/*
*  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source tree.
*/
/*****************************************************************************
				  Author: chensong
				  date:  2026-02-09
				  
				  GB28181 PTZ命令编码器
******************************************************************************/

#ifndef _C_PTZ_COMMAND_ENCODER_HPP
#define _C_PTZ_COMMAND_ENCODER_HPP

#include <string>
#include <vector>
#include <cstdint>

namespace gbsip_server {

/**
 * GB28181 PTZ命令编码器
 * 
 * PTZ命令格式：A50F [命令字节1] [命令字节2] [数据1] [数据2] [数据3] [数据4] [校验]
 * 
 * 命令字节1（高4位）：控制类型
 *   0x8: 停止
 *   0x9: 上
 *   0xA: 下
 *   0xB: 左
 *   0xC: 右
 *   0xD: 左上
 *   0xE: 左下
 *   0xF: 右上
 *   0x1: 右下
 * 
 * 命令字节1（低4位）：水平速度（0-F）
 * 命令字节2（高4位）：垂直速度（0-F）
 * 命令字节2（低4位）：变倍速度（0-F）
 */
class PTZCommandEncoder {
public:
    /**
     * PTZ方向枚举
     */
    enum Direction {
        STOP = 0,
        UP = 1,
        DOWN = 2,
        LEFT = 3,
        RIGHT = 4,
        LEFT_UP = 5,
        LEFT_DOWN = 6,
        RIGHT_UP = 7,
        RIGHT_DOWN = 8
    };
    
    /**
     * 编码PTZ方向控制命令
     * @param dir 方向
     * @param hSpeed 水平速度 (0-255)
     * @param vSpeed 垂直速度 (0-255)
     * @return 十六进制命令字符串
     */
    static std::string encodeDirection(Direction dir, int hSpeed, int vSpeed);
    
    /**
     * 编码变倍控制命令
     * @param zoomIn true=放大，false=缩小
     * @param speed 速度 (0-255)
     * @return 十六进制命令字符串
     */
    static std::string encodeZoom(bool zoomIn, int speed);
    
    /**
     * 编码变焦控制命令
     * @param focusNear true=近焦，false=远焦
     * @param speed 速度 (0-255)
     * @return 十六进制命令字符串
     */
    static std::string encodeFocus(bool focusNear, int speed);
    
    /**
     * 编码光圈控制命令
     * @param irisOpen true=开大，false=关小
     * @param speed 速度 (0-255)
     * @return 十六进制命令字符串
     */
    static std::string encodeIris(bool irisOpen, int speed);
    
    /**
     * 编码设置预置位命令
     * @param presetId 预置位号 (1-255)
     * @return 十六进制命令字符串
     */
    static std::string encodePresetSet(int presetId);
    
    /**
     * 编码调用预置位命令
     * @param presetId 预置位号 (1-255)
     * @return 十六进制命令字符串
     */
    static std::string encodePresetCall(int presetId);
    
    /**
     * 编码删除预置位命令
     * @param presetId 预置位号 (1-255)
     * @return 十六进制命令字符串
     */
    static std::string encodePresetDelete(int presetId);
    
    /**
     * 编码启动巡航命令
     * @param cruiseId 巡航组号 (1-255)
     * @param speed 巡航速度 (0-255)
     * @return 十六进制命令字符串
     */
    static std::string encodeCruiseStart(int cruiseId, int speed);
    
    /**
     * 编码停止巡航命令
     * @param cruiseId 巡航组号 (1-255)
     * @return 十六进制命令字符串
     */
    static std::string encodeCruiseStop(int cruiseId);
    
    /**
     * 编码设置看守位命令
     * @param enabled true=启用，false=禁用
     * @param timeout 超时时间（秒）
     * @return 十六进制命令字符串
     */
    static std::string encodeGuard(bool enabled, int timeout);
    
    /**
     * 字符串方向转枚举
     * @param dirStr 方向字符串
     * @return 方向枚举
     */
    static Direction stringToDirection(const std::string& dirStr);

private:
    /**
     * 构建PTZ命令
     * @param cmd1 命令字节1
     * @param cmd2 命令字节2
     * @param data1 数据字节1
     * @param data2 数据字节2
     * @param data3 数据字节3
     * @param data4 数据字节4
     * @return 十六进制命令字符串
     */
    static std::string buildCommand(uint8_t cmd1, uint8_t cmd2,
                                     uint8_t data1, uint8_t data2,
                                     uint8_t data3, uint8_t data4);
    
    /**
     * 计算校验字节
     * @param data 数据字节数组
     * @return 校验字节
     */
    static uint8_t calculateChecksum(const std::vector<uint8_t>& data);
    
    /**
     * 字节数组转十六进制字符串
     * @param data 字节数组
     * @return 十六进制字符串（大写）
     */
    static std::string bytesToHex(const std::vector<uint8_t>& data);
    
    /**
     * 速度值转换（0-255 -> 0-15）
     * @param speed 速度值 (0-255)
     * @return 转换后的速度 (0-15)
     */
    static uint8_t convertSpeed(int speed);
};

} // namespace gbsip_server

#endif /* _C_PTZ_COMMAND_ENCODER_HPP */
