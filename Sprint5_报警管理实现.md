# Sprint 5: 报警管理功能实现计划

## 📊 Sprint概览

**Sprint名称**: 报警管理  
**优先级**: 🔴 P0（高优先级）  
**预计工作量**: 2周  
**复杂度**: ⭐⭐⭐ (中高)  
**依赖**: Sprint 1（设备目录管理）、Sprint 2（设备信息查询）

---

## 🎯 功能目标

实现GB28181标准的报警管理功能，包括：
1. 报警订阅 (Alarm Subscribe)
2. 报警通知接收 (Alarm Notify)
3. 报警查询 (Alarm Query)
4. 报警确认/复位
5. 报警记录管理

---

## 📋 功能清单（7个API接口）

### 1. 报警订阅
- ✅ POST `/api/v1/alarm/subscribe` - 订阅设备报警
- ✅ POST `/api/v1/alarm/unsubscribe` - 取消报警订阅

### 2. 报警查询
- ✅ POST `/api/v1/alarm/query` - 查询历史报警
- ✅ GET `/api/v1/alarm/list` - 获取报警列表

### 3. 报警处理
- ✅ POST `/api/v1/alarm/confirm` - 确认报警
- ✅ POST `/api/v1/alarm/reset` - 复位报警

### 4. 报警记录
- ✅ GET `/api/v1/alarm/{id}` - 获取报警详情

---

## 🏗️ 技术架构

### 1. 分层设计
```
HTTP API (Controller)
    ↓
Business Logic (Service)
    ↓
SIP Handler (AlarmHandler)
    ↓
eXosip2 (SIP Stack)
    ↓
GB28181 Device
```

### 2. 报警订阅流程

#### GB28181 报警订阅SUBSCRIBE
```
SUBSCRIBE sip:34020000001320000001@192.168.1.100:5060 SIP/2.0
Via: SIP/2.0/UDP 192.168.1.1:5060;rport;branch=z9hG4bK123456
From: <sip:34020000002000000001@192.168.1.1:5060>;tag=123456
To: <sip:34020000001320000001@192.168.1.100:5060>
Call-ID: 123456789@192.168.1.1
CSeq: 1 SUBSCRIBE
Contact: <sip:34020000002000000001@192.168.1.1:5060>
Max-Forwards: 70
Event: Alarm
Expires: 3600
Content-Type: application/MANSCDP+xml
Content-Length: 256

<?xml version="1.0" encoding="GB2312"?>
<Query>
    <CmdType>Alarm</CmdType>
    <SN>123</SN>
    <DeviceID>34020000001320000001</DeviceID>
    <StartAlarmPriority>1</StartAlarmPriority>
    <EndAlarmPriority>4</EndAlarmPriority>
    <AlarmMethod>1</AlarmMethod>
</Query>
```

#### 报警通知NOTIFY
```xml
<?xml version="1.0" encoding="GB2312"?>
<Notify>
    <CmdType>Alarm</CmdType>
    <SN>124</SN>
    <DeviceID>34020000001320000002</DeviceID>
    <AlarmPriority>3</AlarmPriority>
    <AlarmMethod>1</AlarmMethod>
    <AlarmTime>2026-02-09T10:30:00</AlarmTime>
    <AlarmDescription>移动侦测报警</AlarmDescription>
    <Longitude>116.397128</Longitude>
    <Latitude>39.916527</Latitude>
</Notify>
```

### 3. 报警查询流程

#### 报警查询MESSAGE
```xml
<?xml version="1.0" encoding="GB2312"?>
<Query>
    <CmdType>Alarm</CmdType>
    <SN>125</SN>
    <DeviceID>34020000001320000001</DeviceID>
    <StartTime>2026-02-01T00:00:00</StartTime>
    <EndTime>2026-02-09T23:59:59</EndTime>
    <StartAlarmPriority>1</StartAlarmPriority>
    <EndAlarmPriority>4</EndAlarmPriority>
    <AlarmMethod>1</AlarmMethod>
</Query>
```

#### 报警查询响应
```xml
<?xml version="1.0" encoding="GB2312"?>
<Response>
    <CmdType>Alarm</CmdType>
    <SN>125</SN>
    <DeviceID>34020000001320000001</DeviceID>
    <SumNum>2</SumNum>
    <AlarmList Num="2">
        <Item>
            <DeviceID>34020000001320000002</DeviceID>
            <AlarmPriority>3</AlarmPriority>
            <AlarmMethod>1</AlarmMethod>
            <AlarmTime>2026-02-09T10:30:00</AlarmTime>
            <AlarmDescription>移动侦测报警</AlarmDescription>
            <Longitude>116.397128</Longitude>
            <Latitude>39.916527</Latitude>
        </Item>
        <Item>
            <DeviceID>34020000001320000002</DeviceID>
            <AlarmPriority>2</AlarmPriority>
            <AlarmMethod>2</AlarmMethod>
            <AlarmTime>2026-02-09T11:00:00</AlarmTime>
            <AlarmDescription>视频丢失报警</AlarmDescription>
            <Longitude>116.397128</Longitude>
            <Latitude>39.916527</Latitude>
        </Item>
    </AlarmList>
</Response>
```

---

## 📁 文件结构

### 1. DTO层
```
GBServer/GbSipServer/dto/
└── AlarmDto.hpp                  # 报警相关DTO
    ├── AlarmSubscribeDto         # 报警订阅请求
    ├── AlarmQueryDto             # 报警查询请求
    ├── AlarmItemDto              # 报警信息
    ├── AlarmListDto              # 报警列表响应
    ├── AlarmNotifyDto            # 报警通知
    ├── AlarmConfirmDto           # 报警确认请求
    └── AlarmResponseDto          # 通用响应
```

### 2. 数据库层
```
GBServer/GbSipServer/db/
└── AlarmDb.hpp                   # 报警数据库访问
    ├── createAlarm()             # 创建报警记录
    ├── getAlarmsByDeviceId()     # 查询设备报警
    ├── getAlarmsByTimeRange()    # 按时间范围查询
    ├── updateAlarmStatus()       # 更新报警状态
    └── deleteAlarm()             # 删除报警记录
```

### 3. SIP处理层
```
GBServer/GbSipServer/sip/
└── AlarmHandler.hpp/cpp          # 报警SIP处理器
    ├── subscribeAlarm()          # 订阅报警
    ├── unsubscribeAlarm()        # 取消订阅
    ├── queryAlarm()              # 查询报警
    ├── handleAlarmNotify()       # 处理报警通知
    ├── confirmAlarm()            # 确认报警
    └── resetAlarm()              # 复位报警
```

### 4. Service层
```
GBServer/GbSipServer/service/
└── AlarmService.hpp/cpp          # 报警服务
    ├── subscribeAlarm()          # 订阅报警
    ├── unsubscribeAlarm()        # 取消订阅
    ├── queryAlarms()             # 查询报警列表
    ├── getAlarmById()            # 获取报警详情
    ├── confirmAlarm()            # 确认报警
    └── resetAlarm()              # 复位报警
```

### 5. Controller层
```
GBServer/GbSipServer/controller/
└── AlarmController.hpp           # 报警REST控制器
    ├── POST /api/v1/alarm/subscribe
    ├── POST /api/v1/alarm/unsubscribe
    ├── POST /api/v1/alarm/query
    ├── GET /api/v1/alarm/list
    ├── GET /api/v1/alarm/{id}
    ├── POST /api/v1/alarm/confirm
    └── POST /api/v1/alarm/reset
```

---

## 🔧 实现细节

### 1. 报警数据库表设计

```sql
CREATE TABLE IF NOT EXISTS alarms (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    device_id VARCHAR(20) NOT NULL,
    channel_id VARCHAR(20) NOT NULL,
    alarm_priority INTEGER NOT NULL,
    alarm_method INTEGER NOT NULL,
    alarm_time DATETIME NOT NULL,
    alarm_description VARCHAR(512),
    longitude DOUBLE,
    latitude DOUBLE,
    status INTEGER DEFAULT 0,
    confirmed_at DATETIME,
    reset_at DATETIME,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_device_id (device_id),
    INDEX idx_channel_id (channel_id),
    INDEX idx_alarm_time (alarm_time),
    INDEX idx_status (status)
);
```

**字段说明**:
- `alarm_priority`: 报警级别（1-4，1最高）
- `alarm_method`: 报警方式（1-设备报警，2-平台报警，3-其他）
- `status`: 报警状态（0-未处理，1-已确认，2-已复位）

### 2. AlarmHandler实现

#### 2.1 订阅报警
```cpp
bool AlarmHandler::subscribeAlarm(
    const std::string& deviceId,
    int startPriority,
    int endPriority,
    int alarmMethod,
    int expires,
    const std::string& deviceIp,
    int devicePort)
{
    // 1. 构建Alarm订阅XML
    std::string xml = buildAlarmSubscribeXml(deviceId, startPriority, 
                                              endPriority, alarmMethod);
    
    // 2. 发送SUBSCRIBE请求
    int dialogId = sendSubscribe(deviceId, deviceIp, devicePort, xml, expires);
    
    // 3. 保存订阅信息
    if (dialogId > 0) {
        saveSubscription(deviceId, dialogId, expires);
        return true;
    }
    
    return false;
}
```

#### 2.2 处理报警通知
```cpp
void AlarmHandler::handleAlarmNotify(eXosip_event_t* event)
{
    // 1. 解析NOTIFY消息体
    osip_body_t* body = nullptr;
    osip_message_get_body(event->request, 0, &body);
    if (!body || !body->body) {
        return;
    }
    
    // 2. 解析报警XML
    std::string xml(body->body);
    auto alarm = parseAlarmNotifyXml(xml);
    
    // 3. 调用回调函数
    if (alarm_notify_callback_) {
        alarm_notify_callback_(alarm);
    }
    
    // 4. 回复200 OK
    eXosip_call_send_answer(sip_context_, event->tid, 200, nullptr);
}
```

#### 2.3 查询报警
```cpp
std::string AlarmHandler::queryAlarm(
    const std::string& deviceId,
    const std::string& startTime,
    const std::string& endTime,
    int startPriority,
    int endPriority,
    int alarmMethod,
    const std::string& deviceIp,
    int devicePort)
{
    // 1. 构建Alarm查询XML
    std::string xml = buildAlarmQueryXml(deviceId, startTime, endTime,
                                          startPriority, endPriority, alarmMethod);
    
    // 2. 发送MESSAGE请求
    std::string sn = sendMessage(deviceId, deviceIp, devicePort, xml);
    
    // 3. 注册回调
    registerCallback(sn, onSuccess, onError);
    
    return sn;
}
```

### 3. AlarmService实现

#### 3.1 订阅报警
```cpp
oatpp::Object<AlarmResponseDto> AlarmService::subscribeAlarm(
    const oatpp::Object<AlarmSubscribeDto>& dto)
{
    // 1. 参数验证
    if (!dto->device_id) {
        return createErrorResponse("Invalid parameters");
    }
    
    // 2. 查询设备信息
    auto device = device_db_->getDeviceById(dto->device_id);
    if (!device) {
        return createErrorResponse("Device not found");
    }
    
    // 3. 发送订阅请求
    bool success = alarm_handler_->subscribeAlarm(
        dto->device_id->c_str(),
        dto->start_priority ? dto->start_priority : 1,
        dto->end_priority ? dto->end_priority : 4,
        dto->alarm_method ? dto->alarm_method : 1,
        dto->expires ? dto->expires : 3600,
        device->RemoteIP->c_str(),
        device->RemotePort
    );
    
    if (!success) {
        return createErrorResponse("Failed to subscribe alarm");
    }
    
    return createSuccessResponse("Alarm subscribed successfully");
}
```

#### 3.2 处理报警通知
```cpp
void AlarmService::handleAlarmNotify(const oatpp::Object<AlarmNotifyDto>& alarm)
{
    // 1. 保存到数据库
    auto alarmItem = AlarmItemDto::createShared();
    alarmItem->device_id = alarm->device_id;
    alarmItem->channel_id = alarm->device_id;
    alarmItem->alarm_priority = alarm->alarm_priority;
    alarmItem->alarm_method = alarm->alarm_method;
    alarmItem->alarm_time = alarm->alarm_time;
    alarmItem->alarm_description = alarm->alarm_description;
    alarmItem->longitude = alarm->longitude;
    alarmItem->latitude = alarm->latitude;
    alarmItem->status = 0; // 未处理
    
    alarm_db_->createAlarm(alarmItem);
    
    // 2. 触发事件通知（WebSocket/SSE）
    notifyAlarmEvent(alarm);
}
```

---

## 📖 API接口文档

### 1. 订阅报警

**接口**: `POST /api/v1/alarm/subscribe`

**请求体**:
```json
{
  "device_id": "34020000001320000001",
  "start_priority": 1,
  "end_priority": 4,
  "alarm_method": 1,
  "expires": 3600
}
```

**参数说明**:
- `start_priority`: 起始报警级别（1-4，1最高）
- `end_priority`: 结束报警级别（1-4，4最低）
- `alarm_method`: 报警方式（1-设备报警，2-平台报警，3-其他）
- `expires`: 订阅有效期（秒）

**响应**:
```json
{
  "code": 200,
  "status": "OK",
  "message": "Alarm subscribed successfully"
}
```

### 2. 查询报警

**接口**: `POST /api/v1/alarm/query`

**请求体**:
```json
{
  "device_id": "34020000001320000001",
  "start_time": "2026-02-01T00:00:00",
  "end_time": "2026-02-09T23:59:59",
  "start_priority": 1,
  "end_priority": 4,
  "alarm_method": 1
}
```

**响应**:
```json
{
  "code": 200,
  "status": "OK",
  "sum_num": 2,
  "alarms": [
    {
      "id": 1,
      "device_id": "34020000001320000002",
      "alarm_priority": 3,
      "alarm_method": 1,
      "alarm_time": "2026-02-09T10:30:00",
      "alarm_description": "移动侦测报警",
      "longitude": 116.397128,
      "latitude": 39.916527,
      "status": 0
    }
  ]
}
```

### 3. 获取报警列表

**接口**: `GET /api/v1/alarm/list?device_id={deviceId}&offset={offset}&limit={limit}`

**响应**:
```json
{
  "code": 200,
  "status": "OK",
  "total": 10,
  "alarms": [...]
}
```

### 4. 确认报警

**接口**: `POST /api/v1/alarm/confirm`

**请求体**:
```json
{
  "alarm_id": 1
}
```

**响应**:
```json
{
  "code": 200,
  "status": "OK",
  "message": "Alarm confirmed"
}
```

---

## 🔄 数据流

```
报警订阅流程:
HTTP POST /api/v1/alarm/subscribe
    ↓
AlarmController::subscribeAlarm()
    ↓
AlarmService::subscribeAlarm()
    ↓
AlarmHandler::subscribeAlarm()
    ↓
buildAlarmSubscribeXml()
    ↓
sendSubscribe()
    ↓
eXosip_subscribe_send_initial_request(SUBSCRIBE)
    ↓
[设备接受订阅]
    ↓
200 OK
    ↓
HTTP响应

报警通知流程:
[设备发生报警]
    ↓
SIP NOTIFY
    ↓
eXosip_event_wait()
    ↓
EXOSIP_IN_SUBSCRIPTION_NOTIFY
    ↓
AlarmHandler::handleAlarmNotify()
    ↓
parseAlarmNotifyXml()
    ↓
AlarmService::handleAlarmNotify()
    ↓
AlarmDb::createAlarm()
    ↓
notifyAlarmEvent() (WebSocket/SSE)
    ↓
200 OK
```

---

## 📝 实现步骤

### 阶段1: 数据库和DTO（1天）
1. ✅ 创建报警表SQL
2. ✅ 创建 `db/AlarmDb.hpp`
3. ✅ 创建 `dto/AlarmDto.hpp`
4. ✅ 实现数据库CRUD操作

### 阶段2: SIP处理层（3天）
1. ✅ 创建 `sip/AlarmHandler.hpp`
2. ✅ 创建 `sip/AlarmHandler.cpp`
3. ✅ 实现报警订阅SUBSCRIBE
4. ✅ 实现报警通知NOTIFY处理
5. ✅ 实现报警查询MESSAGE
6. ✅ 实现XML解析

### 阶段3: Service层（2天）
1. ✅ 创建 `service/AlarmService.hpp`
2. ✅ 创建 `service/AlarmService.cpp`
3. ✅ 实现7个服务方法
4. ✅ 添加事件通知机制

### 阶段4: Controller层（1天）
1. ✅ 创建 `controller/AlarmController.hpp`
2. ✅ 实现7个API端点
3. ✅ 添加Swagger文档

### 阶段5: 集成和测试（3天）
1. ✅ 集成到SipServer
2. ✅ 注册到main.cpp
3. ✅ 编译测试
4. ✅ API功能测试
5. ✅ 与真实设备联调

---

## 🧪 测试用例

### 1. 订阅报警测试
```bash
curl -X POST http://localhost:8003/api/v1/alarm/subscribe \
  -H "Content-Type: application/json" \
  -d '{
    "device_id": "34020000001320000001",
    "start_priority": 1,
    "end_priority": 4,
    "alarm_method": 1,
    "expires": 3600
  }'
```

### 2. 查询报警测试
```bash
curl -X POST http://localhost:8003/api/v1/alarm/query \
  -H "Content-Type: application/json" \
  -d '{
    "device_id": "34020000001320000001",
    "start_time": "2026-02-01T00:00:00",
    "end_time": "2026-02-09T23:59:59",
    "start_priority": 1,
    "end_priority": 4
  }'
```

### 3. 获取报警列表测试
```bash
curl http://localhost:8003/api/v1/alarm/list?device_id=34020000001320000001&offset=0&limit=10
```

---

## 📊 完成度指标

### Sprint 5目标
- DTO层: 0% → 100%
- 数据库层: 0% → 100%
- SIP处理层: 0% → 100%
- Service层: 0% → 100%
- Controller层: 0% → 100%
- 集成测试: 0% → 100%

**总体目标**: 100%完成

---

## 🎯 验证清单

- [ ] 报警订阅SUBSCRIBE发送成功
- [ ] 报警通知NOTIFY接收正常
- [ ] 报警查询MESSAGE发送成功
- [ ] 报警列表解析正确
- [ ] 7个API接口全部实现
- [ ] Swagger文档完整
- [ ] 与真实设备联调成功
- [ ] 报警数据保存到数据库
- [ ] 报警事件通知正常

---

## 📚 参考文档

1. **GB/T 28181-2016** - 9.5 报警事件通知和分发
2. **RFC 3265** - SIP事件通知框架
3. **RFC 3261** - SIP协议

---

**文档版本**: v1.0  
**创建时间**: 2026-02-09  
**预计完成时间**: 2026-02-23  
**状态**: 📝 规划中

