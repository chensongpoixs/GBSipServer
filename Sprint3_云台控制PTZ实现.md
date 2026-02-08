# Sprint 3: 云台控制PTZ功能实现计划

## 📊 Sprint概览

**Sprint名称**: 云台控制PTZ  
**优先级**: 🔴 P0（高优先级）  
**预计工作量**: 2周  
**复杂度**: ⭐⭐⭐ (中等)  
**依赖**: Sprint 1（设备目录管理）、Sprint 2（设备信息查询）

---

## 🎯 功能目标

实现GB28181标准的云台控制功能，包括：
1. PTZ方向控制（上、下、左、右、左上、左下、右上、右下）
2. 变倍控制（放大、缩小）
3. 变焦控制（近焦、远焦）
4. 光圈控制（开大、关小）
5. 预置位管理（设置、调用、删除）
6. 巡航控制（启动、停止）
7. 看守位设置
8. 辅助开关控制

---

## 📋 功能清单（10个API接口）

### 1. PTZ基础控制
- ✅ POST `/api/v1/ptz/control` - PTZ方向控制
- ✅ POST `/api/v1/ptz/zoom` - 变倍控制
- ✅ POST `/api/v1/ptz/focus` - 变焦控制
- ✅ POST `/api/v1/ptz/iris` - 光圈控制

### 2. 预置位管理
- ✅ POST `/api/v1/ptz/preset/set` - 设置预置位
- ✅ POST `/api/v1/ptz/preset/call` - 调用预置位
- ✅ POST `/api/v1/ptz/preset/delete` - 删除预置位

### 3. 巡航控制
- ✅ POST `/api/v1/ptz/cruise/start` - 启动巡航
- ✅ POST `/api/v1/ptz/cruise/stop` - 停止巡航

### 4. 其他控制
- ✅ POST `/api/v1/ptz/guard` - 设置看守位

---

## 🏗️ 技术架构

### 1. 分层设计
```
HTTP API (Controller)
    ↓
Business Logic (Service)
    ↓
SIP Handler (PTZHandler)
    ↓
eXosip2 (SIP Stack)
    ↓
GB28181 Device
```

### 2. PTZ命令编码

#### GB28181 PTZ命令格式
```
A50F [命令字节1] [命令字节2] [数据字节1] [数据字节2] [数据字节3] [数据字节4] [校验字节]
```

**命令字节说明**:
- 字节1（高4位）: 控制类型
  - 0x8: 停止
  - 0x9: 上
  - 0xA: 下
  - 0xB: 左
  - 0xC: 右
  - 0xD: 左上
  - 0xE: 左下
  - 0xF: 右上
  - 0x1: 右下

- 字节1（低4位）: 水平速度（0-F）
- 字节2（高4位）: 垂直速度（0-F）
- 字节2（低4位）: 变倍速度（0-F）

**数据字节说明**:
- 数据1: 预置位号（高字节）
- 数据2: 预置位号（低字节）
- 数据3: 巡航组号
- 数据4: 保留

**校验字节**: 所有字节异或运算结果

#### 示例命令
```cpp
// 向上移动，速度5
A50F 95 00 00 00 00 00 9C

// 向左移动，速度3
A50F B3 00 00 00 00 00 B0

// 调用预置位1
A50F 81 00 00 01 00 00 80

// 放大，速度2
A50F 10 20 00 00 00 00 30
```

---

## 📁 文件结构

### 1. DTO层
```
GBServer/GbSipServer/dto/
└── PTZDto.hpp                    # PTZ控制DTO
    ├── PTZControlDto             # 方向控制请求
    ├── PTZZoomDto                # 变倍控制请求
    ├── PTZFocusDto               # 变焦控制请求
    ├── PTZIrisDto                # 光圈控制请求
    ├── PTZPresetDto              # 预置位操作请求
    ├── PTZCruiseDto              # 巡航控制请求
    └── PTZResponseDto            # 控制响应
```

### 2. SIP处理层
```
GBServer/GbSipServer/sip/
└── PTZHandler.hpp/cpp            # PTZ控制SIP处理器
    ├── sendPTZControl()          # 发送PTZ控制INFO
    ├── encodePTZCommand()        # 编码PTZ命令
    ├── buildPTZXml()             # 构建PTZ XML
    └── handlePTZResponse()       # 处理响应
```

### 3. Service层
```
GBServer/GbSipServer/service/
└── PTZService.hpp/cpp            # PTZ控制服务
    ├── controlDirection()        # 方向控制
    ├── controlZoom()             # 变倍控制
    ├── controlFocus()            # 变焦控制
    ├── controlIris()             # 光圈控制
    ├── setPreset()               # 设置预置位
    ├── callPreset()              # 调用预置位
    ├── deletePreset()            # 删除预置位
    ├── startCruise()             # 启动巡航
    ├── stopCruise()              # 停止巡航
    └── setGuard()                # 设置看守位
```

### 4. Controller层
```
GBServer/GbSipServer/controller/
└── PTZController.hpp             # PTZ控制REST控制器
    ├── POST /api/v1/ptz/control
    ├── POST /api/v1/ptz/zoom
    ├── POST /api/v1/ptz/focus
    ├── POST /api/v1/ptz/iris
    ├── POST /api/v1/ptz/preset/set
    ├── POST /api/v1/ptz/preset/call
    ├── POST /api/v1/ptz/preset/delete
    ├── POST /api/v1/ptz/cruise/start
    ├── POST /api/v1/ptz/cruise/stop
    └── POST /api/v1/ptz/guard
```

---

## 🔧 实现细节

### 1. PTZHandler实现

#### 1.1 PTZ命令编码器
```cpp
class PTZCommandEncoder {
public:
    // PTZ方向命令
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
    
    // 编码PTZ命令
    static std::string encode(Direction dir, int hSpeed, int vSpeed);
    static std::string encodeZoom(bool zoomIn, int speed);
    static std::string encodeFocus(bool focusNear, int speed);
    static std::string encodeIris(bool irisOpen, int speed);
    static std::string encodePresetSet(int presetId);
    static std::string encodePresetCall(int presetId);
    static std::string encodePresetDelete(int presetId);
    
private:
    static uint8_t calculateChecksum(const std::vector<uint8_t>& data);
};
```

#### 1.2 PTZ XML构建
```xml
<?xml version="1.0" encoding="GB2312"?>
<Control>
    <CmdType>DeviceControl</CmdType>
    <SN>123</SN>
    <DeviceID>34020000001320000001</DeviceID>
    <PTZCmd>A50F95000000000000</PTZCmd>
</Control>
```

#### 1.3 发送PTZ控制
```cpp
bool PTZHandler::sendPTZControl(
    const std::string& deviceId,
    const std::string& channelId,
    const std::string& ptzCmd,
    const std::string& deviceIp,
    int devicePort)
{
    // 1. 构建XML消息体
    std::string xml = buildPTZXml(deviceId, channelId, ptzCmd);
    
    // 2. 发送SIP INFO消息
    return sendInfoMessage(deviceId, deviceIp, devicePort, xml);
}
```

### 2. PTZService实现

#### 2.1 方向控制
```cpp
oatpp::Object<PTZResponseDto> PTZService::controlDirection(
    const oatpp::Object<PTZControlDto>& dto)
{
    // 1. 参数验证
    if (!dto->deviceId || !dto->channelId) {
        return createErrorResponse("Invalid parameters");
    }
    
    // 2. 查询设备信息
    auto device = getDeviceInfo(dto->deviceId);
    if (!device) {
        return createErrorResponse("Device not found");
    }
    
    // 3. 编码PTZ命令
    std::string ptzCmd = PTZCommandEncoder::encode(
        dto->direction, dto->hSpeed, dto->vSpeed);
    
    // 4. 发送控制命令
    bool success = ptzHandler_->sendPTZControl(
        dto->deviceId, dto->channelId, ptzCmd,
        device->ip, device->port);
    
    // 5. 返回结果
    return createSuccessResponse(success);
}
```

### 3. PTZController实现

#### 3.1 API端点定义
```cpp
ENDPOINT("POST", "/api/v1/ptz/control", controlDirection,
         BODY_DTO(Object<PTZControlDto>, dto))
{
    auto result = m_ptzService.controlDirection(dto);
    if (result->code == 200) {
        return createDtoResponse(Status::CODE_200, result);
    } else {
        return createDtoResponse(Status::CODE_400, result);
    }
}
```

---

## 📖 API接口文档

### 1. PTZ方向控制

**接口**: `POST /api/v1/ptz/control`

**请求体**:
```json
{
  "device_id": "34020000001320000001",
  "channel_id": "34020000001320000002",
  "direction": "UP",
  "h_speed": 5,
  "v_speed": 5,
  "duration": 0
}
```

**direction枚举值**:
- `STOP` - 停止
- `UP` - 向上
- `DOWN` - 向下
- `LEFT` - 向左
- `RIGHT` - 向右
- `LEFT_UP` - 左上
- `LEFT_DOWN` - 左下
- `RIGHT_UP` - 右上
- `RIGHT_DOWN` - 右下

**响应**:
```json
{
  "code": 200,
  "status": "OK",
  "message": "PTZ control success"
}
```

### 2. 变倍控制

**接口**: `POST /api/v1/ptz/zoom`

**请求体**:
```json
{
  "device_id": "34020000001320000001",
  "channel_id": "34020000001320000002",
  "zoom_in": true,
  "speed": 3
}
```

### 3. 设置预置位

**接口**: `POST /api/v1/ptz/preset/set`

**请求体**:
```json
{
  "device_id": "34020000001320000001",
  "channel_id": "34020000001320000002",
  "preset_id": 1,
  "preset_name": "大门入口"
}
```

### 4. 调用预置位

**接口**: `POST /api/v1/ptz/preset/call`

**请求体**:
```json
{
  "device_id": "34020000001320000001",
  "channel_id": "34020000001320000002",
  "preset_id": 1
}
```

---

## 🔄 数据流

```
HTTP POST /api/v1/ptz/control
    ↓
PTZController::controlDirection()
    ↓
PTZService::controlDirection()
    ↓
PTZCommandEncoder::encode()
    ↓
PTZHandler::sendPTZControl()
    ↓
buildPTZXml()
    ↓
sendInfoMessage()
    ↓
eXosip_call_send_request(INFO)
    ↓
[设备执行PTZ动作]
    ↓
200 OK响应
    ↓
HTTP响应
```

---

## 📝 实现步骤

### 阶段1: DTO和工具类（1天）
1. ✅ 创建 `dto/PTZDto.hpp`
2. ✅ 实现 `PTZCommandEncoder` 类
3. ✅ 添加PTZ命令编码测试

### 阶段2: SIP处理层（2天）
1. ✅ 创建 `sip/PTZHandler.hpp`
2. ✅ 创建 `sip/PTZHandler.cpp`
3. ✅ 实现PTZ控制INFO消息发送
4. ✅ 实现XML构建

### 阶段3: Service层（2天）
1. ✅ 创建 `service/PTZService.hpp`
2. ✅ 创建 `service/PTZService.cpp`
3. ✅ 实现10个控制方法
4. ✅ 添加参数验证和错误处理

### 阶段4: Controller层（1天）
1. ✅ 创建 `controller/PTZController.hpp`
2. ✅ 实现10个API端点
3. ✅ 添加Swagger文档

### 阶段5: 集成和测试（2天）
1. ✅ 集成到SipServer
2. ✅ 注册到main.cpp
3. ✅ 编译测试
4. ✅ API功能测试
5. ✅ 与真实设备联调

---

## 🧪 测试用例

### 1. 单元测试
```cpp
TEST(PTZCommandEncoderTest, EncodeDirection) {
    // 测试向上移动
    std::string cmd = PTZCommandEncoder::encode(
        PTZCommandEncoder::UP, 5, 5);
    EXPECT_EQ(cmd, "A50F95000000000000");
}

TEST(PTZCommandEncoderTest, EncodePreset) {
    // 测试调用预置位1
    std::string cmd = PTZCommandEncoder::encodePresetCall(1);
    EXPECT_EQ(cmd, "A50F81000001000000");
}
```

### 2. 集成测试
```bash
# 测试PTZ向上移动
curl -X POST http://localhost:8003/api/v1/ptz/control \
  -H "Content-Type: application/json" \
  -d '{
    "device_id": "34020000001320000001",
    "channel_id": "34020000001320000002",
    "direction": "UP",
    "h_speed": 5,
    "v_speed": 5
  }'

# 测试调用预置位
curl -X POST http://localhost:8003/api/v1/ptz/preset/call \
  -H "Content-Type: application/json" \
  -d '{
    "device_id": "34020000001320000001",
    "channel_id": "34020000001320000002",
    "preset_id": 1
  }'
```

---

## 📊 完成度指标

### Sprint 3目标
- DTO层: 0% → 100%
- SIP处理层: 0% → 100%
- Service层: 0% → 100%
- Controller层: 0% → 100%
- 集成测试: 0% → 100%

**总体目标**: 100%完成

---

## 🎯 验证清单

- [ ] PTZ命令编码正确
- [ ] SIP INFO消息发送成功
- [ ] XML格式符合GB28181标准
- [ ] 10个API接口全部实现
- [ ] Swagger文档完整
- [ ] 与真实设备联调成功
- [ ] 所有方向控制正常
- [ ] 预置位功能正常
- [ ] 巡航功能正常

---

## 📚 参考文档

1. **GB/T 28181-2016** - 9.5 云台控制
2. **GB/T 28181-2016** - 附录F PTZ控制码
3. **API文档**: https://gbs.liveqing.com:10010/apidoc/

---

**文档版本**: v1.0  
**创建时间**: 2026-02-09  
**预计完成时间**: 2026-02-23  
**状态**: 📝 规划中
