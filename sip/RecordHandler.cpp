/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Author: chensong
 *  Date: 2026-02-09
 *  Description: 录像SIP处理器实现
 ******************************************************************************/

#include "RecordHandler.hpp"
#include "gbsip_server_log.h"
#include <sstream>
#include <iomanip>
#include <ctime>

namespace gbsip_server {

RecordHandler::RecordHandler(eXosip_t* context,
                             const std::string& serverId,
                             const std::string& serverIp,
                             int serverPort)
    : sip_context_(context)
    , server_id_(serverId)
    , server_ip_(serverIp)
    , server_port_(serverPort)
    , sn_counter_(1)
{
    SIPSERVER_LOG(LS_INFO) << "RecordHandler initialized";
}

RecordHandler::~RecordHandler() {
    SIPSERVER_LOG(LS_INFO) << "RecordHandler destroyed";
}

std::string RecordHandler::queryRecordInfo(
    const std::string& deviceId,
    const std::string& channelId,
    const std::string& startTime,
    const std::string& endTime,
    const std::string& type,
    const std::string& deviceIp,
    int devicePort)
{
    // 构建RecordInfo查询XML
    std::string xml = buildRecordInfoXml(deviceId, channelId, startTime, endTime, type);
    
    // 发送MESSAGE请求
    std::string sn = sendMessage(deviceId, deviceIp, devicePort, xml);
    
    SIPSERVER_LOG(LS_INFO) << "Query record info sent, SN: " << sn;
    
    return sn;
}

int RecordHandler::startPlayback(
    const std::string& deviceId,
    const std::string& channelId,
    const std::string& startTime,
    const std::string& endTime,
    const std::string& localIp,
    int localPort,
    const std::string& deviceIp,
    int devicePort)
{
    // 构建SDP
    std::string sdp = buildPlaybackSdp(localIp, localPort, startTime, endTime);
    
    // 发送INVITE请求
    int callId = sendInvite(deviceId, channelId, deviceIp, devicePort, sdp);
    
    if (callId > 0) {
        SIPSERVER_LOG(LS_INFO) << "Playback started, call_id: " << callId;
    } else {
        SIPSERVER_LOG(LS_WARNING) << "Failed to start playback";
    }
    
    return callId;
}

bool RecordHandler::pausePlayback(
    const std::string& deviceId,
    const std::string& channelId,
    int callId,
    const std::string& deviceIp,
    int devicePort)
{
    std::string xml = buildPlaybackControlXml(deviceId, channelId, "Pause");
    return sendInfo(callId, xml);
}

bool RecordHandler::resumePlayback(
    const std::string& deviceId,
    const std::string& channelId,
    int callId,
    const std::string& deviceIp,
    int devicePort)
{
    std::string xml = buildPlaybackControlXml(deviceId, channelId, "Resume");
    return sendInfo(callId, xml);
}

bool RecordHandler::stopPlayback(int callId) {
    return sendBye(callId);
}

bool RecordHandler::seekPlayback(
    const std::string& deviceId,
    const std::string& channelId,
    int callId,
    const std::string& seekTime,
    const std::string& deviceIp,
    int devicePort)
{
    std::string xml = buildPlaybackControlXml(deviceId, channelId, "Seek", seekTime);
    return sendInfo(callId, xml);
}

bool RecordHandler::setPlaybackSpeed(
    const std::string& deviceId,
    const std::string& channelId,
    int callId,
    float speed,
    const std::string& deviceIp,
    int devicePort)
{
    std::string command = (speed > 1.0f) ? "Fast" : (speed < 1.0f) ? "Slow" : "Resume";
    std::string xml = buildPlaybackControlXml(deviceId, channelId, command, "", speed);
    return sendInfo(callId, xml);
}

int RecordHandler::startDownload(
    const std::string& deviceId,
    const std::string& channelId,
    const std::string& startTime,
    const std::string& endTime,
    const std::string& localIp,
    int localPort,
    const std::string& deviceIp,
    int devicePort)
{
    // 下载使用与回放相同的INVITE流程，但SDP中的s=字段不同
    std::string sdp = buildPlaybackSdp(localIp, localPort, startTime, endTime);
    // 将s=Play改为s=Download
    size_t pos = sdp.find("s=Play");
    if (pos != std::string::npos) {
        sdp.replace(pos, 6, "s=Download");
    }
    
    int callId = sendInvite(deviceId, channelId, deviceIp, devicePort, sdp);
    
    if (callId > 0) {
        SIPSERVER_LOG(LS_INFO) << "Download started, call_id: " << callId;
    } else {
        SIPSERVER_LOG(LS_WARNING) << "Failed to start download";
    }
    
    return callId;
}

bool RecordHandler::stopDownload(int callId) {
    return sendBye(callId);
}

void RecordHandler::handleMessageResponse(eXosip_event_t* event) {
    if (!event || !event->response) {
        return;
    }
    
    osip_body_t* body = nullptr;
    osip_message_get_body(event->response, 0, &body);
    if (!body || !body->body) {
        return;
    }
    
    std::string xml(body->body);
    auto root = XmlHelper::parseXml(xml);
    if (!root) {
        SIPSERVER_LOG(LS_WARNING) << "Failed to parse XML response";
        return;
    }
    
    std::string cmdType = XmlHelper::getNodeValue(root, "Response/CmdType");
    std::string sn = XmlHelper::getNodeValue(root, "Response/SN");
    
    if (cmdType == "RecordInfo") {
        auto it = query_callbacks_.find(sn);
        if (it != query_callbacks_.end()) {
            try {
                auto records = parseRecordInfoXml(xml);
                
                // 调用成功回调
                if (it->second.onSuccess) {
                    it->second.onSuccess(records);
                }
                
                // 设置Promise值
                if (it->second.promise) {
                    it->second.promise->set_value(records);
                }
                
                SIPSERVER_LOG(LS_INFO) << "RecordInfo response processed, SN: " << sn 
                                       << ", count: " << records.size();
            } catch (const std::exception& e) {
                SIPSERVER_LOG(LS_WARNING) << "Failed to parse RecordInfo: " << e.what();
                
                // 调用错误回调
                if (it->second.onError) {
                    it->second.onError(500, e.what());
                }
                
                // 设置Promise异常
                if (it->second.promise) {
                    it->second.promise->set_exception(std::current_exception());
                }
            }
            
            // 移除回调
            query_callbacks_.erase(it);
        }
    }
}

void RecordHandler::registerQueryCallback(
    const std::string& sn,
    std::function<void(const std::vector<oatpp::Object<RecordItemDto>>&)> onSuccess,
    std::function<void(int, const std::string&)> onError)
{
    QueryCallback callback;
    callback.onSuccess = onSuccess;
    callback.onError = onError;
    callback.promise = std::make_shared<std::promise<std::vector<oatpp::Object<RecordItemDto>>>>();
    
    query_callbacks_[sn] = callback;
}

std::shared_future<std::vector<oatpp::Object<RecordItemDto>>> 
RecordHandler::getQueryFuture(const std::string& sn) {
    auto it = query_callbacks_.find(sn);
    if (it != query_callbacks_.end() && it->second.promise) {
        return it->second.promise->get_future().share();
    }
    
    // 返回一个已经设置异常的future
    auto promise = std::make_shared<std::promise<std::vector<oatpp::Object<RecordItemDto>>>>();
    promise->set_exception(std::make_exception_ptr(std::runtime_error("SN not found")));
    return promise->get_future().share();
}

std::string RecordHandler::buildRecordInfoXml(
    const std::string& deviceId,
    const std::string& channelId,
    const std::string& startTime,
    const std::string& endTime,
    const std::string& type)
{
    std::string sn = generateSN();
    
    std::ostringstream oss;
    oss << "<?xml version=\"1.0\" encoding=\"GB2312\"?>\r\n"
        << "<Query>\r\n"
        << "  <CmdType>RecordInfo</CmdType>\r\n"
        << "  <SN>" << sn << "</SN>\r\n"
        << "  <DeviceID>" << channelId << "</DeviceID>\r\n"
        << "  <StartTime>" << startTime << "</StartTime>\r\n"
        << "  <EndTime>" << endTime << "</EndTime>\r\n"
        << "  <Secrecy>0</Secrecy>\r\n"
        << "  <Type>" << type << "</Type>\r\n"
        << "</Query>\r\n";
    
    return oss.str();
}

std::string RecordHandler::buildPlaybackSdp(
    const std::string& localIp,
    int localPort,
    const std::string& startTime,
    const std::string& endTime)
{
    // 将时间格式从 "2026-02-01T08:00:00" 转换为 "20260201080000"
    std::string start = startTime;
    std::string end = endTime;
    start.erase(std::remove(start.begin(), start.end(), '-'), start.end());
    start.erase(std::remove(start.begin(), start.end(), ':'), start.end());
    start.erase(std::remove(start.begin(), start.end(), 'T'), start.end());
    end.erase(std::remove(end.begin(), end.end(), '-'), end.end());
    end.erase(std::remove(end.begin(), end.end(), ':'), end.end());
    end.erase(std::remove(end.begin(), end.end(), 'T'), end.end());
    
    std::ostringstream oss;
    oss << "v=0\r\n"
        << "o=" << server_id_ << " 0 0 IN IP4 " << localIp << "\r\n"
        << "s=Playback\r\n"
        << "c=IN IP4 " << localIp << "\r\n"
        << "t=" << start << " " << end << "\r\n"
        << "m=video " << localPort << " TCP/RTP/AVP 96 98 97\r\n"
        << "a=recvonly\r\n"
        << "a=rtpmap:96 PS/90000\r\n"
        << "a=rtpmap:98 H264/90000\r\n"
        << "a=rtpmap:97 MPEG4/90000\r\n"
        << "a=setup:passive\r\n"
        << "a=connection:new\r\n";
    
    return oss.str();
}

std::string RecordHandler::buildPlaybackControlXml(
    const std::string& deviceId,
    const std::string& channelId,
    const std::string& command,
    const std::string& seekTime,
    float speed)
{
    std::string sn = generateSN();
    
    std::ostringstream oss;
    oss << "<?xml version=\"1.0\" encoding=\"GB2312\"?>\r\n"
        << "<Control>\r\n"
        << "  <CmdType>PlaybackControl</CmdType>\r\n"
        << "  <SN>" << sn << "</SN>\r\n"
        << "  <DeviceID>" << channelId << "</DeviceID>\r\n"
        << "  <PlaybackCmd>" << command << "</PlaybackCmd>\r\n";
    
    if (!seekTime.empty()) {
        oss << "  <SeekTime>" << seekTime << "</SeekTime>\r\n";
    }
    
    if (speed != 1.0f) {
        oss << "  <Speed>" << std::fixed << std::setprecision(1) << speed << "</Speed>\r\n";
    }
    
    oss << "</Control>\r\n";
    
    return oss.str();
}

std::string RecordHandler::sendMessage(
    const std::string& deviceId,
    const std::string& deviceIp,
    int devicePort,
    const std::string& body)
{
    osip_message_t* message = nullptr;
    
    char from[256], to[256];
    snprintf(from, sizeof(from), "sip:%s@%s:%d", 
             server_id_.c_str(), server_ip_.c_str(), server_port_);
    snprintf(to, sizeof(to), "sip:%s@%s:%d", 
             deviceId.c_str(), deviceIp.c_str(), devicePort);
    
    eXosip_lock(sip_context_);
    int ret = eXosip_message_build_request(sip_context_, &message, "MESSAGE", 
                                           to, from, nullptr);
    if (ret == 0 && message) {
        osip_message_set_body(message, body.c_str(), body.length());
        osip_message_set_content_type(message, "Application/MANSCDP+xml");
        
        eXosip_message_send_request(sip_context_, message);
    }
    eXosip_unlock(sip_context_);
    
    // 从XML中提取SN
    size_t snStart = body.find("<SN>");
    size_t snEnd = body.find("</SN>");
    if (snStart != std::string::npos && snEnd != std::string::npos) {
        return body.substr(snStart + 4, snEnd - snStart - 4);
    }
    
    return "";
}

int RecordHandler::sendInvite(
    const std::string& deviceId,
    const std::string& channelId,
    const std::string& deviceIp,
    int devicePort,
    const std::string& sdp)
{
    osip_message_t* message = nullptr;
    
    char from[256], to[256];
    snprintf(from, sizeof(from), "sip:%s@%s:%d", 
             server_id_.c_str(), server_ip_.c_str(), server_port_);
    snprintf(to, sizeof(to), "sip:%s@%s:%d", 
             channelId.c_str(), deviceIp.c_str(), devicePort);
    
    eXosip_lock(sip_context_);
    int ret = eXosip_call_build_initial_invite(sip_context_, &message, 
                                               to, from, nullptr, nullptr);
    if (ret == 0 && message) {
        osip_message_set_body(message, sdp.c_str(), sdp.length());
        osip_message_set_content_type(message, "application/sdp");
        
        int callId = eXosip_call_send_initial_invite(sip_context_, message);
        eXosip_unlock(sip_context_);
        return callId;
    }
    eXosip_unlock(sip_context_);
    
    return -1;
}

bool RecordHandler::sendInfo(int callId, const std::string& body) {
    osip_message_t* message = nullptr;
    
    eXosip_lock(sip_context_);
    int ret = eXosip_call_build_info(sip_context_, callId, &message);
    if (ret == 0 && message) {
        osip_message_set_body(message, body.c_str(), body.length());
        osip_message_set_content_type(message, "Application/MANSCDP+xml");
        
        ret = eXosip_call_send_request(sip_context_, callId, message);
        eXosip_unlock(sip_context_);
        return ret == 0;
    }
    eXosip_unlock(sip_context_);
    
    return false;
}

bool RecordHandler::sendBye(int callId) {
    eXosip_lock(sip_context_);
    int ret = eXosip_call_terminate(sip_context_, callId, 0);
    eXosip_unlock(sip_context_);
    
    return ret == 0;
}

std::vector<oatpp::Object<RecordItemDto>> 
RecordHandler::parseRecordInfoXml(const std::string& xml) {
    std::vector<oatpp::Object<RecordItemDto>> records;
    
    auto root = XmlHelper::parseXml(xml);
    if (!root) {
        return records;
    }
    
    // 解析RecordList
    auto recordList = XmlHelper::getChildNode(root, "Response/RecordList");
    if (!recordList) {
        return records;
    }
    
    // 遍历Item节点
    for (auto item = recordList->first_node("Item"); item; item = item->next_sibling("Item")) {
        auto record = RecordItemDto::createShared();
        
        record->device_id = XmlHelper::getNodeValue(item, "DeviceID");
        record->channel_id = record->device_id;  // 通常DeviceID就是ChannelID
        record->name = XmlHelper::getNodeValue(item, "Name");
        record->file_path = XmlHelper::getNodeValue(item, "FilePath");
        record->address = XmlHelper::getNodeValue(item, "Address");
        record->start_time = XmlHelper::getNodeValue(item, "StartTime");
        record->end_time = XmlHelper::getNodeValue(item, "EndTime");
        
        std::string secrecy = XmlHelper::getNodeValue(item, "Secrecy");
        record->secrecy = secrecy.empty() ? 0 : std::stoi(secrecy);
        
        record->type = XmlHelper::getNodeValue(item, "Type");
        record->recorder_id = XmlHelper::getNodeValue(item, "RecorderID");
        
        std::string fileSize = XmlHelper::getNodeValue(item, "FileSize");
        record->file_size = fileSize.empty() ? 0 : std::stoll(fileSize);
        
        records.push_back(record);
    }
    
    return records;
}

std::string RecordHandler::generateSN() {
    return std::to_string(sn_counter_++);
}

} // namespace gbsip_server
