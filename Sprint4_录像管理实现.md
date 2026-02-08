# Sprint 4: 录像管理功能实现计划

## 📊 Sprint概览

**Sprint名称**: 录像管理  
**优先级**: 🔴 P0（高优先级）  
**预计工作量**: 3周  
**复杂度**: ⭐⭐⭐⭐ (高)  
**依赖**: Sprint 1（设备目录管理）、Sprint 2（设备信息查询）

---

## 🎯 功能目标

实现GB28181标准的录像管理功能，包括：
1. 录像文件查询 (RecordInfo Query)
2. 录像回放控制（开始、暂停、继续、停止）
3. 录像回放进度控制（跳转、速度调整）
4. 录像下载
5. 录像文件管理

---

## 📋 功能清单（9个API接口）

### 1. 录像查询
- ✅ POST `/api/v1/record/query` - 查询录像列表

### 2. 录像回放控制
- ✅ POST `/api/v1/record/playback/start` - 开始回放
- ✅ POST `/api/v1/record/playback/pause` - 暂停回放
- ✅ POST `/api/v1/record/playback/resume` - 继续回放
- ✅ POST `/api/v1/record/playback/stop` - 停止回放

### 3. 回放进度控制
- ✅ POST `/api/v1/record/playback/seek` - 跳转播放位置
- ✅ POST `/api/v1/record/playback/speed` - 调整播放速度

### 4. 录像下载
- ✅ POST `/api/v1/record/download/start` - 开始下载
- ✅ POST `/api/v1/record/download/stop` - 停止下载

---

## 🏗️ 技术架构

### 1. 分层设计
```
HTTP API (Controller)
    ↓
Business Logic (Service)
    ↓
SIP Handler (RecordHandler)
    ↓
eXosip2 (SIP Stack)
    ↓
GB28181 Device
```

### 2. 录像查询流程

#### GB28181 录像查询MESSAGE
```xml
<?xml version="1.0" encoding="GB2312"?>
<Query>
    <CmdType>RecordInfo</CmdType>
    <SN>123</SN>
    <DeviceID>34020000001320000001</DeviceID>
    <StartTime>2026-02-01T00:00:00</StartTime>
    <EndTime>2026-02-09T23:59:59</EndTime>
    <Secrecy>0</Secrecy>
    <Type>all</Type>
</Query>
```

#### 录像查询响应
```xml
<?xml version="1.0" encoding="GB2312"?>
<Response>
    <CmdType>RecordInfo</CmdType>
    <SN>123</SN>
    <DeviceID>34020000001320000001</DeviceID>
    <SumNum>2</SumNum>
    <RecordList Num="2">
        <Item>
            <DeviceID>34020000001320000002</DeviceID>
            <Name>Camera 1</Name>
            <FilePath>/record/20260201/001.mp4</FilePath>
            <Address>Disk 1</Address>
            <StartTime>2026-02-01T08:00:00</StartTime>
            <EndTime>2026-02-01T09:00:00</EndTime>
            <Secrecy>0</Secrecy>
            <Type>time</Type>
            <RecorderID>34020000001320000001</RecorderID>
        </Item>
        <Item>
            <DeviceID>34020000001320000002</DeviceID>
            <Name>Camera 1</Name>
            <FilePath>/record/20260201/002.mp4</FilePath>
            <Address>Disk 1</Address>
            <StartTime>2026-02-01T10:00:00</StartTime>
            <EndTime>2026-02-01T11:00:00</EndTime>
            <Secrecy>0</Secrecy>
            <Type>time</Type>
            <RecorderID>34020000001320000001</RecorderID>
        </Item>
    </RecordList>
</Response>
```

### 3. 录像回放流程

#### 回放INVITE请求
```
INVITE sip:34020000001320000002@192.168.1.100:5060 SIP/2.0
Via: SIP/2.0/UDP 192.168.1.1:5060;rport;branch=z9hG4bK123456
From: <sip:34020000002000000001@192.168.1.1:5060>;tag=123456
To: <sip:34020000001320000002@192.168.1.100:5060>
Call-ID: 123456789@192.168.1.1
CSeq: 1 INVITE
Contact: <sip:34020000002000000001@192.168.1.1:5060>
Max-Forwards: 70
Subject: 34020000001320000002:0,34020000002000000001:0
Content-Type: application/sdp
Content-Length: 256

v=0
o=34020000002000000001 0 0 IN IP4 192.168.1.1
s=Playback
c=IN IP4 192.168.1.1
t=2026020108000000 2026020109000000
m=video 10000 TCP/RTP/AVP 96 98 97
a=recvonly
a=rtpmap:96 PS/90000
a=rtpmap:98 H264/90000
a=rtpmap:97 MPEG4/90000
a=setup:passive
a=connection:new
```

#### 回放控制INFO请求
```xml
<?xml version="1.0" encoding="GB2312"?>
<Control>
    <CmdType>PlaybackControl</CmdType>
    <SN>124</SN>
    <DeviceID>34020000001320000002</DeviceID>
    <PlaybackCmd>Pause</PlaybackCmd>
</Control>
```

**PlaybackCmd取值**:
- `Pause` - 暂停
- `Resume` - 继续
- `Fast` - 快进
- `Slow` - 慢放
- `Seek` - 跳转

---

## 📁 文件结构

### 1. DTO层
```
GBServer/GbSipServer/dto/
└── RecordDto.hpp                 # 录像相关DTO
    ├── RecordQueryDto            # 录像查询请求
    ├── RecordItemDto             # 录像文件信息
    ├── RecordListDto             # 录像列表响应
    ├── PlaybackStartDto          # 回放开始请求
    ├── PlaybackControlDto        # 回放控制请求
    ├── PlaybackSeekDto           # 回放跳转请求
    ├── PlaybackSpeedDto          # 回放速度请求
    ├── DownloadStartDto          # 下载开始请求
    └── RecordResponseDto         # 通用响应
```

### 2. 数据库层
```
GBServer/GbSipServer/db/
└── RecordDb.hpp                  # 录像数据库访问
    ├── createRecord()            # 创建录像记录
    ├── getRecordsByDeviceId()    # 查询设备录像
    ├── getRecordsByTimeRange()   # 按时间范围查询
    ├── updateRecordStatus()      # 更新录像状态
    └── deleteRecord()            # 删除录像记录
```

### 3. SIP处理层
```
GBServer/GbSipServer/sip/
└── RecordHandler.hpp/cpp         # 录像SIP处理器
    ├── queryRecordInfo()         # 查询录像信息
    ├── startPlayback()           # 开始回放
    ├── pausePlayback()           # 暂停回放
    ├── resumePlayback()          # 继续回放
    ├── stopPlayback()            # 停止回放
    ├── seekPlayback()            # 跳转回放
    ├── setPlaybackSpeed()        # 设置回放速度
    ├── startDownload()           # 开始下载
    └── stopDownload()            # 停止下载
```

### 4. Service层
```
GBServer/GbSipServer/service/
└── RecordService.hpp/cpp         # 录像服务
    ├── queryRecords()            # 查询录像列表
    ├── startPlayback()           # 开始回放
    ├── pausePlayback()           # 暂停回放
    ├── resumePlayback()          # 继续回放
    ├── stopPlayback()            # 停止回放
    ├── seekPlayback()            # 跳转回放
    ├── setPlaybackSpeed()        # 设置回放速度
    ├── startDownload()           # 开始下载
    └── stopDownload()            # 停止下载
```

### 5. Controller层
```
GBServer/GbSipServer/controller/
└── RecordController.hpp          # 录像REST控制器
    ├── POST /api/v1/record/query
    ├── POST /api/v1/record/playback/start
    ├── POST /api/v1/record/playback/pause
    ├── POST /api/v1/record/playback/resume
    ├── POST /api/v1/record/playback/stop
    ├── POST /api/v1/record/playback/seek
    ├── POST /api/v1/record/playback/speed
    ├── POST /api/v1/record/download/start
    └── POST /api/v1/record/download/stop
```

---

## 🔧 实现细节

### 1. 录像数据库表设计

```sql
CREATE TABLE IF NOT EXISTS records (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    device_id VARCHAR(20) NOT NULL,
    channel_id VARCHAR(20) NOT NULL,
    name VARCHAR(255),
    file_path VARCHAR(512),
    address VARCHAR(255),
    start_time DATETIME NOT NULL,
    end_time DATETIME NOT NULL,
    secrecy INTEGER DEFAULT 0,
    type VARCHAR(20),
    recorder_id VARCHAR(20),
    file_size BIGINT,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_device_id (device_id),
    INDEX idx_channel_id (channel_id),
    INDEX idx_time_range (start_time, end_time)
);
```

### 2. RecordHandler实现

#### 2.1 查询录像信息
```cpp
std::string RecordHandler::queryRecordInfo(
    const std::string& deviceId,
    const std::string& channelId,
    const std::string& startTime,
    const std::string& endTime,
    const std::string& type,
    const std::string& deviceIp,
    int devicePort,
    std::function<void(const RecordList&)> onSuccess,
    std::function<void(int, const std::string&)> onError)
{
    // 1. 构建RecordInfo查询XML
    std::string xml = buildRecordInfoXml(deviceId, channelId, 
                                          startTime, endTime, type);
    
    // 2. 发送MESSAGE请求
    std::string sn = sendMessage(deviceId, deviceIp, devicePort, xml);
    
    // 3. 注册回调
    registerCallback(sn, onSuccess, onError);
    
    return sn;
}
```

#### 2.2 开始回放
```cpp
bool RecordHandler::startPlayback(
    const std::string& deviceId,
    const std::string& channelId,
    const std::string& startTime,
    const std::string& endTime,
    const std::string& localIp,
    int localPort,
    const std::string& deviceIp,
    int devicePort,
    std::function<void(const std::string& sdp)> onSuccess,
    std::function<void(int, const std::string&)> onError)
{
    // 1. 构建SDP
    std::string sdp = buildPlaybackSdp(localIp, localPort, 
                                        startTime, endTime);
    
    // 2. 发送INVITE请求
    int callId = sendInvite(deviceId, channelId, deviceIp, 
                            devicePort, sdp);
    
    // 3. 注册回调
    registerInviteCallback(callId, onSuccess, onError);
    
    return callId > 0;
}
```

#### 2.3 回放控制
```cpp
bool RecordHandler::controlPlayback(
    const std::string& deviceId,
    const std::string& channelId,
    const std::string& command,
    const std::string& deviceIp,
    int devicePort)
{
    // 1. 构建PlaybackControl XML
    std::string xml = buildPlaybackControlXml(deviceId, channelId, command);
    
    // 2. 发送INFO消息
    return sendInfoMessage(deviceId, deviceIp, devicePort, xml);
}
```

### 3. RecordService实现

#### 3.1 查询录像列表
```cpp
oatpp::Object<RecordListDto> RecordService::queryRecords(
    const oatpp::Object<RecordQueryDto>& dto)
{
    // 1. 参数验证
    if (!dto->deviceId || !dto->startTime || !dto->endTime) {
        return createErrorResponse("Invalid parameters");
    }
    
    // 2. 查询设备信息
    auto device = getDeviceInfo(dto->deviceId);
    if (!device) {
        return createErrorResponse("Device not found");
    }
    
    // 3. 发送录像查询
    std::promise<RecordList> promise;
    auto future = promise.get_future();
    
    record_handler_->queryRecordInfo(
        dto->deviceId, dto->channelId,
        dto->startTime, dto->endTime, dto->type,
        device->ip, device->port,
        [&promise](const RecordList& records) {
            promise.set_value(records);
        },
        [&promise](int code, const std::string& msg) {
            promise.set_exception(
                std::make_exception_ptr(std::runtime_error(msg)));
        }
    );
    
    // 4. 等待响应（超时30秒）
    if (future.wait_for(std::chrono::seconds(30)) == 
        std::future_status::timeout) {
        return createErrorResponse("Query timeout");
    }
    
    // 5. 获取结果
    try {
        auto records = future.get();
        
        // 6. 保存到数据库
        saveRecordsToDb(records);
        
        // 7. 构建响应
        return buildRecordListResponse(records);
    } catch (const std::exception& e) {
        return createErrorResponse(e.what());
    }
}
```

---

## 📖 API接口文档

### 1. 查询录像列表

**接口**: `POST /api/v1/record/query`

**请求体**:
```json
{
  "device_id": "34020000001320000001",
  "channel_id": "34020000001320000002",
  "start_time": "2026-02-01T00:00:00",
  "end_time": "2026-02-09T23:59:59",
  "type": "all"
}
```

**type枚举值**:
- `all` - 所有录像
- `time` - 定时录像
- `alarm` - 报警录像
- `manual` - 手动录像

**响应**:
```json
{
  "code": 200,
  "status": "OK",
  "sum_num": 2,
  "records": [
    {
      "device_id": "34020000001320000002",
      "name": "Camera 1",
      "file_path": "/record/20260201/001.mp4",
      "address": "Disk 1",
      "start_time": "2026-02-01T08:00:00",
      "end_time": "2026-02-01T09:00:00",
      "secrecy": 0,
      "type": "time",
      "recorder_id": "34020000001320000001"
    }
  ]
}
```

### 2. 开始回放

**接口**: `POST /api/v1/record/playback/start`

**请求体**:
```json
{
  "device_id": "34020000001320000001",
  "channel_id": "34020000001320000002",
  "start_time": "2026-02-01T08:00:00",
  "end_time": "2026-02-01T09:00:00",
  "local_ip": "192.168.1.1",
  "local_port": 10000
}
```

**响应**:
```json
{
  "code": 200,
  "status": "OK",
  "message": "Playback started",
  "call_id": "123456",
  "sdp": "v=0\r\no=..."
}
```

### 3. 暂停回放

**接口**: `POST /api/v1/record/playback/pause`

**请求体**:
```json
{
  "device_id": "34020000001320000001",
  "channel_id": "34020000001320000002",
  "call_id": "123456"
}
```

### 4. 跳转回放

**接口**: `POST /api/v1/record/playback/seek`

**请求体**:
```json
{
  "device_id": "34020000001320000001",
  "channel_id": "34020000001320000002",
  "call_id": "123456",
  "seek_time": "2026-02-01T08:30:00"
}
```

### 5. 调整回放速度

**接口**: `POST /api/v1/record/playback/speed`

**请求体**:
```json
{
  "device_id": "34020000001320000001",
  "channel_id": "34020000001320000002",
  "call_id": "123456",
  "speed": 2.0
}
```

**speed取值**:
- `0.25` - 1/4倍速
- `0.5` - 1/2倍速
- `1.0` - 正常速度
- `2.0` - 2倍速
- `4.0` - 4倍速

---

## 🔄 数据流

```
HTTP POST /api/v1/record/query
    ↓
RecordController::queryRecords()
    ↓
RecordService::queryRecords()
    ↓
RecordHandler::queryRecordInfo()
    ↓
buildRecordInfoXml()
    ↓
sendMessage()
    ↓
eXosip_message_send_request(MESSAGE)
    ↓
[设备查询录像文件]
    ↓
200 OK + RecordInfo XML
    ↓
RecordHandler::handleMessageResponse()
    ↓
parseRecordInfoXml()
    ↓
callback(RecordList)
    ↓
RecordService::saveRecordsToDb()
    ↓
RecordDb::createRecord()
    ↓
HTTP响应
```

---

## 📝 实现步骤

### 阶段1: 数据库和DTO（2天）
1. ✅ 创建录像表SQL
2. ✅ 创建 `db/RecordDb.hpp`
3. ✅ 创建 `dto/RecordDto.hpp`
4. ✅ 实现数据库CRUD操作

### 阶段2: SIP处理层（5天）
1. ✅ 创建 `sip/RecordHandler.hpp`
2. ✅ 创建 `sip/RecordHandler.cpp`
3. ✅ 实现录像查询MESSAGE
4. ✅ 实现回放INVITE
5. ✅ 实现回放控制INFO
6. ✅ 实现XML解析

### 阶段3: Service层（3天）
1. ✅ 创建 `service/RecordService.hpp`
2. ✅ 创建 `service/RecordService.cpp`
3. ✅ 实现9个服务方法
4. ✅ 添加异步处理

### 阶段4: Controller层（2天）
1. ✅ 创建 `controller/RecordController.hpp`
2. ✅ 实现9个API端点
3. ✅ 添加Swagger文档

### 阶段5: 集成和测试（3天）
1. ✅ 集成到SipServer
2. ✅ 注册到main.cpp
3. ✅ 编译测试
4. ✅ API功能测试
5. ✅ 与真实设备联调

---

## 🧪 测试用例

### 1. 录像查询测试
```bash
curl -X POST http://localhost:8003/api/v1/record/query \
  -H "Content-Type: application/json" \
  -d '{
    "device_id": "34020000001320000001",
    "channel_id": "34020000001320000002",
    "start_time": "2026-02-01T00:00:00",
    "end_time": "2026-02-09T23:59:59",
    "type": "all"
  }'
```

### 2. 回放测试
```bash
# 开始回放
curl -X POST http://localhost:8003/api/v1/record/playback/start \
  -H "Content-Type: application/json" \
  -d '{
    "device_id": "34020000001320000001",
    "channel_id": "34020000001320000002",
    "start_time": "2026-02-01T08:00:00",
    "end_time": "2026-02-01T09:00:00",
    "local_ip": "192.168.1.1",
    "local_port": 10000
  }'

# 暂停回放
curl -X POST http://localhost:8003/api/v1/record/playback/pause \
  -H "Content-Type: application/json" \
  -d '{
    "device_id": "34020000001320000001",
    "channel_id": "34020000001320000002",
    "call_id": "123456"
  }'
```

---

## 📊 完成度指标

### Sprint 4目标
- DTO层: 0% → 100%
- 数据库层: 0% → 100%
- SIP处理层: 0% → 100%
- Service层: 0% → 100%
- Controller层: 0% → 100%
- 集成测试: 0% → 100%

**总体目标**: 100%完成

---

## 🎯 验证清单

- [ ] 录像查询MESSAGE发送成功
- [ ] 录像列表解析正确
- [ ] 回放INVITE发送成功
- [ ] SDP协商成功
- [ ] 回放控制INFO发送成功
- [ ] 9个API接口全部实现
- [ ] Swagger文档完整
- [ ] 与真实设备联调成功
- [ ] 录像数据保存到数据库
- [ ] 回放流媒体接收正常

---

## 📚 参考文档

1. **GB/T 28181-2016** - 9.3 历史视频回放
2. **GB/T 28181-2016** - 9.4 视频文件下载
3. **RFC 3261** - SIP协议
4. **RFC 4566** - SDP协议

---

**文档版本**: v1.0  
**创建时间**: 2026-02-09  
**预计完成时间**: 2026-03-02  
**状态**: 📝 规划中
