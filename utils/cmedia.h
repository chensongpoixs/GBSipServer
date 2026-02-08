#ifndef CMEDIA_H
#define CMEDIA_H
#include <cinttypes>

namespace   cmedia
{
	 bool    GetRequest(const char * rtsp_url, const char * media_url);
}


#if 0
int main(int argc, char * argv[])
{
	std::string rtsp_url = "rtsp://127.0.0.1/live/test11";
	std::string media_url = "http://127.0.0.1:80";
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
	printf("WSAStartup failed\n");
	//	exit(EXIT_FAILURE);
	}
	cmedia::GetRequest(rtsp_url.c_str(), media_url.c_str());
	return 0;
}
#endif // 

#endif // 