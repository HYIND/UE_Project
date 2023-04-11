#pragma once

#include "stdafx.h"
#include "MsgNum.h"

using namespace std;

#define TokenSize 32

struct Header
{
	int type = 0;
	int length = 0;
	Header() {};
	Header(int type) : type(type) {};
};

struct Socket_Message
{
	int socket_fd;
	Header header;
	char* content = nullptr;

	Socket_Message() :socket_fd(0), content(nullptr) {};
	Socket_Message(int socket) : socket_fd(socket), content(nullptr) {}
	Socket_Message(int socket, Header header) : socket_fd(socket), header(header), content(nullptr) {}
	~Socket_Message()
	{
		if (content)
			delete content;
	}

	void SetHeader() {};
	void SetContent() {};

	void SetHeader(char* ch) {
		memcpy(&header, ch, sizeof(Header));
	}
	void SetContent(char* ch) {
		//获取内容（可能为空）
		if (header.length > 0)
		{
			content = new char[header.length];
			memcpy(content, ch, header.length);
		}
	}
};

struct RPC_Message
{
	// Request
	string Request_token;
	Header Request_header;
	char* Request_buffer = nullptr;

	// Response
	Header Response_header;
	char* Response_buffer = nullptr;

	// server
	sockaddr_in remote_addr;

	RPC_Message() { }
	RPC_Message(string token_in, sockaddr_in remote_addr_in) : Request_token(token_in), remote_addr(remote_addr_in) {}

	~RPC_Message()
	{
		if (Request_buffer)
			delete Request_buffer;
		if (Response_buffer)
			delete Response_buffer;
	};

	template <typename T>
	void Fill_RequestInfo(T& proto_message)
	{
		Request_header.type = Get_Header_Type(proto_message);
		if (Request_header.type == 0)
			return;
		Request_header.length = proto_message.ByteSizeLong();

		Request_buffer = new char[Request_header.length + 1];
		proto_message.SerializeToArray(Request_buffer, Request_header.length);
	}
	template <typename T>
	void Fill_ResponseInfo(T& proto_message)
	{
		Response_header.type = Get_Header_Type(proto_message);
		if (Response_header.type == 0)
			return;
		Response_header.length = proto_message.ByteSizeLong();

		Response_buffer = new char[Response_header.length + 1];
		proto_message.SerializeToArray(Response_buffer, Response_header.length);
	}
	void Fill_RemoteAddrInfo(string& IP, int Port);
	bool GenerateRequestBuf(char*& buf_out, int* sum_bufferlen_out);
	bool GenerateResponseBuf(char*& buf_out, int* sum_bufferlen_out);
	bool ParseRequestBuf(const char* buf, const int len);
	bool ParseResponseBuf(const char* buf, const int len);

};

//hIOCP 重叠结构体
typedef struct _PER_IO_DATA
{
	OVERLAPPED ol;          //重叠结构
	char buf[1024];			//数据缓冲区
	int OperationType;     //I/O操作类型
#define OP_READ 1
#define OP_WRITE 2
//#define OP_ACCEPT 3
}PER_IO_DATA, * PPER_IO_DATA;
