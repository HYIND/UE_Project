#include "event.h"
#include <assert.h>
#include <signal.h>
#include "Sig_Manager.h"
#include <string.h>

vector<int> &SigManager::GetPipes()
{
    return Pipes;
}
void SigManager::AddPipe(int pipe)
{
    Pipes.push_back(pipe);
}
void SigManager::ErasePipe(int pipe)
{
    auto it = find(Pipes.begin(), Pipes.end(), pipe);
    if (it != Pipes.end())
        Pipes.erase(it);
}

void sig_handler(int sig)
{
    int save_errno = errno;
    int msg = sig;

    for (auto pipe : SigManager::Instance()->GetPipes())
    {
        send(pipe, (char *)&msg, 1, 0);
    }
    // send(Listen_Server::Instance()->Get_pipe(), (char *)&msg, 1, 0);
    // send(Login_Server::Instance()->Get_pipe(), (char *)&msg, 1, 0);
    // send(Center_Server::Instance()->Get_pipe(), (char *)&msg, 1, 0);
    // send(RPC_Server::Instance()->Get_pipe(), (char *)&msg, 1, 0);
    errno = save_errno;
}

void addsig(int sig)
{
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = sig_handler;
    sa.sa_flags |= SA_RESTART;
    // sigfillset(&sa.sa_mask);
    assert(sigaction(sig, &sa, NULL) != -1);
}