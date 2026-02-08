/*
*  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source tree.
*/
/*****************************************************************************
				  Author: chensong
				  date:  2026-02-09
				  
				  目录查询集成测试
******************************************************************************/

#include <gtest/gtest.h>
#include "utils/XmlHelper.hpp"
#include "sip/CatalogHandler.hpp"
#include <thread>
#include <chrono>

using namespace gbsip_server;

/**
 * XML解析测试
 */
class XmlHelperTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 准备测试数据
    }
    
    void TearDown() override {
        // 清理测试数据
    }
};

TEST_F(XmlHelperTest, ParseSimpleXml) {
    std::string xml = R"(
        <?xml version="1.0" encoding="GB2312"?>
        <Query>
            <CmdType>Catalog</CmdType>
            <SN>12345</SN>
            <DeviceID>34020000001320000001</DeviceID>
        </Query>
    )";
    
    auto root = XmlHelper::parseXml(xml);
    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->name, "Query");
    
    std::string cmdType = XmlHelper::getNodeValue(root, "Query/CmdType");
    EXPECT_EQ(cmdType, "Catalog");
    
    std::string sn = XmlHelper::getNodeValue(root, "Query/SN");
    EXPECT_EQ(sn, "12345");
    
    std::string deviceId = XmlHelper::getNodeValue(root, "Query/DeviceID");
    EXPECT_EQ(deviceId, "34020000001320000001");
}

TEST_F(XmlHelperTest, ParseCatalogResponse) {
    std::string xml = R"(
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
                <Item>
                    <DeviceID>34020000001320000003</DeviceID>
                    <Name>Camera 2</Name>
                    <Status>OFF</Status>
                </Item>
            </DeviceList>
        </Response>
    )";
    
    auto root = XmlHelper::parseXml(xml);
    ASSERT_NE(root, nullptr);
    
    std::string cmdType = XmlHelper::getNodeValue(root, "Response/CmdType");
    EXPECT_EQ(cmdType, "Catalog");
    
    std::string sumNum = XmlHelper::getNodeValue(root, "Response/SumNum");
    EXPECT_EQ(sumNum, "2");
    
    auto responseNode = XmlHelper::getChildNode(root, "Response");
    ASSERT_NE(responseNode, nullptr);
    
    auto deviceListNode = XmlHelper::getChildNode(responseNode, "DeviceList");
    ASSERT_NE(deviceListNode, nullptr);
    
    auto items = XmlHelper::getChildNodes(deviceListNode, "Item");
    EXPECT_EQ(items.size(), 2);
    
    // 验证第一个通道
    if (items.size() > 0) {
        std::string channelId = XmlHelper::getNodeValue(items[0], "DeviceID");
        EXPECT_EQ(channelId, "34020000001320000002");
        
        std::string name = XmlHelper::getNodeValue(items[0], "Name");
        EXPECT_EQ(name, "Camera 1");
        
        std::string manufacturer = XmlHelper::getNodeValue(items[0], "Manufacturer");
        EXPECT_EQ(manufacturer, "Hikvision");
    }
}

TEST_F(XmlHelperTest, BuildCatalogQuery) {
    std::string deviceId = "34020000001320000001";
    int sn = 12345;
    
    std::string xml = GB28181XmlBuilder::buildCatalogQuery(deviceId, sn);
    
    EXPECT_FALSE(xml.empty());
    EXPECT_NE(xml.find("<?xml"), std::string::npos);
    EXPECT_NE(xml.find("<Query>"), std::string::npos);
    EXPECT_NE(xml.find("<CmdType>Catalog</CmdType>"), std::string::npos);
    EXPECT_NE(xml.find("<SN>12345</SN>"), std::string::npos);
    EXPECT_NE(xml.find("<DeviceID>34020000001320000001</DeviceID>"), std::string::npos);
}

/**
 * CatalogHandler测试
 * 注意：这些测试需要真实的SIP环境或模拟器
 */
class CatalogHandlerTest : public ::testing::Test {
protected:
    eXosip_t* sip_context_;
    std::shared_ptr<CatalogHandler> handler_;
    
    void SetUp() override {
        // 初始化eXosip
        sip_context_ = eXosip_malloc();
        ASSERT_NE(sip_context_, nullptr);
        
        int ret = eXosip_init(sip_context_);
        ASSERT_EQ(ret, 0);
        
        ret = eXosip_listen_addr(sip_context_, IPPROTO_UDP, nullptr, 15060, AF_INET, 0);
        ASSERT_EQ(ret, 0);
        
        // 创建Handler
        handler_ = std::make_shared<CatalogHandler>(
            sip_context_,
            "41010500002000000001",
            "192.168.1.1",
            15060
        );
    }
    
    void TearDown() override {
        handler_.reset();
        
        if (sip_context_) {
            eXosip_quit(sip_context_);
            sip_context_ = nullptr;
        }
    }
};

TEST_F(CatalogHandlerTest, QueryCatalog) {
    // 这个测试需要真实的GB28181设备或模拟器
    // 这里只测试Handler的创建和基本功能
    
    ASSERT_NE(handler_, nullptr);
    
    bool callbackCalled = false;
    bool errorCalled = false;
    
    std::string sn = handler_->queryCatalog(
        "34020000001320000001",
        "192.168.1.100",
        5060,
        [&callbackCalled](const CatalogResponse& response) {
            callbackCalled = true;
            std::cout << "Received catalog response:" << std::endl;
            std::cout << "  Device ID: " << response.deviceId << std::endl;
            std::cout << "  SN: " << response.sn << std::endl;
            std::cout << "  Channels: " << response.channels.size() << std::endl;
        },
        [&errorCalled](int code, const std::string& message) {
            errorCalled = true;
            std::cout << "Query failed: " << code << " - " << message << std::endl;
        }
    );
    
    EXPECT_FALSE(sn.empty());
    
    // 等待响应（最多5秒）
    for (int i = 0; i < 50 && !callbackCalled && !errorCalled; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // 注意：如果没有真实设备，这个测试会超时
    // EXPECT_TRUE(callbackCalled || errorCalled);
}

/**
 * 性能测试
 */
class PerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(PerformanceTest, XmlParsePerformance) {
    std::string xml = R"(
        <?xml version="1.0" encoding="GB2312"?>
        <Response>
            <CmdType>Catalog</CmdType>
            <SN>12345</SN>
            <DeviceID>34020000001320000001</DeviceID>
            <SumNum>100</SumNum>
            <DeviceList Num="100">
    )";
    
    // 生成100个通道
    for (int i = 0; i < 100; i++) {
        xml += "<Item>";
        xml += "<DeviceID>3402000000132000000" + std::to_string(i) + "</DeviceID>";
        xml += "<Name>Camera " + std::to_string(i) + "</Name>";
        xml += "<Status>ON</Status>";
        xml += "</Item>";
    }
    
    xml += "</DeviceList></Response>";
    
    // 测试解析性能
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; i++) {
        auto root = XmlHelper::parseXml(xml);
        ASSERT_NE(root, nullptr);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Parse 1000 times: " << duration.count() << " ms" << std::endl;
    std::cout << "Average: " << duration.count() / 1000.0 << " ms per parse" << std::endl;
    
    // 性能要求：平均每次解析不超过10ms
    EXPECT_LT(duration.count() / 1000.0, 10.0);
}

/**
 * 主函数
 */
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
