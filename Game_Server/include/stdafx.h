#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <comdef.h>
// WinSocket
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#include <signal.h>
#include <thread>

#include <vector>
#include <string>
#include <queue>
#include <mutex> 

#include <map>
#include <unordered_map>
#include <timeapi.h>
#pragma comment(lib,"Winmm.lib")

#include <type_traits>

#include "Config.h"

#define LOGGERMODE_ON
#include "Log.h"

#include "NetworkBase.h"
#include "MsgNum.h"
