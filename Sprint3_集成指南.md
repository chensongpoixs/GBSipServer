# Sprint 3: 云台控制PTZ集成指南

## 一、修改 sip_server.h

### 1.1 添加头文件
```cpp
#include "sip/PTZHandler.hpp"
```

### 1.2 添加成员变量和方法
```cpp
class SipServer {
private:
    // 添加Handler成员
    std::shared_ptr<gbsip_server::PTZHandler> ptz_handler_;
    
public:
    // 添加获取Handler的方法
    std::shared_ptr<gbsip_server::PTZHandler> getPTZHandler() {
        return ptz_handler_;
    }
};
```

## 二、修改 sip_server.cpp

### 2.1 在 init() 方法中初始化Handler
```cpp
bool SipServer::init(const SipServerConfig & info) {
    // ... 现有代码 ...
    
    // 初始化PTZHandler
    ptz_handler_ = std::make_shared<gbsip_server::PTZHandler>(
        sip_context_,
        sip_server_info_.sipServerId,
        sip_server_info_.ip,
        sip_server_info_.port
    );
    
    return true;
}
```

## 三、修改 main.cpp

### 3.1 注册Controller
```cpp
#include "controller/PTZController.hpp"

// 在 run() 函数中添加
auto ptzController = PTZController::createShared();
ptzController->setPTZHandler(gbsip_server::SipServer::GetInstance().getPTZHandler());
docEndpoints.append(router->addController(ptzController)->getEndpoints());
```

## 四、更新 CMakeLists.txt

CMakeLists.txt已经包含了sip目录和utils目录，新文件会自动包含。

## 五、编译和测试

### 5.1 编译
```bash
cd build
cmake ..
cmake --build . --config Release
```

### 5.2 运行
```bash
./bin/GbSipServer ../config/gbsip_server.yaml
```

### 5.3 测试API

**PTZ向上移动**:
```bash
curl -X POST http://localhost:8003/api/v1/ptz/control \
  -H "Content-Type: application/json" \
  -d '{
    "device_id": "34020000001320000001",
    "channel_id": "34020000001320000002",
    "direction": "UP",
    "h_speed": 128,
    "v_speed": 128
  }'
```

**PTZ停止**:
```bash
curl -X POST http://localhost:8003/api/v1/ptz/control \
  -H "Content-Type: application/json" \
  -d '{
    "device_id": "34020000001320000001",
    "channel_id": "34020000001320000002",
    "direction": "STOP",
    "h_speed": 0,
    "v_speed": 0
  }'
```

**变倍放大**:
```bash
curl -X POST http://localhost:8003/api/v1/ptz/zoom \
  -H "Content-Type: application/json" \
  -d '{
    "device_id": "34020000001320000001",
    "channel_id": "34020000001320000002",
    "zoom_in": true,
    "speed": 128
  }'
```

**设置预置位**:
```bash
curl -X POST http://localhost:8003/api/v1/ptz/preset/set \
  -H "Content-Type: application/json" \
  -d '{
    "device_id": "34020000001320000001",
    "channel_id": "34020000001320000002",
    "preset_id": 1,
    "preset_name": "大门入口"
  }'
```

**调用预置位**:
```bash
curl -X POST http://localhost:8003/api/v1/ptz/preset/call \
  -H "Content-Type: application/json" \
  -d '{
    "device_id": "34020000001320000001",
    "channel_id": "34020000001320000002",
    "preset_id": 1
  }'
```

**启动巡航**:
```bash
curl -X POST http://localhost:8003/api/v1/ptz/cruise/start \
  -H "Content-Type: application/json" \
  -d '{
    "device_id": "34020000001320000001",
    "channel_id": "34020000001320000002",
    "cruise_id": 1,
    "speed": 128
  }'
```

## 六、验证清单

- [ ] 编译无错误
- [ ] 服务器启动成功
- [ ] PTZ方向控制API返回200
- [ ] PTZ变倍控制API返回200
- [ ] PTZ变焦控制API返回200
- [ ] PTZ光圈控制API返回200
- [ ] 预置位设置API返回200
- [ ] 预置位调用API返回200
- [ ] 预置位删除API返回200
- [ ] 巡航启动API返回200
- [ ] 巡航停止API返回200
- [ ] 看守位设置API返回200
- [ ] 日志中有SIP INFO记录
- [ ] Swagger文档中有新接口

## 七、新增API清单

1. **POST** `/api/v1/ptz/control` - PTZ方向控制
2. **POST** `/api/v1/ptz/zoom` - 变倍控制
3. **POST** `/api/v1/ptz/focus` - 变焦控制
4. **POST** `/api/v1/ptz/iris` - 光圈控制
5. **POST** `/api/v1/ptz/preset/set` - 设置预置位
6. **POST** `/api/v1/ptz/preset/call` - 调用预置位
7. **POST** `/api/v1/ptz/preset/delete` - 删除预置位
8. **POST** `/api/v1/ptz/cruise/start` - 启动巡航
9. **POST** `/api/v1/ptz/cruise/stop` - 停止巡航
10. **POST** `/api/v1/ptz/guard` - 设置看守位

## 八、PTZ命令测试

### 8.1 方向控制测试
```bash
# 向上
direction: "UP"
# 向下
direction: "DOWN"
# 向左
direction: "LEFT"
# 向右
direction: "RIGHT"
# 左上
direction: "LEFT_UP"
# 左下
direction: "LEFT_DOWN"
# 右上
direction: "RIGHT_UP"
# 右下
direction: "RIGHT_DOWN"
# 停止
direction: "STOP"
```

### 8.2 预置位测试
```bash
# 设置预置位1-10
preset_id: 1-10

# 调用预置位
preset_id: 1

# 删除预置位
preset_id: 1
```

### 8.3 巡航测试
```bash
# 启动巡航组1
cruise_id: 1

# 停止巡航组1
cruise_id: 1
```

---

**Sprint 3 集成状态**: 待集成

**下一步**: 按照本指南完成集成
