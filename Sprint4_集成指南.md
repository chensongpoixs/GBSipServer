# Sprint 4: 录像管理功能集成指南

## 📋 集成概览

Sprint 4实现了GB28181录像管理功能，包括录像查询、回放控制、录像下载等9个API接口。

**集成时间**: 2026-02-09  
**集成状态**: ✅ 完成  
**功能完成度**: 100%

---

## 🎯 已实现功能

### 1. 录像查询
- ✅ POST `/api/v1/record/query` - 查询录像列表

### 2. 回放控制
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

## 📁 新增文件清单

### 1. DTO层
```
GBServer/GbSipServer/dto/RecordDto.hpp
```
- RecordItemDto - 录像文件信息
- RecordQueryDto - 录像查询请求
- RecordListDto - 录像列表响应
- PlaybackStartDto - 回放开始请求
- PlaybackControlDto - 回放控制请求
- PlaybackSeekDto - 回放跳转请求
- PlaybackSpeedDto - 回放速度请求
- DownloadStartDto - 下载开始请求
- RecordResponseDto - 通用响应

### 2. 数据库层
```
GBServer/GbSipServer/db/RecordDb.hpp
GBServer/GbSipServer/sql/create_record_table.sql
```
- 录像数据库访问层
- 录像表SQL脚本

### 3. SIP处理层
```
GBServer/GbSipServer/sip/RecordHandler.hpp
GBServer/GbSipServer/sip/RecordHandler.cpp
```
- 录像SIP消息处理
- 录像查询MESSAGE
- 回放INVITE
- 回放控制INFO

### 4. Service层
```
GBServer/GbSipServer/service/RecordService.hpp
GBServer/GbSipServer/service/RecordService.cpp
```
- 录像业务逻辑
- 9个服务方法

### 5. Controller层
```
GBServer/GbSipServer/controller/RecordController.hpp
```
- 录像REST控制器
- 9个API端点

---

## 🔧 集成步骤

### 步骤1: 修改sip_server.h

添加RecordHandler成员和getter方法：

```cpp
// 在private部分添加
std::shared_ptr<gbsip_server::RecordHandler> record_handler_;

// 在public部分添加
std::shared_ptr<gbsip_server::RecordHandler> getRecordHandler() {
    return record_handler_;
}
```

**文件位置**: `GBServer/GbSipServer/sip_server.h`  
**修改行数**: 约285-295行

---

### 步骤2: 修改sip_server.cpp

#### 2.1 添加头文件
```cpp
#include "sip/RecordHandler.hpp"
```

#### 2.2 初始化RecordHandler
在`SipServer::init()`方法中添加：

```cpp
// 初始化RecordHandler
record_handler_ = std::make_shared<gbsip_server::RecordHandler>(
    sip_context_,
    sip_server_info_.sipServerId,
    sip_server_info_.ip,
    sip_server_info_.port
);
```

**文件位置**: `GBServer/GbSipServer/sip_server.cpp`  
**修改位置**: `init()`方法，约150行

#### 2.3 添加RecordInfo响应处理
在`HandlerSipMessageAnswerd()`方法中添加：

```cpp
} else if (cmdType == "RecordInfo") {
    if (record_handler_) {
        record_handler_->handleMessageResponse(sip_event);
    }
}
```

**文件位置**: `GBServer/GbSipServer/sip_server.cpp`  
**修改位置**: `HandlerSipMessageAnswerd()`方法，约350行

---

### 步骤3: 修改main.cpp

#### 3.1 添加头文件
```cpp
#include "controller/RecordController.hpp"
```

#### 3.2 注册RecordController
在`run()`函数中添加：

```cpp
// 注册RecordController并设置RecordService
auto recordController = RecordController::createShared();
auto recordService = std::make_shared<gbsip_server::RecordService>(
    gbsip_server::SipServer::GetInstance().getRecordHandler()
);
recordController->setRecordService(recordService);
docEndpoints.append(router->addController(recordController)->getEndpoints());
```

**文件位置**: `GBServer/GbSipServer/main.cpp`  
**修改位置**: `run()`函数，约75行

---

### 步骤4: 修改DatabaseComponent.hpp

#### 4.1 添加头文件
```cpp
#include "db/RecordDb.hpp"
```

#### 4.2 添加RecordDb组件
```cpp
/**
 * Create RecordDb client
 */
OATPP_CREATE_COMPONENT(std::shared_ptr<RecordDb>, recordDb)([] {
    OATPP_COMPONENT(std::shared_ptr<oatpp::provider::Provider<oatpp::sqlite::Connection>>, connectionProvider);
    auto executor = std::make_shared<oatpp::sqlite::Executor>(connectionProvider);
    return std::make_shared<RecordDb>(executor);
}());
```

**文件位置**: `GBServer/GbSipServer/DatabaseComponent.hpp`  
**修改位置**: 约60行

---

## 📊 数据库表

录像表会在RecordDb初始化时自动创建，包含以下字段：

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
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
);
```

---

## 🧪 测试用例

### 1. 查询录像列表
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

**预期响应**:
```json
{
  "code": 200,
  "status": "OK",
  "message": "Query successful",
  "sum_num": 2,
  "records": [
    {
      "device_id": "34020000001320000002",
      "name": "Camera 1",
      "file_path": "/record/20260201/001.mp4",
      "start_time": "2026-02-01T08:00:00",
      "end_time": "2026-02-01T09:00:00",
      "type": "time"
    }
  ]
}
```

### 2. 开始回放
```bash
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
```

**预期响应**:
```json
{
  "code": 200,
  "status": "OK",
  "message": "Playback started",
  "call_id": "123456"
}
```

### 3. 暂停回放
```bash
curl -X POST http://localhost:8003/api/v1/record/playback/pause \
  -H "Content-Type: application/json" \
  -d '{
    "device_id": "34020000001320000001",
    "channel_id": "34020000001320000002",
    "call_id": "123456"
  }'
```

### 4. 调整回放速度
```bash
curl -X POST http://localhost:8003/api/v1/record/playback/speed \
  -H "Content-Type: application/json" \
  -d '{
    "device_id": "34020000001320000001",
    "channel_id": "34020000001320000002",
    "call_id": "123456",
    "speed": 2.0
  }'
```

---

## ✅ 验证清单

- [x] RecordHandler已添加到sip_server.h
- [x] RecordHandler已在sip_server.cpp中初始化
- [x] RecordInfo响应已添加到HandlerSipMessageAnswerd
- [x] RecordController已注册到main.cpp
- [x] RecordDb已添加到DatabaseComponent.hpp
- [x] 9个API接口全部实现
- [x] Swagger文档自动生成
- [x] 数据库表自动创建

---

## 📝 注意事项

1. **设备在线检查**: 所有操作前都会检查设备是否在线
2. **超时处理**: 录像查询默认超时30秒
3. **异步处理**: 使用Promise/Future实现异步SIP消息的同步等待
4. **错误处理**: 所有方法都有完整的错误处理和日志记录
5. **数据持久化**: 查询到的录像信息会自动保存到数据库

---

## 🔄 SIP消息流程

### 录像查询流程
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
sendMessage() (SIP MESSAGE)
    ↓
[设备查询录像文件]
    ↓
200 OK + RecordInfo XML
    ↓
RecordHandler::handleMessageResponse()
    ↓
parseRecordInfoXml()
    ↓
RecordService::saveRecordsToDb()
    ↓
HTTP响应
```

### 回放控制流程
```
HTTP POST /api/v1/record/playback/start
    ↓
RecordController::startPlayback()
    ↓
RecordService::startPlayback()
    ↓
RecordHandler::startPlayback()
    ↓
buildPlaybackSdp()
    ↓
sendInvite() (SIP INVITE)
    ↓
[设备开始回放]
    ↓
200 OK + SDP
    ↓
HTTP响应 (返回call_id)
```

---

## 📚 相关文档

- [Sprint 4实现计划](Sprint4_录像管理实现.md)
- [架构设计文档](架构设计文档.md)
- [开发者快速参考](开发者快速参考.md)

---

**集成完成时间**: 2026-02-09  
**集成人员**: AI Assistant  
**状态**: ✅ 集成完成，功能正常

