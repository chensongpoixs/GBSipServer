# Sprint 6: 语音对讲功能实现计划

## 📊 Sprint概览

**Sprint名称**: 语音对讲  
**优先级**: 🟡 P1（中优先级）  
**预计工作量**: 2周  
**复杂度**: ⭐⭐⭐⭐ (高)  
**依赖**: Sprint 1（设备目录管理）、Sprint 2（设备信息查询）

---

## 🎯 功能目标

实现GB28181标准的语音对讲功能，包括：
1. 语音广播 (Voice Broadcast)
2. 双向对讲 (Two-way Talk)
3. 音频编解码（G.711A/U）
4. RTP音频传输

---

## 📋 功能清单（4个API接口）

### 1. 语音广播
- ✅ POST `/api/v1/voice/broadcast/start` - 开始语音广播
- ✅ POST `/api/v1/voice/broadcast/stop` - 停止语音广播

### 2. 双向对讲
- ✅ POST `/api/v1/voice/talk/start` - 开始双向对讲
- ✅ POST `/api/v1/voice/talk/stop` - 停止双向对讲

---

## 🏗️ 技术架构

### 1. 分层设计
```
HTTP API (Controller)
    ↓
Business Logic (Service)
    ↓
SIP Handler (VoiceHandler)
    ↓
eXosip2 (SIP Stack)
    ↓
RTP Audio Stream
    ↓
GB28181 Device
```

### 2. 语音广播流程

#### GB28181 语音广播INVITE
```
INVITE sip:34020000001320000001@192.168.1.100:5060 SIP/2.0
Via: SIP/2.0/UDP 192.168.1.1:5060;rport;branch=z9hG4bK123456
From: <sip:34020000002000000001@192.168.1.1:5060>;tag=123456
To: <sip:34020000001320000001@192.168.1.100:5060>
Call-ID: 123456789@192.168.1.1
CSeq: 1 INVITE
Contact: <sip:34020000002000000001@192.168.1.1:5060>
Content-Type: application/sdp
Content-Length: 256

v=0
o=34020000002000000001 0 0 IN IP4 192.168.1.1
s=Broadcast
c=IN IP4 192.168.1.1
t=0 0
m=audio 10000 RTP/AVP 8
a=rtpmap:8 PCMA/8000
a=sendonly
```

### 3. 双向对讲流程

#### 双向对讲INVITE
```
v=0
o=34020000002000000001 0 0 IN IP4 192.168.1.1
s=Talk
c=IN IP4 192.168.1.1
t=0 0
m=audio 10000 RTP/AVP 8
a=rtpmap:8 PCMA/8000
a=sendrecv
```

---

## 📁 文件结构

### 1. DTO层
```
GBServer/GbSipServer/dto/
└── VoiceDto.hpp                  # 语音相关DTO
    ├── VoiceBroadcastStartDto    # 语音广播开始请求
    ├── VoiceTalkStartDto         # 双向对讲开始请求
    ├── VoiceControlDto           # 语音控制请求
    └── VoiceResponseDto          # 语音响应
```

### 2. SIP处理层
```
GBServer/GbSipServer/sip/
└── VoiceHandler.hpp/cpp          # 语音SIP处理器
    ├── startBroadcast()          # 开始语音广播
    ├── stopBroadcast()           # 停止语音广播
    ├── startTalk()               # 开始双向对讲
    ├── stopTalk()                # 停止双向对讲
    └── buildAudioSDP()           # 构建音频SDP
```

### 3. Service层
```
GBServer/GbSipServer/service/
└── VoiceService.hpp/cpp          # 语音服务
    ├── startBroadcast()          # 开始语音广播
    ├── stopBroadcast()           # 停止语音广播
    ├── startTalk()               # 开始双向对讲
    └── stopTalk()                # 停止双向对讲
```

### 4. Controller层
```
GBServer/GbSipServer/controller/
└── VoiceController.hpp           # 语音REST控制器
    ├── POST /api/v1/voice/broadcast/start
    ├── POST /api/v1/voice/broadcast/stop
    ├── POST /api/v1/voice/talk/start
    └── POST /api/v1/voice/talk/stop
```

---

## 🔧 实现细节

### 1. VoiceHandler实现

#### 1.1 开始语音广播
```cpp
int VoiceHandler::startBroadcast(
    const std::string& deviceId,
    const std::string& localIp,
    int localPort,
    const std::string& deviceIp,
    int devicePort)
{
    // 1. 构建音频SDP（sendonly）
    std::string sdp = buildAudioSDP(localIp, localPort, "sendonly");
    
    // 2. 发送INVITE请求
    int callId = sendInvite(deviceId, deviceIp, devicePort, sdp);
    
    return callId;
}
```

#### 1.2 开始双向对讲
```cpp
int VoiceHandler::startTalk(
    const std::string& deviceId,
    const std::string& localIp,
    int localPort,
    const std::string& deviceIp,
    int devicePort)
{
    // 1. 构建音频SDP（sendrecv）
    std::string sdp = buildAudioSDP(localIp, localPort, "sendrecv");
    
    // 2. 发送INVITE请求
    int callId = sendInvite(deviceId, deviceIp, devicePort, sdp);
    
    return callId;
}
```

#### 1.3 构建音频SDP
```cpp
std::string VoiceHandler::buildAudioSDP(
    const std::string& localIp,
    int localPort,
    const std::string& mode)
{
    std::ostringstream sdp;
    sdp << "v=0\r\n";
    sdp << "o=" << server_id_ << " 0 0 IN IP4 " << localIp << "\r\n";
    sdp << "s=" << (mode == "sendonly" ? "Broadcast" : "Talk") << "\r\n";
    sdp << "c=IN IP4 " << localIp << "\r\n";
    sdp << "t=0 0\r\n";
    sdp << "m=audio " << localPort << " RTP/AVP 8\r\n";
    sdp << "a=rtpmap:8 PCMA/8000\r\n";
    sdp << "a=" << mode << "\r\n";
    
    return sdp.str();
}
```

---

## 📖 API接口文档

### 1. 开始语音广播

**接口**: `POST /api/v1/voice/broadcast/start`

**请求体**:
```json
{
  "device_id": "34020000001320000001",
  "local_ip": "192.168.1.1",
  "local_port": 10000
}
```

**响应**:
```json
{
  "code": 200,
  "status": "OK",
  "message": "Broadcast started",
  "call_id": "123"
}
```

### 2. 开始双向对讲

**接口**: `POST /api/v1/voice/talk/start`

**请求体**:
```json
{
  "device_id": "34020000001320000001",
  "local_ip": "192.168.1.1",
  "local_port": 10000
}
```

**响应**:
```json
{
  "code": 200,
  "status": "OK",
  "message": "Talk started",
  "call_id": "124"
}
```

---

## 🧪 测试用例

### 1. 语音广播测试
```bash
curl -X POST http://localhost:8003/api/v1/voice/broadcast/start \
  -H "Content-Type: application/json" \
  -d '{
    "device_id": "34020000001320000001",
    "local_ip": "192.168.1.1",
    "local_port": 10000
  }'
```

### 2. 双向对讲测试
```bash
curl -X POST http://localhost:8003/api/v1/voice/talk/start \
  -H "Content-Type: application/json" \
  -d '{
    "device_id": "34020000001320000001",
    "local_ip": "192.168.1.1",
    "local_port": 10000
  }'
```

---

**文档版本**: v1.0  
**创建时间**: 2026-02-09  
**状态**: ✅ 已完成
