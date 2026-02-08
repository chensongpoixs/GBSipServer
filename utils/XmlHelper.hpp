/*
*  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
*
*  Please visit https://chensongpoixs.github.io for detail
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source
*  tree. An additional intellectual property rights grant can be found
*  in the file PATENTS.  All contributing project authors may
*  be found in the AUTHORS file in the root of the source tree.
*/
/*****************************************************************************
				  Author: chensong
				  date:  2026-02-09
				  
				  GB28181 XML辅助工具类
******************************************************************************/

#ifndef _C_XML_HELPER_HPP
#define _C_XML_HELPER_HPP

#include <string>
#include <map>
#include <vector>
#include <memory>

namespace gbsip_server {

/**
 * XML节点结构
 */
struct XmlNode {
    std::string name;
    std::string value;
    std::map<std::string, std::string> attributes;
    std::vector<std::shared_ptr<XmlNode>> children;
};

/**
 * GB28181 XML辅助类
 * 提供XML解析和构建功能
 */
class XmlHelper {
public:
    /**
     * 解析XML字符串
     * @param xml XML字符串
     * @return 根节点
     */
    static std::shared_ptr<XmlNode> parseXml(const std::string& xml);
    
    /**
     * 构建XML字符串
     * @param root 根节点
     * @return XML字符串
     */
    static std::string buildXml(const std::shared_ptr<XmlNode>& root);
    
    /**
     * 获取节点值
     * @param node 父节点
     * @param path 节点路径（如 "Response/DeviceID"）
     * @return 节点值，不存在返回空字符串
     */
    static std::string getNodeValue(const std::shared_ptr<XmlNode>& node, 
                                     const std::string& path);
    
    /**
     * 获取子节点
     * @param node 父节点
     * @param name 子节点名称
     * @return 子节点，不存在返回nullptr
     */
    static std::shared_ptr<XmlNode> getChildNode(const std::shared_ptr<XmlNode>& node,
                                                   const std::string& name);
    
    /**
     * 获取所有子节点
     * @param node 父节点
     * @param name 子节点名称
     * @return 子节点列表
     */
    static std::vector<std::shared_ptr<XmlNode>> getChildNodes(
        const std::shared_ptr<XmlNode>& node, const std::string& name);
    
    /**
     * 创建节点
     * @param name 节点名称
     * @param value 节点值
     * @return 新节点
     */
    static std::shared_ptr<XmlNode> createNode(const std::string& name,
                                                 const std::string& value = "");
    
    /**
     * 添加子节点
     * @param parent 父节点
     * @param child 子节点
     */
    static void addChild(std::shared_ptr<XmlNode>& parent,
                         const std::shared_ptr<XmlNode>& child);
    
    /**
     * 设置节点属性
     * @param node 节点
     * @param name 属性名
     * @param value 属性值
     */
    static void setAttribute(std::shared_ptr<XmlNode>& node,
                             const std::string& name,
                             const std::string& value);

private:
    /**
     * 解析XML节点（递归）
     */
    static std::shared_ptr<XmlNode> parseNode(const char* data, size_t& pos);
    
    /**
     * 构建XML字符串（递归）
     */
    static void buildXmlRecursive(const std::shared_ptr<XmlNode>& node,
                                   std::string& xml, int indent = 0);
    
    /**
     * 跳过空白字符
     */
    static void skipWhitespace(const char* data, size_t& pos);
    
    /**
     * 读取标签名
     */
    static std::string readTagName(const char* data, size_t& pos);
    
    /**
     * 读取标签值
     */
    static std::string readTagValue(const char* data, size_t& pos);
    
    /**
     * XML转义
     */
    static std::string escapeXml(const std::string& str);
    
    /**
     * XML反转义
     */
    static std::string unescapeXml(const std::string& str);
};

/**
 * GB28181 XML消息构建器
 */
class GB28181XmlBuilder {
public:
    /**
     * 构建目录查询请求XML
     * @param deviceId 设备ID
     * @param sn 序列号
     * @return XML字符串
     */
    static std::string buildCatalogQuery(const std::string& deviceId, int sn);
    
    /**
     * 构建设备信息查询请求XML
     * @param deviceId 设备ID
     * @param sn 序列号
     * @return XML字符串
     */
    static std::string buildDeviceInfoQuery(const std::string& deviceId, int sn);
    
    /**
     * 构建设备状态查询请求XML
     * @param deviceId 设备ID
     * @param sn 序列号
     * @return XML字符串
     */
    static std::string buildDeviceStatusQuery(const std::string& deviceId, int sn);
    
    /**
     * 构建PTZ控制命令XML
     * @param deviceId 设备ID
     * @param channelId 通道ID
     * @param ptzCmd PTZ命令
     * @return XML字符串
     */
    static std::string buildPTZControl(const std::string& deviceId,
                                        const std::string& channelId,
                                        const std::string& ptzCmd);
    
    /**
     * 构建录像查询请求XML
     * @param deviceId 设备ID
     * @param channelId 通道ID
     * @param startTime 开始时间
     * @param endTime 结束时间
     * @param sn 序列号
     * @param type 录像类型（all/time/alarm/manual）
     * @return XML字符串
     */
    static std::string buildRecordInfoQuery(const std::string& deviceId,
                                             const std::string& channelId,
                                             const std::string& startTime,
                                             const std::string& endTime,
                                             int sn,
                                             const std::string& type = "all");

private:
    /**
     * 生成序列号
     */
    static int generateSN();
};

} // namespace gbsip_server

#endif /* _C_XML_HELPER_HPP */
