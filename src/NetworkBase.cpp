#include "NetworkBase.h"

struct token_part
{
    char part1[8];
    char part2[8];
    char part3[8];
    char part4[8];
};

union Token
{
    char ch[TokenSize];
    token_part split;
    Token() { memset(ch, '0', TokenSize); }
    Token &operator=(const Token &other);
    bool operator==(Token &other);
    string Tostring();
};

Token &Token::operator=(const Token &other)
{
    memcpy(this->ch, other.ch, sizeof(Token));
    return *this;
}
bool Token::operator==(Token &other)
{
    for (int i = 0; i < sizeof(ch); i++)
    {
        if (ch[i] != other.ch[i])
            return false;
    }
    return true;
}
string Token::Tostring()
{
    string str;
    str.assign(ch, sizeof(ch));
    return str;
}

void Get_Token(Token &token)
{
    auto random_seed = std::chrono::system_clock::now().time_since_epoch().count();

    std::mt19937 seed_engine(random_seed);
    std::uniform_int_distribution<unsigned> random_gen;
    unsigned random_value = random_gen(seed_engine);
    sizeof(random_value);

    std::stringstream s1, s2;
    s1 << std::hex << random_seed;
    s2 << std::hex << random_value;

    string strtime = s1.str();
    string strrandom = s2.str();

    memcpy(token.split.part1, strtime.c_str(), sizeof(random_seed));
    memcpy(token.split.part2, s2.str().c_str(), sizeof(random_value) * 2);
    memcpy(token.split.part3, strtime.c_str() + sizeof(random_seed), sizeof(random_seed));
}
string Get_Token()
{
    Token token;
    Get_Token(token);
    return token.Tostring();
}

void RPC_Message::Fill_RemoteAddrInfo(string &IP, int Port)
{
    bzero(&remote_addr, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(Port);
    remote_addr.sin_addr.s_addr = inet_addr(IP.c_str());
}

bool RPC_Message::GenerateRequestBuf(char *&buf_out, int *sum_bufferlen_out)
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
    catch (exception &e)
    {
        perror("RPC_Message::GenerateRequestBuf occurred unknow error :");
        LOGINFO("RPC_Message::GenerateRequestBuf occurred unknow error : {}", e.what());
        return false;
    }
}
bool RPC_Message::GenerateResponseBuf(char *&buf_out, int *sum_bufferlen_out)
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
    catch (exception &e)
    {
        perror("RPC_Message::GenerateResponseBuf occurred unknow error :");
        LOGINFO("RPC_Message::GenerateResponseBuf occurred unknow error : {}", e.what());
        return false;
    }
}

bool RPC_Message::ParseRequestBuf(const char *buf, const int len)
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
bool RPC_Message::ParseResponseBuf(const char *buf, const int len)
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
