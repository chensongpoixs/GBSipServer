/*
*  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source tree.
*/
/*****************************************************************************
				  Author: chensong
				  date:  2026-02-09
******************************************************************************/

#include "XmlHelper.hpp"
#include <sstream>
#include <cstring>
#include <atomic>
#include <ctime>

namespace gbsip_server {

// 简单的XML解析实现（基于字符串解析）
std::shared_ptr<XmlNode> XmlHelper::parseXml(const std::string& xml) {
    if (xml.empty()) {
        return nullptr;
    }
    
    size_t pos = 0;
    const char* data = xml.c_str();
    
    // 跳过XML声明
    if (xml.find("<?xml") == 0) {
        pos = xml.find("?>") + 2;
    }
    
    skipWhitespace(data, pos);
    return parseNode(data, pos);
}

std::shared_ptr<XmlNode> XmlHelper::parseNode(const char* data, size_t& pos) {
    skipWhitespace(data, pos);
    
    if (data[pos] != '<') {
        return nullptr;
    }
    
    pos++; // 跳过 '<'
    
    // 读取标签名
    std::string tagName = readTagName(data, pos);
    if (tagName.empty()) {
        return nullptr;
    }
    
    auto node = std::make_shared<XmlNode>();
    node->name = tagName;
    
    // 跳过到 '>'
    while (data[pos] && data[pos] != '>') {
        pos++;
    }
    if (data[pos] == '>') {
        pos++;
    }
    
    // 读取内容
    skipWhitespace(data, pos);
    
    if (data[pos] == '<') {
        // 有子节点
        while (data[pos] == '<' && data[pos + 1] != '/') {
            auto child = parseNode(data, pos);
            if (child) {
                node->children.push_back(child);
            }
            skipWhitespace(data, pos);
        }
    } else {
        // 读取文本值
        node->value = readTagValue(data, pos);
    }
    
    // 跳过结束标签
    if (data[pos] == '<' && data[pos + 1] == '/') {
        pos += 2;
        while (data[pos] && data[pos] != '>') {
            pos++;
        }
        if (data[pos] == '>') {
            pos++;
        }
    }
    
    return node;
}

std::string XmlHelper::buildXml(const std::shared_ptr<XmlNode>& root) {
    if (!root) {
        return "";
    }
    
    std::string xml = "<?xml version=\"1.0\" encoding=\"GB2312\"?>\n";
    buildXmlRecursive(root, xml, 0);
    return xml;
}

void XmlHelper::buildXmlRecursive(const std::shared_ptr<XmlNode>& node,
                                   std::string& xml, int indent) {
    if (!node) {
        return;
    }
    
    // 缩进
    for (int i = 0; i < indent; i++) {
        xml += "    ";
    }
    
    // 开始标签
    xml += "<" + node->name;
    
    // 属性
    for (const auto& attr : node->attributes) {
        xml += " " + attr.first + "=\"" + escapeXml(attr.second) + "\"";
    }
    
    if (node->children.empty() && node->value.empty()) {
        xml += "/>\n";
        return;
    }
    
    xml += ">";
    
    // 值或子节点
    if (!node->value.empty()) {
        xml += escapeXml(node->value);
    } else if (!node->children.empty()) {
        xml += "\n";
        for (const auto& child : node->children) {
            buildXmlRecursive(child, xml, indent + 1);
        }
        for (int i = 0; i < indent; i++) {
            xml += "    ";
        }
    }
    
    // 结束标签
    xml += "</" + node->name + ">\n";
}

std::string XmlHelper::getNodeValue(const std::shared_ptr<XmlNode>& node,
                                     const std::string& path) {
    if (!node || path.empty()) {
        return "";
    }
    
    size_t pos = path.find('/');
    if (pos == std::string::npos) {
        // 最后一级
        auto child = getChildNode(node, path);
        return child ? child->value : "";
    }
    
    // 递归查找
    std::string first = path.substr(0, pos);
    std::string rest = path.substr(pos + 1);
    auto child = getChildNode(node, first);
    return child ? getNodeValue(child, rest) : "";
}

std::shared_ptr<XmlNode> XmlHelper::getChildNode(const std::shared_ptr<XmlNode>& node,
                                                   const std::string& name) {
    if (!node) {
        return nullptr;
    }
    
    for (const auto& child : node->children) {
        if (child->name == name) {
            return child;
        }
    }
    
    return nullptr;
}

std::vector<std::shared_ptr<XmlNode>> XmlHelper::getChildNodes(
    const std::shared_ptr<XmlNode>& node, const std::string& name) {
    std::vector<std::shared_ptr<XmlNode>> result;
    
    if (!node) {
        return result;
    }
    
    for (const auto& child : node->children) {
        if (child->name == name) {
            result.push_back(child);
        }
    }
    
    return result;
}

std::shared_ptr<XmlNode> XmlHelper::createNode(const std::string& name,
                                                 const std::string& value) {
    auto node = std::make_shared<XmlNode>();
    node->name = name;
    node->value = value;
    return node;
}

void XmlHelper::addChild(std::shared_ptr<XmlNode>& parent,
                         const std::shared_ptr<XmlNode>& child) {
    if (parent && child) {
        parent->children.push_back(child);
    }
}

void XmlHelper::setAttribute(std::shared_ptr<XmlNode>& node,
                             const std::string& name,
                             const std::string& value) {
    if (node) {
        node->attributes[name] = value;
    }
}

void XmlHelper::skipWhitespace(const char* data, size_t& pos) {
    while (data[pos] && (data[pos] == ' ' || data[pos] == '\t' ||
                         data[pos] == '\n' || data[pos] == '\r')) {
        pos++;
    }
}

std::string XmlHelper::readTagName(const char* data, size_t& pos) {
    std::string name;
    while (data[pos] && data[pos] != ' ' && data[pos] != '>' && data[pos] != '/') {
        name += data[pos++];
    }
    return name;
}

std::string XmlHelper::readTagValue(const char* data, size_t& pos) {
    std::string value;
    while (data[pos] && data[pos] != '<') {
        value += data[pos++];
    }
    
    // 去除首尾空白
    size_t start = value.find_first_not_of(" \t\n\r");
    size_t end = value.find_last_not_of(" \t\n\r");
    if (start != std::string::npos && end != std::string::npos) {
        value = value.substr(start, end - start + 1);
    }
    
    return unescapeXml(value);
}

std::string XmlHelper::escapeXml(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '<': result += "&lt;"; break;
            case '>': result += "&gt;"; break;
            case '&': result += "&amp;"; break;
            case '"': result += "&quot;"; break;
            case '\'': result += "&apos;"; break;
            default: result += c;
        }
    }
    return result;
}

std::string XmlHelper::unescapeXml(const std::string& str) {
    std::string result = str;
    size_t pos = 0;
    while ((pos = result.find("&lt;", pos)) != std::string::npos) {
        result.replace(pos, 4, "<");
        pos++;
    }
    pos = 0;
    while ((pos = result.find("&gt;", pos)) != std::string::npos) {
        result.replace(pos, 4, ">");
        pos++;
    }
    pos = 0;
    while ((pos = result.find("&amp;", pos)) != std::string::npos) {
        result.replace(pos, 5, "&");
        pos++;
    }
    pos = 0;
    while ((pos = result.find("&quot;", pos)) != std::string::npos) {
        result.replace(pos, 6, "\"");
        pos++;
    }
    pos = 0;
    while ((pos = result.find("&apos;", pos)) != std::string::npos) {
        result.replace(pos, 6, "'");
        pos++;
    }
    return result;
}

// GB28181XmlBuilder 实现

int GB28181XmlBuilder::generateSN() {
    static std::atomic<int> sn(1);
    return sn.fetch_add(1);
}

std::string GB28181XmlBuilder::buildCatalogQuery(const std::string& deviceId, int sn) {
    auto root = XmlHelper::createNode("Query");
    XmlHelper::addChild(root, XmlHelper::createNode("CmdType", "Catalog"));
    XmlHelper::addChild(root, XmlHelper::createNode("SN", std::to_string(sn)));
    XmlHelper::addChild(root, XmlHelper::createNode("DeviceID", deviceId));
    
    return XmlHelper::buildXml(root);
}

std::string GB28181XmlBuilder::buildDeviceInfoQuery(const std::string& deviceId, int sn) {
    auto root = XmlHelper::createNode("Query");
    XmlHelper::addChild(root, XmlHelper::createNode("CmdType", "DeviceInfo"));
    XmlHelper::addChild(root, XmlHelper::createNode("SN", std::to_string(sn)));
    XmlHelper::addChild(root, XmlHelper::createNode("DeviceID", deviceId));
    
    return XmlHelper::buildXml(root);
}

std::string GB28181XmlBuilder::buildDeviceStatusQuery(const std::string& deviceId, int sn) {
    auto root = XmlHelper::createNode("Query");
    XmlHelper::addChild(root, XmlHelper::createNode("CmdType", "DeviceStatus"));
    XmlHelper::addChild(root, XmlHelper::createNode("SN", std::to_string(sn)));
    XmlHelper::addChild(root, XmlHelper::createNode("DeviceID", deviceId));
    
    return XmlHelper::buildXml(root);
}

std::string GB28181XmlBuilder::buildPTZControl(const std::string& deviceId,
                                                const std::string& channelId,
                                                const std::string& ptzCmd) {
    auto root = XmlHelper::createNode("Control");
    XmlHelper::addChild(root, XmlHelper::createNode("CmdType", "DeviceControl"));
    XmlHelper::addChild(root, XmlHelper::createNode("SN", std::to_string(generateSN())));
    XmlHelper::addChild(root, XmlHelper::createNode("DeviceID", deviceId));
    XmlHelper::addChild(root, XmlHelper::createNode("PTZCmd", ptzCmd));
    
    return XmlHelper::buildXml(root);
}

std::string GB28181XmlBuilder::buildRecordInfoQuery(const std::string& deviceId,
                                                     const std::string& channelId,
                                                     const std::string& startTime,
                                                     const std::string& endTime,
                                                     int sn,
                                                     const std::string& type) {
    auto root = XmlHelper::createNode("Query");
    XmlHelper::addChild(root, XmlHelper::createNode("CmdType", "RecordInfo"));
    XmlHelper::addChild(root, XmlHelper::createNode("SN", std::to_string(sn)));
    XmlHelper::addChild(root, XmlHelper::createNode("DeviceID", channelId));
    XmlHelper::addChild(root, XmlHelper::createNode("StartTime", startTime));
    XmlHelper::addChild(root, XmlHelper::createNode("EndTime", endTime));
    XmlHelper::addChild(root, XmlHelper::createNode("Type", type));
    
    return XmlHelper::buildXml(root);
}

} // namespace gbsip_server
