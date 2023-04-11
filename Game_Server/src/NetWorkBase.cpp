#include "NetworkBase.h"

void RPC_Message::Fill_RemoteAddrInfo(string& IP, int Port)
{
	ZeroMemory(&remote_addr, sizeof(remote_addr));
	remote_addr.sin_family = AF_INET;
	remote_addr.sin_port = htons(Port);
	inet_pton(AF_INET, IP.c_str(), &(remote_addr.sin_addr.s_addr));

}

bool RPC_Message::GenerateRequestBuf(char*& buf_out, int* sum_bufferlen_out)
{
	int sum_sendRequestlen = TokenSize + sizeof(Request_header) + Request_header.length;
	try
	{
		buf_out = new char[sum_sendRequestlen + 1];
		memcpy(buf_out, Request_token.c_str(), TokenSize);
		memcpy(buf_out + TokenSize, &Request_header, sizeof(Request_header));
		memcpy(buf_out + TokenSize + sizeof(Request_header), Request_buffer, Request_header.length);

		if (sum_bufferlen_out)
			*sum_bufferlen_out = sum_sendRequestlen;

		return true;
	}
	catch (exception& e)
	{
		perror("RPC_Message::GenerateRequestBuf occurred unknow error :");
		LOGINFO("RPC_Message::GenerateRequestBuf occurred unknow error : {}", e.what());
		return false;
	}
}
bool RPC_Message::GenerateResponseBuf(char*& buf_out, int* sum_bufferlen_out)
{
	int sum_sendlen = sizeof(Response_header) + Response_header.length;
	try
	{

		buf_out = new char[sum_sendlen + 1];
		memcpy(buf_out, &Response_header, sizeof(Response_header));
		memcpy(buf_out + sizeof(Response_header), Response_buffer, Response_header.length);
		if (sum_bufferlen_out)
			*sum_bufferlen_out = sum_sendlen;

		return true;
	}
	catch (exception& e)
	{
		perror("RPC_Message::GenerateResponseBuf occurred unknow error :");
		LOGINFO("RPC_Message::GenerateResponseBuf occurred unknow error : {}", e.what());
		return false;
	}
}

bool RPC_Message::ParseRequestBuf(const char* buf, const int len)
{
	if (len < (int)(TokenSize + sizeof(Request_header)))
		return false;
	// 获取token
	char token_ch[TokenSize];
	memcpy(token_ch, buf, TokenSize);
	Request_token.assign(token_ch, TokenSize);
	// 获取头
	memcpy(&Request_header, buf + TokenSize, sizeof(Request_header));

	if (Request_header.length < 0)
		return false;
	// 获取内容（可能为空）
	Request_buffer = new char[Request_header.length + 1];
	memcpy(Request_buffer, buf + TokenSize + sizeof(Request_header), Request_header.length);

	return true;
}
bool RPC_Message::ParseResponseBuf(const char* buf, const int len)
{
	if (len < (int)(sizeof(Response_header)))
		return false;

	// 获取头
	memcpy(&Response_header, buf, sizeof(Response_header));

	if (Response_header.length < 0)
		return false;
	// 获取内容（可能为空）
	Response_buffer = new char[Response_header.length + 1];
	memcpy(Response_buffer, buf + sizeof(Response_header), Response_header.length);

	return true;
}