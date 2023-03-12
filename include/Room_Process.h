// #pragma once

// #include "NetworkStuct.h"
// #include "Hall_Server.h"

// using namespace std;

// extern int Get_Header_Type_bystring(string &str);

// struct socket_recvinfo
// {
//     int socket;
//     Header header;
//     char *content = nullptr;
//     socket_recvinfo(int socket) : socket(socket), content(nullptr) {}
//     ~socket_recvinfo()
//     {
//         if (content)
//             delete content;
//     }
// };

// struct socket_sendinfo
// {
//     int socket;
//     int len;
//     char *send_ch;

//     template <typename T>
//     socket_sendinfo(int socket, T &message) : socket(socket)
//     {
//         Header header;
//         header.type = Get_Header_Type(message);
//         if (header.type == 0)
//             return;
//         header.length = message.ByteSizeLong();

//         len = sizeof(Header) + header.length;

//         send_ch = new char[len];
//         memset(send_ch, '\0', len);

//         memcpy(send_ch, &header, sizeof(header));
//         message.SerializeToArray(send_ch + sizeof(Header), header.length);
//     }

//     socket_sendinfo(int socket, string &str) : socket(socket)
//     {
//         Header header;
//         header.type = Get_Header_Type_bystring(str);
//         if (header.type == 0)
//             return;
//         header.length = 0;
//         len = sizeof(Header);

//         send_ch = new char[len];
//         memset(send_ch, '\0', len);
//         memcpy(send_ch, &header, len);
//     };

//     socket_sendinfo(int socket, int type) : socket(socket)
//     {
//         Header header(type);
//         header.length = 0;
//         len = sizeof(Header);

//         send_ch = new char[len];
//         memset(send_ch, '\0', len);
//         memcpy(send_ch, &header, len);
//     }

//     ~socket_sendinfo()
//     {
//         if (send_ch)
//             delete[] send_ch;
//     }
// };

// struct Room_Userinfo
// {
//     int tank_id;
//     User_ *sockinfo = NULL;
//     bool Ready = false;
// };

// // 房间/游戏处理类
// class Room_Process
// {
// public:
//     /* 房间基本信息 */
//     int room_id = 0;      // 房间号
//     int user_count = 0;   // 当前玩家人数
//     int user_limited = 4; // 玩家人数上限
//     int socket_host = 0;  // 房主socket

//     unordered_map<int, Room_userinfo *> info; // 房内玩家信息 socket->userinfo

//     /* 地图相关 */
//     int map_id = 0; // 当前地图号

//     /* 相关的fd */
//     int recv_pipe[2];        // 接收线程管道
//     char buffer[100];        // 读缓冲
//     int recv_epoll;          // 接收epoll
//     epoll_event events[100]; // epoll events
//     int timefd;              // 用于投放道具的定时器

//     /* 标志位 */
//     bool gaming = false; // 记录是否在游戏中
//     bool stop = false;   // 控制epoll循环

//     /* 消息队列 */
//     queue<socket_recvinfo *> recv_queue; // 消息接收队列
//     queue<socket_sendinfo *> send_queue; // 消息发送队列

//     /* 锁和相关条件变量 */
//     mutex process_mtx;             // 接收线程唤醒处理线程的锁
//     condition_variable process_cv; // 接收线程唤醒处理线程的条件变量

//     mutex send_mtx;             // 处理线程唤醒发送线程的锁
//     condition_variable send_cv; // 处理线程唤醒发送线程的条件变量

//     mutex info_mtx;      // 访问用户信息的锁
//     mutex recvqueue_mtx; // 访问接收队列的锁
//     mutex sendqueue_mtx; // 访问发送队列的锁

// public:
//     Room_Process(int socket); // 构造函数，初始化；
//     ~Room_Process();          // 析构函数

//     /*以下为房间消息的相关处理函数*/
//     void Add_player(int socket);                                  // 为房间添加一个新用户
//     void Ready(int socket);                                       // 准备
//     void Cancel_Ready(int socket);                                // 取消准备
//     string Start_Game(int socket);                                // 开始游戏
//     string Quit_Room(int socket);                                 // 退出房间
//     void Return_Roominfo(int socket);                             // 返回房间信息
//     void Room_Message(int socket, Header &header, char *content); // 发送消息
//     void Set_Map(int id);                                         // 设置地图信息
//     void Change_Map(int socket, string &new_id_str);              // 房主切换地图

//     string return_class_room(int socket, Header &header, char *content); // 处理房间消息的入口函数，负责解析消息并调用相应的函数

//     /*以下为游戏消息的相关处理函数*/
//     void Init_Timer();   // 初始化定时器
//     void Delete_Timer(); // 销毁定时器

//     /* 各个线程的线程函数 */
//     void run(); // 初始运行函数，运行三个线程：接收线程、房间处理线程、发送线程

//     void recv_process(); // 接收线程函数，负责接收数据并加入消息队列
//     void room_process(); // 房间消息处理线程函数，负责处理消息队列中的数据
//     void send_process(); // 发送线程函数，负责将发送队列中的消息发回

// };
