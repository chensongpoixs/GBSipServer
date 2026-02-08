# Sprint 2: 设备信息查询实现

## 一、功能概述

### 1.1 目标功能
实现GB28181标准中的设备信息查询功能，包括：
- 设备基本信息查询（DeviceInfo）
- 设备状态查询（DeviceStatus）
- 设备配置查询（ConfigDownload）

### 1.2 涉及的SIP消息

#### 1.2.1 设备信息查询（DeviceInfo）
**请求XML**:
```xml
<?xml version="1.0" encoding="GB2312"?>
<Query>
    <CmdType>DeviceInfo</CmdType>
    <SN>123</SN>
    <DeviceID>34020000001320000001</DeviceID>
</Query>
```

**响应XML**:
```xml
<?xml version="1.0" encoding="GB2312"?>
<Response>
    <CmdType>DeviceInfo</CmdType>
    <SN>123</SN>
    <DeviceID>34020000001320000001</DeviceID>
    <DeviceName>IPC Camera</DeviceName>
    <Manufacturer>Hikvision</Manufacturer>
    <Model>DS-2CD2T47EWD-L</Model>
    <Firmware>V5.5.0</Firmware>
    <Channel>1</Channel>
</Response>
```

#### 1.2.2 设备状态查询（DeviceStatus）
**请求XML**:
```xml
<?xml version="1.0" encoding="GB2312"?>
<Query>
    <CmdType>DeviceStatus</CmdType>
    <SN>124</SN>
    <DeviceID>34020000001320000001</DeviceID>
</Query>
```

**响应XML**:
```xml
<?xml version="1.0" encoding="GB2312"?>
<Response>
    <CmdType>DeviceStatus</CmdType>
    <SN>124</SN>
    <DeviceID>34020000001320000001</DeviceID>
    <Online>ONLINE</Online>
    <Status>OK</Status>
    <Encode>ON</Encode>
    <Record>OFF</Record>
    <DeviceTime>2026-02-09T10:30:00</DeviceTime>
</Response>
```

## 二、实现架构

### 2.1 类设计

```
DeviceInfoHandler (继承 SipMessageHandler)
    ├── queryDeviceInfo()      - 查询设备信息
    ├── queryDeviceStatus()    - 查询设备状态
    ├── handleDeviceInfoResponse()   - 处理设备信息响应
    └── handleDeviceStatusResponse() - 处理设备状态响应
```

### 2.2 数据结构

```cpp
// 设备信息结构
struct DeviceInfo {
    std::string deviceId;
    std::string deviceName;
    std::string manufacturer;
    std::string model;
    std::string firmware;
    int channel;
};

// 设备状态结构
struct DeviceStatus {
    std::string deviceId;
    std::string online;      // ONLINE/OFFLINE
    std::string status;      // OK/ERROR
    std::string encode;      // ON/OFF
    std::string record;      // ON/OFF
    std::string deviceTime;
};
```

### 2.3 API接口

1. **GET** `/api/v1/device/{deviceId}/info` - 查询设备信息
2. **GET** `/api/v1/device/{deviceId}/status` - 查询设备状态
3. **GET** `/api/v1/device/{deviceId}/config` - 查询设备配置（预留）

## 三、实现步骤

### 步骤1: 创建DTO
- DeviceInfoDto.hpp
- DeviceStatusDto.hpp

### 步骤2: 创建Handler
- DeviceInfoHandler.hpp
- DeviceInfoHandler.cpp

### 步骤3: 创建Service
- DeviceInfoService.hpp
- DeviceInfoService.cpp

### 步骤4: 创建Controller
- DeviceInfoController.hpp

### 步骤5: 集成到SipServer
- 修改 sip_server.h
- 修改 sip_server.cpp

### 步骤6: 测试
- 单元测试
- 集成测试

## 四、预计工作量

- DTO创建: 0.5小时
- Handler实现: 2小时
- Service实现: 1小时
- Controller实现: 0.5小时
- 集成: 1小时
- 测试: 1小时

**总计**: 6小时（约1天）

---

**开始实现时间**: 2026-02-09
