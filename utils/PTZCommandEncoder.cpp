/*
*  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source tree.
*/
/*****************************************************************************
				  Author: chensong
				  date:  2026-02-09
				  
				  GB28181 PTZ命令编码器实现
******************************************************************************/

#include "PTZCommandEncoder.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace gbsip_server {

std::string PTZCommandEncoder::encodeDirection(Direction dir, int hSpeed, int vSpeed) {
    uint8_t cmd1 = 0;
    uint8_t cmd2 = 0;
    
    // 转换速度值 (0-255 -> 0-15)
    uint8_t h = convertSpeed(hSpeed);
    uint8_t v = convertSpeed(vSpeed);
    
    // 根据方向设置命令字节
    switch (dir) {
        case STOP:
            cmd1 = 0x80 | h;  // 停止
            cmd2 = v << 4;
            break;
        case UP:
            cmd1 = 0x90 | h;  // 上
            cmd2 = v << 4;
            break;
        case DOWN:
            cmd1 = 0xA0 | h;  // 下
            cmd2 = v << 4;
            break;
        case LEFT:
            cmd1 = 0xB0 | h;  // 左
            cmd2 = v << 4;
            break;
        case RIGHT:
            cmd1 = 0xC0 | h;  // 右
            cmd2 = v << 4;
            break;
        case LEFT_UP:
            cmd1 = 0xD0 | h;  // 左上
            cmd2 = v << 4;
            break;
        case LEFT_DOWN:
            cmd1 = 0xE0 | h;  // 左下
            cmd2 = v << 4;
            break;
        case RIGHT_UP:
            cmd1 = 0xF0 | h;  // 右上
            cmd2 = v << 4;
            break;
        case RIGHT_DOWN:
            cmd1 = 0x10 | h;  // 右下
            cmd2 = v << 4;
            break;
        default:
            cmd1 = 0x80;  // 默认停止
            cmd2 = 0x00;
            break;
    }
    
    return buildCommand(cmd1, cmd2, 0x00, 0x00, 0x00, 0x00);
}

std::string PTZCommandEncoder::encodeZoom(bool zoomIn, int speed) {
    uint8_t z = convertSpeed(speed);
    uint8_t cmd1 = 0x10;  // 变倍命令
    uint8_t cmd2 = zoomIn ? (z << 4) : (0x00 | z);  // 高4位=放大，低4位=缩小
    
    return buildCommand(cmd1, cmd2, 0x00, 0x00, 0x00, 0x00);
}

std::string PTZCommandEncoder::encodeFocus(bool focusNear, int speed) {
    uint8_t f = convertSpeed(speed);
    uint8_t cmd1 = 0x20;  // 变焦命令
    uint8_t cmd2 = focusNear ? (f << 4) : (0x00 | f);  // 高4位=近焦，低4位=远焦
    
    return buildCommand(cmd1, cmd2, 0x00, 0x00, 0x00, 0x00);
}

std::string PTZCommandEncoder::encodeIris(bool irisOpen, int speed) {
    uint8_t i = convertSpeed(speed);
    uint8_t cmd1 = 0x30;  // 光圈命令
    uint8_t cmd2 = irisOpen ? (i << 4) : (0x00 | i);  // 高4位=开大，低4位=关小
    
    return buildCommand(cmd1, cmd2, 0x00, 0x00, 0x00, 0x00);
}

std::string PTZCommandEncoder::encodePresetSet(int presetId) {
    uint8_t cmd1 = 0x81;  // 设置预置位
    uint8_t cmd2 = 0x00;
    uint8_t data1 = (presetId >> 8) & 0xFF;  // 预置位号高字节
    uint8_t data2 = presetId & 0xFF;         // 预置位号低字节
    
    return buildCommand(cmd1, cmd2, data1, data2, 0x00, 0x00);
}

std::string PTZCommandEncoder::encodePresetCall(int presetId) {
    uint8_t cmd1 = 0x82;  // 调用预置位
    uint8_t cmd2 = 0x00;
    uint8_t data1 = (presetId >> 8) & 0xFF;
    uint8_t data2 = presetId & 0xFF;
    
    return buildCommand(cmd1, cmd2, data1, data2, 0x00, 0x00);
}

std::string PTZCommandEncoder::encodePresetDelete(int presetId) {
    uint8_t cmd1 = 0x83;  // 删除预置位
    uint8_t cmd2 = 0x00;
    uint8_t data1 = (presetId >> 8) & 0xFF;
    uint8_t data2 = presetId & 0xFF;
    
    return buildCommand(cmd1, cmd2, data1, data2, 0x00, 0x00);
}

std::string PTZCommandEncoder::encodeCruiseStart(int cruiseId, int speed) {
    uint8_t s = convertSpeed(speed);
    uint8_t cmd1 = 0x84;  // 启动巡航
    uint8_t cmd2 = s;
    uint8_t data3 = cruiseId & 0xFF;  // 巡航组号
    
    return buildCommand(cmd1, cmd2, 0x00, 0x00, data3, 0x00);
}

std::string PTZCommandEncoder::encodeCruiseStop(int cruiseId) {
    uint8_t cmd1 = 0x85;  // 停止巡航
    uint8_t cmd2 = 0x00;
    uint8_t data3 = cruiseId & 0xFF;
    
    return buildCommand(cmd1, cmd2, 0x00, 0x00, data3, 0x00);
}

std::string PTZCommandEncoder::encodeGuard(bool enabled, int timeout) {
    uint8_t cmd1 = enabled ? 0x86 : 0x87;  // 0x86=启用看守位，0x87=禁用看守位
    uint8_t cmd2 = 0x00;
    uint8_t data1 = (timeout >> 8) & 0xFF;  // 超时时间高字节
    uint8_t data2 = timeout & 0xFF;         // 超时时间低字节
    
    return buildCommand(cmd1, cmd2, data1, data2, 0x00, 0x00);
}

PTZCommandEncoder::Direction PTZCommandEncoder::stringToDirection(const std::string& dirStr) {
    std::string upper = dirStr;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    
    if (upper == "STOP") return STOP;
    if (upper == "UP") return UP;
    if (upper == "DOWN") return DOWN;
    if (upper == "LEFT") return LEFT;
    if (upper == "RIGHT") return RIGHT;
    if (upper == "LEFT_UP" || upper == "LEFTUP") return LEFT_UP;
    if (upper == "LEFT_DOWN" || upper == "LEFTDOWN") return LEFT_DOWN;
    if (upper == "RIGHT_UP" || upper == "RIGHTUP") return RIGHT_UP;
    if (upper == "RIGHT_DOWN" || upper == "RIGHTDOWN") return RIGHT_DOWN;
    
    return STOP;  // 默认停止
}

std::string PTZCommandEncoder::buildCommand(uint8_t cmd1, uint8_t cmd2,
                                             uint8_t data1, uint8_t data2,
                                             uint8_t data3, uint8_t data4) {
    std::vector<uint8_t> bytes;
    bytes.push_back(0xA5);  // 起始字节
    bytes.push_back(0x0F);  // 地址字节
    bytes.push_back(cmd1);
    bytes.push_back(cmd2);
    bytes.push_back(data1);
    bytes.push_back(data2);
    bytes.push_back(data3);
    bytes.push_back(data4);
    
    // 计算校验字节
    uint8_t checksum = calculateChecksum(bytes);
    bytes.push_back(checksum);
    
    return bytesToHex(bytes);
}

uint8_t PTZCommandEncoder::calculateChecksum(const std::vector<uint8_t>& data) {
    uint8_t checksum = 0;
    for (size_t i = 0; i < data.size(); ++i) {
        checksum ^= data[i];
    }
    return checksum;
}

std::string PTZCommandEncoder::bytesToHex(const std::vector<uint8_t>& data) {
    std::ostringstream oss;
    for (size_t i = 0; i < data.size(); ++i) {
        oss << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
            << static_cast<int>(data[i]);
    }
    return oss.str();
}

uint8_t PTZCommandEncoder::convertSpeed(int speed) {
    // 将0-255的速度值转换为0-15
    if (speed < 0) speed = 0;
    if (speed > 255) speed = 255;
    return static_cast<uint8_t>((speed * 15) / 255);
}

} // namespace gbsip_server
