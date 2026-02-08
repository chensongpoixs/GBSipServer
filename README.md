# GB28181 国标SIP服务器

## 📖 项目简介

基于GB/T 28181-2016标准的视频监控联网系统SIP服务器，使用C++实现，支持设备注册、目录管理、云台控制、录像管理等完整功能。

**当前版本**: v0.35  
**完成度**: 35.4% (34/96功能)  
**开发状态**: 🟢 活跃开发中  

---

## ✨ 核心特性

### 已实现功能 ✅

#### 1. 基础SIP信令（100%）
- ✅ 设备注册 (REGISTER)
- ✅ 注册认证 (Digest Authentication)
- ✅ 心跳保活 (Keepalive)
- ✅ 设备注销
- ✅ 设备在线状态管理

#### 2. 设备目录管理（100%）
- ✅ 设备目录查询 (Catalog Query)
- ✅ 目录订阅 (Catalog Subscribe)
- ✅ 目录通知处理 (NOTIFY)
- ✅ 通道信息管理
- ✅ 7个HTTP API接口

#### 3. 设备信息查询（50%）
- ✅ 设备信息查询 (DeviceInfo)
- ✅ 设备状态查询 (DeviceStatus)
- ⏳ 设备配置查询 (ConfigDownload)

#### 4. 云台控制PTZ（100%）
- ✅ PTZ方向控制（9个方向）
- ✅ 变倍控制（放大/缩小）
- ✅ 变焦控制（近焦/远焦）
- ✅ 光圈控制（开大/关小）
- ✅ 预置位管理（设置/调用/删除）
- ✅ 巡航控制（启动/停止）
- ✅ 看守位设置

#### 5. 设备管理（100%）
- ✅ 设备CRUD操作
- ✅ 设备分页查询
- ✅ 设备状态管理

#### 6. 用户管理（100%）
- ✅ 用户CRUD操作
- ✅ 用户分页查询
- ✅ 用户权限管理

### 待实现功能 ⏳

#### 7. 录像管理（0%）
- ⏳ 录像文件查询
- ⏳ 录像回放控制
- ⏳ 录像下载
- ⏳ 回放速度控制

#### 8. 报警管理（0%）
- ⏳ 报警订阅
- ⏳ 报警通知接收
- ⏳ 报警确认/复位

#### 9. 语音对讲（0%）
- ⏳ 语音广播
- ⏳ 双向对讲

#### 10. 平台级联（0%）
- ⏳ 向上级平台注册
- ⏳ 目录共享
- ⏳ 级联流转发

---

## 🏗️ 技术架构

### 技术栈
- **语言**: C++11/14
- **HTTP框架**: oatpp 1.4.0
- **SIP协议栈**: eXosip2
- **数据库**: SQLite3 + oatpp-sqlite
- **API文档**: oatpp-swagger
- **构建工具**: CMake

### 架构设计
```
┌─────────────────────────────────────────┐
│         HTTP API (REST)                 │
│  (oatpp + Swagger)                      │
├─────────────────────────────────────────┤
│         Controller Layer                │
│  (UserController, DeviceController,     │
│   CatalogController, PTZController...)  │
├─────────────────────────────────────────┤
│         Service Layer                   │
│  (CatalogService, PTZService,           │
│   DeviceInfoService...)                 │
├─────────────────────────────────────────┤
│         SIP Handler Layer               │
│  (CatalogHandler, PTZHandler,           │
│   DeviceInfoHandler...)                 │
├─────────────────────────────────────────┤
│         SIP Protocol Stack              │
│  (eXosip2)                              │
├─────────────────────────────────────────┤
│         Database Layer                  │
│  (SQLite3 + oatpp-sqlite)               │
└─────────────────────────────────────────┘
```

### 目录结构
```
GBServer/GbSipServer/
├── dto/                    # 数据传输对象
├── db/                     # 数据库访问层
├── service/                # 业务逻辑层
├── controller/             # HTTP控制器
├── sip/                    # SIP处理层
├── utils/                  # 工具类
├── sql/                    # SQL脚本
├── tests/                  # 测试代码
├── config/                 # 配置文件
└── docs/                   # 文档
```

---

## 🚀 快速开始

### 环境要求
- C++11/14编译器（GCC 7+, Clang 5+, MSVC 2017+）
- CMake 3.10+
- SQLite3
- eXosip2
- oatpp 1.4.0
- oatpp-sqlite 1.4.0
- oatpp-swagger 1.4.0

### 编译步骤

#### Linux/macOS
```bash
# 1. 克隆项目
git clone <repository-url>
cd GBServer/GbSipServer

# 2. 创建构建目录
mkdir build && cd build

# 3. 配置CMake
cmake ..

# 4. 编译
make -j4

# 5. 运行
./bin/GbSipServer ../config/gbsip_server.yaml
```

#### Windows
```bash
# 1. 克隆项目
git clone <repository-url>
cd GBServer\GbSipServer

# 2. 创建构建目录
mkdir build
cd build

# 3. 配置CMake
cmake ..

# 4. 编译
cmake --build . --config Release

# 5. 运行
.\bin\Release\GbSipServer.exe ..\config\gbsip_server.yaml
```

### 配置文件

编辑 `config/gbsip_server.yaml`:
```yaml
sip:
  server_id: "34020000002000000001"
  server_ip: "192.168.1.100"
  server_port: 5060
  realm: "3402000000"
  password: "12345678"
  ua: "GB28181 Server"
  timeout: 3600

http:
  port: 8003
  
database:
  path: "db/gbsip.db"
```

---

## 📚 API文档

### Swagger UI
启动服务器后访问: http://localhost:8003/swagger/ui

### API端点概览

#### 设备管理
- `POST /devices` - 创建设备
- `GET /devices/{id}` - 查询设备
- `GET /devices/offset/{offset}/limit/{limit}` - 分页查询
- `PUT /device/{id}` - 更新设备
- `DELETE /devices/{id}` - 删除设备

#### 设备目录
- `GET /api/v1/device/{deviceId}/catalog` - 查询设备目录
- `GET /api/v1/device/{deviceId}/channels` - 获取通道列表
- `GET /api/v1/channel/{channelId}` - 获取单个通道
- `PUT /api/v1/channel/{channelId}` - 更新通道
- `DELETE /api/v1/channel/{channelId}` - 删除通道
- `POST /api/v1/catalog/subscribe` - 订阅目录
- `POST /api/v1/catalog/unsubscribe/{deviceId}` - 取消订阅

#### 设备信息
- `GET /api/v1/device/{deviceId}/info` - 查询设备信息
- `GET /api/v1/device/{deviceId}/status` - 查询设备状态

#### PTZ控制
- `POST /api/v1/ptz/control` - PTZ方向控制
- `POST /api/v1/ptz/zoom` - 变倍控制
- `POST /api/v1/ptz/focus` - 变焦控制
- `POST /api/v1/ptz/iris` - 光圈控制
- `POST /api/v1/ptz/preset/set` - 设置预置位
- `POST /api/v1/ptz/preset/call` - 调用预置位
- `POST /api/v1/ptz/preset/delete` - 删除预置位
- `POST /api/v1/ptz/cruise/start` - 启动巡航
- `POST /api/v1/ptz/cruise/stop` - 停止巡航
- `POST /api/v1/ptz/guard` - 设置看守位

---

## 🧪 测试示例

### PTZ控制测试
```bash
# 向上移动
curl -X POST http://localhost:8003/api/v1/ptz/control \
  -H "Content-Type: application/json" \
  -d '{
    "device_id": "34020000001320000001",
    "channel_id": "34020000001320000002",
    "direction": "UP",
    "h_speed": 128,
    "v_speed": 128
  }'

# 调用预置位
curl -X POST http://localhost:8003/api/v1/ptz/preset/call \
  -H "Content-Type: application/json" \
  -d '{
    "device_id": "34020000001320000001",
    "channel_id": "34020000001320000002",
    "preset_id": 1
  }'
```

### 设备信息查询
```bash
# 查询设备信息
curl http://localhost:8003/api/v1/device/34020000001320000001/info

# 查询设备状态
curl http://localhost:8003/api/v1/device/34020000001320000001/status
```

---

## 📖 开发文档

### Sprint文档
- [Sprint 1: 设备目录管理](实现思路.md)
- [Sprint 2: 设备信息查询](Sprint2_设备信息查询实现.md)
- [Sprint 3: 云台控制PTZ](Sprint3_云台控制PTZ实现.md)

### 技术文档
- [SIP处理层实现思路](SIP处理层实现思路.md)
- [快速开始指南](快速开始指南.md)
- [实现进度总览](实现进度_总览.md)

### 集成指南
- [Sprint 1集成指南](集成指南.md)
- [Sprint 2集成指南](Sprint2_集成指南.md)
- [Sprint 3集成指南](Sprint3_集成指南.md)

---

## 🗓️ 开发路线图

### 已完成 ✅
- [x] Sprint 1: 设备目录管理 (2026-02-09)
- [x] Sprint 2: 设备信息查询 (2026-02-09)
- [x] Sprint 3: 云台控制PTZ (2026-02-09)

### 进行中 🟡
- [ ] Sprint 4: 录像管理 (预计3周)

### 计划中 ⏳
- [ ] Sprint 5: 报警管理 (预计2周)
- [ ] Sprint 6: 语音对讲 (预计2周)
- [ ] Sprint 7: 平台级联 (预计2周)
- [ ] Sprint 8: 移动位置订阅 (预计1周)
- [ ] Sprint 9: 设备配置管理 (预计1.5周)
- [ ] Sprint 10: 设备控制 (预计1.5周)

---

## 🤝 贡献指南

### 开发流程
1. Fork项目
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 开启Pull Request

### 代码规范
- 遵循C++11/14标准
- 使用4空格缩进
- 类名使用大驼峰命名
- 函数名使用小驼峰命名
- 添加必要的注释和文档

---

## 📄 许可证

本项目采用BSD许可证 - 详见 [LICENSE](LICENSE) 文件

---

## 📞 联系方式

- **项目主页**: https://chensongpoixs.github.io
- **问题反馈**: [GitHub Issues](https://github.com/your-repo/issues)
- **邮箱**: your-email@example.com

---

## 🙏 致谢

- [GB/T 28181-2016](http://www.gb688.cn/bzgk/gb/newGbInfo?hcno=469659DC56B9B8187671FF08748CEC89) - 国标协议标准
- [eXosip2](http://savannah.nongnu.org/projects/exosip) - SIP协议栈
- [oatpp](https://oatpp.io/) - 现代C++ Web框架
- [SQLite](https://www.sqlite.org/) - 嵌入式数据库

---

**最后更新**: 2026-02-09  
**版本**: v0.35  
**状态**: 🟢 活跃开发中
