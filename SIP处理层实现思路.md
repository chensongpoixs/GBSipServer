# SIP处理层和集成测试实现思路

## 一、SIP处理层架构设计

### 1.1 整体架构

```
┌─────────────────────────────────────────────────────────┐
│                    SipServer                            │
│  ┌──────────────────────────────────────────────────┐  │
│  │         SIP Event Loop (eXosip)                  │  │
│  └────────────────┬─────────────────────────────────┘  │
│                   │                                     │
│  ┌────────────────▼─────────────────────────────────┐  │
│  │      SipMessageHandler (基类)                    │  │
│  │  - sendMessage()                                 │  │
│  │  - sendSubscribe()                               │  │
│  │  - handleResponse()                              │  │
│  │  - handleTimeout()                               │  │
│  └────────────────┬─────────────────────────────────┘  │
│                   │                                     │
│         ┌─────────┴─────────┬──────────────┐          │
│         │                   │              │          │
│  ┌──────▼──────┐   ┌───────▼──────┐  ┌───▼──────┐   │
│  │ Catalog     │   │ DeviceInfo   │  │ PTZ      │   │
│  │ Handler     │   │ Handler      │  │ Handler  │   │
│  └─────────────┘   └──────────────┘  └──────────┘   │
└─────────────────────────────────────────────────────────┘
```

### 1.2 核心类设计

#### 1.2.1 SipMessageHandler（基类）
**职责**：
- 封装eXosip的MESSAGE和SUBSCRIBE发送
- 管理请求-响应映射（SN -> Callback）
- 处理超时机制
- 提供统一的错误处理

**关键方法**：
```cpp
class SipMessageHandler {
protected:
    // 发送MESSAGE消息
    int32_t sendMessage(const std::string& deviceId, 
                        const std::string& xml,
                        ResponseCallback callback);
    
    // 发送SUBSCRIBE消息
    int32_t sendSubscribe(const std::string& deviceId,
                          int expires,
                          ResponseCallback callback);
    
    // 处理响应
    void handleResponse(eXosip_event_t* event);
    
    // 处理超时
    void handleTimeout(const std::string& sn);
    
    // 生成序列号
    std::string generateSN();
    
private:
    // SN -> Callback映射
    std::unordered_map<std::string, ResponseCallback> callbacks_;
    
    // SN -> Timer映射
    std::unordered_map<std::string, std::shared_ptr<Timer>> timers_;
    
    // eXosip上下文
    eXosip_t* sip_context_;
};
```

#### 1.2.2 CatalogHandler（目录处理器）
**职责**：
- 发送目录查询MESSAGE
- 解析目录查询响应XML
- 发送目录订阅SUBSCRIBE
- 处理目录通知NOTIFY
- 更新数据库中的通道信息

**关键方法**：
```cpp
class CatalogHandler : public SipMessageHandler {
public:
    // 查询设备目录
    void queryCatalog(const std::string& deviceId,
                      std::function<void(const CatalogResponse&)> onSuccess,
                      std::function<void(int, const std::string&)> onError);
    
    // 订阅目录
    void subscribeCatalog(const std::string& deviceId,
                          int expires,
                          std::function<void()> onSuccess,
                          std::function<void(int, const std::string&)> onError);
    
    // 处理目录响应
    void handleCatalogResponse(eXosip_event_t* event);
    
    // 处理目录通知
    void handleCatalogNotify(eXosip_event_t* event);
    
private:
    // 解析目录XML
    CatalogResponse parseCatalogXml(const std::string& xml);
    
    // 保存通道到数据库
    void saveChannels(const std::string& deviceId,
                      const std::vector<ChannelInfo>& channels);
};
```

### 1.3 数据流设计

```
HTTP API请求
    ↓
CatalogService::queryCatalog()
    ↓
CatalogHandler::queryCatalog()
    ↓
SipMessageHandler::sendMessage()
    ↓
eXosip_message_send_request()
    ↓
[等待设备响应]
    ↓
SipServer::HandlerSipMessageAnswerd()
    ↓
CatalogHandler::handleCatalogResponse()
    ↓
parseCatalogXml()
    ↓
saveChannels()
    ↓
callback(success)
    ↓
HTTP API响应
```

## 二、关键技术实现

### 2.1 请求-响应映射机制

**问题**：如何将异步的SIP响应与原始请求关联？

**解决方案**：使用SN（序列号）作为唯一标识

```cpp
// 发送请求时
std::string sn = generateSN();
callbacks_[sn] = callback;
timers_[sn] = startTimer(30, [this, sn]() {
    handleTimeout(sn);
});

// 收到响应时
std::string sn = extractSN(xml);
auto it = callbacks_.find(sn);
if (it != callbacks_.end()) {
    it->second(response);
    callbacks_.erase(it);
    timers_.erase(sn);
}
```

### 2.2 超时处理机制

**实现方式**：使用定时器

```cpp
class Timer {
public:
    void start(int seconds, std::function<void()> callback) {
        thread_ = std::thread([seconds, callback]() {
            std::this_thread::sleep_for(std::chrono::seconds(seconds));
            callback();
        });
        thread_.detach();
    }
};

void handleTimeout(const std::string& sn) {
    auto it = callbacks_.find(sn);
    if (it != callbacks_.end()) {
        it->second.onError(408, "Request Timeout");
        callbacks_.erase(it);
        timers_.erase(sn);
    }
}
```

### 2.3 XML消息构建

**目录查询MESSAGE**：
```xml
<?xml version="1.0" encoding="GB2312"?>
<Query>
    <CmdType>Catalog</CmdType>
    <SN>12345</SN>
    <DeviceID>34020000001320000001</DeviceID>
</Query>
```

**SIP消息格式**：
```
MESSAGE sip:34020000001320000001@192.168.1.100:5060 SIP/2.0
Via: SIP/2.0/UDP 192.168.1.1:5060;branch=z9hG4bK...
From: <sip:41010500002000000001@192.168.1.1:5060>;tag=...
To: <sip:34020000001320000001@192.168.1.100:5060>
Call-ID: ...
CSeq: 1 MESSAGE
Content-Type: Application/MANSCDP+xml
Content-Length: 123

<?xml version="1.0" encoding="GB2312"?>
<Query>...</Query>
```

### 2.4 XML响应解析

**目录响应XML**：
```xml
<?xml version="1.0" encoding="GB2312"?>
<Response>
    <CmdType>Catalog</CmdType>
    <SN>12345</SN>
    <DeviceID>34020000001320000001</DeviceID>
    <SumNum>2</SumNum>
    <DeviceList Num="2">
        <Item>
            <DeviceID>34020000001320000002</DeviceID>
            <Name>Camera 1</Name>
            <Manufacturer>Hikvision</Manufacturer>
            <Model>DS-2CD2T47EWD-L</Model>
            <Status>ON</Status>
            <Longitude>116.397128</Longitude>
            <Latitude>39.916527</Latitude>
            <PTZType>1</PTZType>
        </Item>
    </DeviceList>
</Response>
```

**解析流程**：
```cpp
CatalogResponse parseCatalogXml(const std::string& xml) {
    auto root = XmlHelper::parseXml(xml);
    
    CatalogResponse response;
    response.cmdType = XmlHelper::getNodeValue(root, "Response/CmdType");
    response.sn = XmlHelper::getNodeValue(root, "Response/SN");
    response.deviceId = XmlHelper::getNodeValue(root, "Response/DeviceID");
    response.sumNum = std::stoi(XmlHelper::getNodeValue(root, "Response/SumNum"));
    
    auto deviceList = XmlHelper::getChildNode(root, "DeviceList");
    auto items = XmlHelper::getChildNodes(deviceList, "Item");
    
    for (const auto& item : items) {
        ChannelInfo channel;
        channel.channelId = XmlHelper::getNodeValue(item, "DeviceID");
        channel.name = XmlHelper::getNodeValue(item, "Name");
        channel.manufacturer = XmlHelper::getNodeValue(item, "Manufacturer");
        channel.model = XmlHelper::getNodeValue(item, "Model");
        channel.status = XmlHelper::getNodeValue(item, "Status");
        // ... 解析其他字段
        
        response.channels.push_back(channel);
    }
    
    return response;
}
```

## 三、集成到SipServer

### 3.1 修改sip_server.h

```cpp
class SipServer {
private:
    // 添加Handler成员
    std::shared_ptr<CatalogHandler> catalog_handler_;
    std::shared_ptr<DeviceInfoHandler> device_info_handler_;
    
    // 添加回调管理
    std::unordered_map<std::string, std::function<void(const std::string&)>> 
        message_callbacks_;
    
public:
    // 获取Handler
    std::shared_ptr<CatalogHandler> getCatalogHandler() {
        return catalog_handler_;
    }
};
```

### 3.2 修改sip_server.cpp

```cpp
SipServer::SipServer() {
    // 初始化Handler
    catalog_handler_ = std::make_shared<CatalogHandler>(sip_context_);
    device_info_handler_ = std::make_shared<DeviceInfoHandler>(sip_context_);
}

void SipServer::HandlerSipMessageAnswerd(eXosip_event_t* sip_event) {
    if (!sip_event || !sip_event->request) {
        return;
    }
    
    // 获取消息体
    osip_body_t* body = nullptr;
    osip_message_get_body(sip_event->request, 0, &body);
    if (!body || !body->body) {
        return;
    }
    
    std::string xml(body->body);
    
    // 解析CmdType
    auto root = XmlHelper::parseXml(xml);
    std::string cmdType = XmlHelper::getNodeValue(root, "Response/CmdType");
    
    // 分发到对应的Handler
    if (cmdType == "Catalog") {
        catalog_handler_->handleCatalogResponse(sip_event);
    } else if (cmdType == "DeviceInfo") {
        device_info_handler_->handleDeviceInfoResponse(sip_event);
    }
    // ... 其他类型
}
```

### 3.3 修改CatalogService

```cpp
oatpp::Object<CatalogResponseDto> CatalogService::queryCatalog(
    const oatpp::String& deviceId) {
    
    // 检查设备是否存在
    auto device = m_deviceDb->getDeviceByDeviceId(deviceId);
    if (!device) {
        return nullptr;
    }
    
    // 创建Promise用于同步等待
    auto promise = std::make_shared<std::promise<CatalogResponse>>();
    auto future = promise->get_future();
    
    // 调用CatalogHandler查询
    auto& sipServer = SipServer::GetInstance();
    auto catalogHandler = sipServer.getCatalogHandler();
    
    catalogHandler->queryCatalog(
        deviceId->c_str(),
        [promise](const CatalogResponse& response) {
            promise->set_value(response);
        },
        [promise](int code, const std::string& message) {
            promise->set_exception(
                std::make_exception_ptr(std::runtime_error(message))
            );
        }
    );
    
    // 等待响应（超时30秒）
    auto status = future.wait_for(std::chrono::seconds(30));
    if (status == std::future_status::timeout) {
        return nullptr;
    }
    
    try {
        auto response = future.get();
        
        // 保存到数据库
        for (const auto& channel : response.channels) {
            saveOrUpdateChannel(channel);
        }
        
        // 构建响应DTO
        auto dto = CatalogResponseDto::createShared();
        dto->deviceId = deviceId;
        dto->sn = response.sn;
        dto->sumNum = response.sumNum;
        // ... 填充channels
        
        return dto;
    } catch (const std::exception& e) {
        return nullptr;
    }
}
```

## 四、集成测试设计

### 4.1 测试环境准备

#### 4.1.1 GB28181模拟器
使用开源模拟器：
- [GB28181-Server](https://github.com/GB28181/GB28181.Solution)
- [SIPp](http://sipp.sourceforge.net/) - SIP压力测试工具

#### 4.1.2 测试设备配置
```yaml
# 模拟设备配置
device:
  id: "34020000001320000001"
  name: "Test Device"
  ip: "192.168.1.100"
  port: 5060
  channels:
    - id: "34020000001320000002"
      name: "Camera 1"
      status: "ON"
    - id: "34020000001320000003"
      name: "Camera 2"
      status: "ON"
```

### 4.2 测试用例设计

#### 4.2.1 单元测试

**测试XmlHelper**：
```cpp
TEST(XmlHelperTest, ParseCatalogXml) {
    std::string xml = R"(
        <?xml version="1.0" encoding="GB2312"?>
        <Response>
            <CmdType>Catalog</CmdType>
            <SN>12345</SN>
            <DeviceID>34020000001320000001</DeviceID>
            <SumNum>1</SumNum>
            <DeviceList Num="1">
                <Item>
                    <DeviceID>34020000001320000002</DeviceID>
                    <Name>Camera 1</Name>
                </Item>
            </DeviceList>
        </Response>
    )";
    
    auto root = XmlHelper::parseXml(xml);
    ASSERT_NE(root, nullptr);
    
    std::string cmdType = XmlHelper::getNodeValue(root, "Response/CmdType");
    EXPECT_EQ(cmdType, "Catalog");
}
```

**测试CatalogHandler**：
```cpp
TEST(CatalogHandlerTest, QueryCatalog) {
    auto handler = std::make_shared<CatalogHandler>(sip_context);
    
    bool success = false;
    handler->queryCatalog(
        "34020000001320000001",
        [&success](const CatalogResponse& response) {
            success = true;
            EXPECT_GT(response.channels.size(), 0);
        },
        [](int code, const std::string& message) {
            FAIL() << "Query failed: " << message;
        }
    );
    
    // 等待响应
    std::this_thread::sleep_for(std::chrono::seconds(5));
    EXPECT_TRUE(success);
}
```

#### 4.2.2 集成测试

**测试完整流程**：
```cpp
TEST(IntegrationTest, CatalogQueryFlow) {
    // 1. 启动SipServer
    SipServer& server = SipServer::GetInstance();
    server.Start();
    
    // 2. 注册测试设备
    // ... 设备注册逻辑
    
    // 3. 调用API查询目录
    auto response = httpClient->get("/api/v1/device/34020000001320000001/catalog");
    EXPECT_EQ(response->getStatusCode(), 200);
    
    // 4. 验证响应
    auto json = response->readBodyToString();
    // ... 验证JSON内容
    
    // 5. 验证数据库
    auto channels = channelDb->getChannelsByDeviceId("34020000001320000001");
    EXPECT_GT(channels->size(), 0);
}
```

### 4.3 性能测试

**并发查询测试**：
```cpp
TEST(PerformanceTest, ConcurrentQuery) {
    const int CONCURRENT_COUNT = 100;
    std::vector<std::thread> threads;
    std::atomic<int> successCount(0);
    
    for (int i = 0; i < CONCURRENT_COUNT; i++) {
        threads.emplace_back([&successCount]() {
            auto response = httpClient->get("/api/v1/device/.../catalog");
            if (response->getStatusCode() == 200) {
                successCount++;
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_EQ(successCount, CONCURRENT_COUNT);
}
```

### 4.4 测试工具

#### 4.4.1 Postman测试集合
```json
{
  "info": {
    "name": "GB28181 API Tests"
  },
  "item": [
    {
      "name": "Query Catalog",
      "request": {
        "method": "GET",
        "url": "http://localhost:8003/api/v1/device/34020000001320000001/catalog"
      },
      "test": "pm.test('Status is 200', () => { pm.response.to.have.status(200); });"
    }
  ]
}
```

#### 4.4.2 SIP消息抓包分析
使用Wireshark过滤器：
```
sip && (sip.Method == "MESSAGE" || sip.CSeq.Method == "MESSAGE")
```

## 五、错误处理和日志

### 5.1 错误码定义

```cpp
enum class GB28181ErrorCode {
    SUCCESS = 0,
    DEVICE_NOT_FOUND = 404,
    REQUEST_TIMEOUT = 408,
    XML_PARSE_ERROR = 500,
    SIP_SEND_ERROR = 501,
    DATABASE_ERROR = 502
};
```

### 5.2 日志记录

```cpp
// 发送请求
SIPSERVER_LOG(LS_INFO) << "Send catalog query to device: " << deviceId 
                       << ", SN: " << sn;

// 收到响应
SIPSERVER_LOG(LS_INFO) << "Received catalog response from device: " << deviceId
                       << ", channels: " << channels.size();

// 错误处理
SIPSERVER_LOG(LS_ERROR) << "Catalog query timeout, device: " << deviceId
                        << ", SN: " << sn;
```

## 六、实现步骤总结

### 步骤1: 创建SIP处理基类（1天）
- [ ] SipMessageHandler.hpp
- [ ] SipMessageHandler.cpp
- [ ] 实现sendMessage和sendSubscribe
- [ ] 实现超时机制

### 步骤2: 创建目录处理器（1天）
- [ ] CatalogHandler.hpp
- [ ] CatalogHandler.cpp
- [ ] 实现queryCatalog
- [ ] 实现XML解析

### 步骤3: 集成到SipServer（0.5天）
- [ ] 修改sip_server.h
- [ ] 修改sip_server.cpp
- [ ] 修改CatalogService

### 步骤4: 单元测试（0.5天）
- [ ] XmlHelper测试
- [ ] CatalogHandler测试

### 步骤5: 集成测试（1天）
- [ ] 完整流程测试
- [ ] 性能测试
- [ ] 错误场景测试

**总计**: 4天

---

**下一步**: 开始编写代码实现
