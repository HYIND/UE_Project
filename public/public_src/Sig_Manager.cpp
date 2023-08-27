#include "event.h"
#include <assert.h>
#include <signal.h>
#include "Sig_Manager.h"
#include "Log.h"

vector<int> &SigManager::GetPipes()
{
    return Pipes;
}
void SigManager::AddPipe(int pipe)
{
    Pipes.push_back(pipe);
}
void SigManager::DelPipe(int pipe)
{
    auto it = find(Pipes.begin(), Pipes.end(), pipe);
    if (it != Pipes.end())
        Pipes.erase(it);
}
