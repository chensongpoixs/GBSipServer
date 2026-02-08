/*
*  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source tree.
*/
/*****************************************************************************
				  Author: chensong
				  date:  2026-02-09
				  
				  目录查询SIP处理器
******************************************************************************/

#ifndef _C_CATALOG_HANDLER_HPP
#define _C_CATALOG_HANDLER_HPP

#include "SipMessageHandler.hpp"
#include "dto/ChannelDto.hpp"
#include <vector>

namespace gbsip_server {

/**
 * 通道信息结构
 */
struct ChannelInfo {
    std::string channelId;
    std::string name;
    std::string manufacturer;
    std::string model;
    std::string owner;
    std::string civilCode;
    std::string address;
    int parental;
    std::string parentId;
    int safetyWay;
    int registerWay;
    int secrecy;
    std::string status;
    double longitude;
    double latitude;
    int ptzType;
};

/**
 * 目录响应结构
 */
struct CatalogResponse {
    std::string cmdType;
    std::string sn;
    std::string deviceId;
    int sumNum;
    std::vector<ChannelInfo> channels;
};

/**
 * 目录查询SIP处理器
 * 负责发送目录查询MESSAGE、解析响应、处理订阅和通知
 */
class CatalogHandler : public SipMessageHandler {
public:
    /**
     * 构造函数
     */
    CatalogHandler(eXosip_t* context,
                   const std::string& serverId,
                   const std::string& serverIp,
                   int serverPort);
    
    /**
     * 析构函数
     */
    virtual ~CatalogHandler();
    
    /**
     * 查询设备目录
     * @param deviceId 设备ID
     * @param deviceIp 设备IP
     * @param devicePort 设备端口
     * @param onSuccess 成功回调
     * @param onError 错误回调
     * @return 序列号
     */
    std::string queryCatalog(const std::string& deviceId,
                             const std::string& deviceIp,
                             int devicePort,
                             std::function<void(const CatalogResponse&)> onSuccess,
                             std::function<void(int, const std::string&)> onError);
    
    /**
     * 订阅设备目录
     * @param deviceId 设备ID
     * @param deviceIp 设备IP
     * @param devicePort 设备端口
     * @param expires 过期时间（秒）
     * @param onSuccess 成功回调
     * @param onError 错误回调
     * @return 订阅ID
     */
    int subscribeCatalog(const std::string& deviceId,
                         const std::string& deviceIp,
                         int devicePort,
                         int expires,
                         std::function<void()> onSuccess,
                         std::function<void(int, const std::string&)> onError);
    
    /**
     * 处理目录通知（NOTIFY）
     * @param event eXosip事件
     */
    virtual void handleNotify(eXosip_event_t* event) override;
    
    /**
     * 设置目录通知回调
     * @param callback 通知回调函数
     */
    void setNotifyCallback(std::function<void(const std::string& deviceId,
                                               const std::vector<ChannelInfo>& channels)> callback);

private:
    /**
     * 解析目录XML响应
     * @param xml XML字符串
     * @return 目录响应结构
     */
    CatalogResponse parseCatalogXml(const std::string& xml);
    
    /**
     * 解析通道信息
     * @param itemNode Item节点
     * @return 通道信息
     */
    ChannelInfo parseChannelInfo(const std::shared_ptr<XmlNode>& itemNode);

private:
    // 目录通知回调
    std::function<void(const std::string& deviceId,
                       const std::vector<ChannelInfo>& channels)> notify_callback_;
    std::mutex notify_callback_mutex_;
};

} // namespace gbsip_server

#endif /* _C_CATALOG_HANDLER_HPP */
