#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <process.h>
#include <WinSock2.h>
#include <Windows.h>
#include <vector>
#include <thread>
#include <mswsock.h> 
#include "JobManager.h"
#include "Protocol.pb.h"
#include "SessionManager.h"
#include "PacketHandler.h"

#pragma comment(lib, "ws2_32.lib")

#define MAX_SESSSION_COUNT 2000

using namespace std;

typedef struct PER_HANDLE_DATA {
    SOCKET hClntSock;
    SOCKADDR_IN hClntAddr;
    uint32_t sessionId;
};
using LPPER_HANDLE_DATA = PER_HANDLE_DATA*;

vector<thread> workerThreads;

void WorkThreadMain(LPVOID pComPort);
void LogicTreadMain();
void ErrorHandling(const char *message);

int main()
{
    SOCKET lincSock; 
    SOCKADDR_IN lincAddr;

    HANDLE hComport;
    OVERLAPPED hOverlapped;
    LPPER_HANDLE_DATA handleInfo{};
    DWORD recvBytes = 0;

    SYSTEM_INFO sysInfo;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        ErrorHandling("WSAStartup failed");
    }

    hComport = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    
    thread logicThread(LogicTreadMain);

    GetSystemInfo(&sysInfo);
    for (int i = 0; i < sysInfo.dwNumberOfProcessors; i++) {
        workerThreads.emplace_back(WorkThreadMain, hComport);
    }
    
    SessionManager::GetInstance().Init(MAX_SESSSION_COUNT);

    //printf("Session Count: %d\n", SessionManager::GetInstance().Lenth());

    lincSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (lincSock == INVALID_SOCKET) {
        WSACleanup();
        ErrorHandling("INVALID_SOCKET : lincSocket error");
        return 1;
    }
    
    memset(&lincAddr, 0, sizeof(lincAddr));
    lincAddr.sin_family = AF_INET;
    lincAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    lincAddr.sin_port = htons(atoi("9000"));

    if (::bind(lincSock, (SOCKADDR*)&lincAddr, sizeof(lincAddr)) == SOCKET_ERROR) {
        cerr << "bind error" << endl;
    }

    if (::listen(lincSock, 5) == SOCKET_ERROR) {
        cerr << "listen error" << endl;
        return 1;
    }

    InitPacketHandler();

    while (1) {
        SOCKET clntSocket;
        SOCKADDR_IN clntAddr;
        int addrLen = sizeof(clntAddr);

        clntSocket = accept(lincSock, (SOCKADDR*)&clntAddr, &addrLen);
        
        if (clntSocket == INVALID_SOCKET) continue;

        handleInfo = (LPPER_HANDLE_DATA) new PER_HANDLE_DATA{};
        handleInfo->hClntSock = clntSocket;
        memcpy(&(handleInfo->hClntAddr), &clntAddr, addrLen);
        
        std::shared_ptr<Session> session = SessionManager::GetInstance().Pop();

        if (session == nullptr) {
            closesocket(clntSocket);
            continue;
        }
        session->_socket = clntSocket;
        session->AddRef();

        CreateIoCompletionPort((HANDLE)clntSocket, hComport, (ULONG_PTR)session.get(), 0);

        DWORD flags = 0;
        WSABUF wsaBuf;
        wsaBuf.buf = session->_recvBuff.WritePos();
        wsaBuf.len = session->_recvBuff.FreeSize();
        session->AddRef();
        if(SOCKET_ERROR == WSARecv(handleInfo->hClntSock, &wsaBuf, 1, &recvBytes, &flags, &(session->_recvIoInfo.overlapped), NULL)){
            if (WSAGetLastError() != ERROR_IO_PENDING) {
                session->ReleaseRef();
            }
        }
    }

    WSACleanup();
}

void WorkThreadMain(LPVOID pComPort) {
    HANDLE hComPort = (HANDLE)pComPort;
    DWORD bytesTrans;
    LPPER_IO_DATA ioData = nullptr;
    Session* session = nullptr;

    while (true) {
        BOOL res = GetQueuedCompletionStatus(hComPort, &bytesTrans, (PULONG_PTR)&session, (LPOVERLAPPED*)&ioData, INFINITE);

        if (res == false || bytesTrans == 0) {
            if (session) {
                uint32_t id = session->_sessionId;
                BroadcastLeaveGame(id);
                session->Disconnect();
                session->ReleaseRef();
            }
            continue;
        }

        if (ioData->ioType == IO_TYPE::RECV) {
            if (session->_recvBuff.OnWrite(bytesTrans) == false) {
                ErrorHandling("Buffer overflow");
                uint32_t id = session->_sessionId;
                BroadcastLeaveGame(id);
                session->Disconnect();
                session->ReleaseRef();
                continue;
            }

            while (true)
            {
                int dataSize = session->_recvBuff.DataSize();
                if (dataSize < sizeof(PacketHeader)) {
                    break;
                }

                PacketHeader* header = reinterpret_cast<PacketHeader*>(session->_recvBuff.ReadPos());
                if (dataSize < header->size) {
                    break;
                }

                int payloadSize = header->size - sizeof(PacketHeader);
                vector<char> payload(payloadSize);

                memcpy(payload.data(), session->_recvBuff.ReadPos() + sizeof(PacketHeader), payloadSize);
                auto newJob = make_shared<Job>();
                newJob->sessionId = session->_sessionId;
                newJob->protocolId = header->protocolId;
                newJob->rawData = move(payload);
                newJob->len = payloadSize;
                
                JobManager::GetInstance().PushJob(newJob);

                session->_recvBuff.OnRead(header->size);
            }
       
            session->_recvBuff.Clean();

            DWORD flags = 0;
            WSABUF wsaBuf;
            wsaBuf.buf = session->_recvBuff.WritePos();
            wsaBuf.len = session->_recvBuff.FreeSize();
            session->AddRef();
            if (SOCKET_ERROR == WSARecv(session->_socket, &(wsaBuf), 1, NULL, &flags, &(session->_recvIoInfo.overlapped), NULL)) {
                if (WSAGetLastError() != ERROR_IO_PENDING) {
                    session->ReleaseRef();
                }
            }

            session->ReleaseRef();
        }

        if (ioData->ioType == IO_TYPE::SEND) {
            lock_guard<mutex> lock(session->_sendLock);
            { session->ClearSending(); }

            session->FlushSend();

            session->ReleaseRef();
        }
    }
}

void LogicTreadMain()
{
    while (true)
    {
        shared_ptr<Job> job = JobManager::GetInstance().PopJob();

        if (job && GPacketHandler[job->protocolId]) {

            shared_ptr<Session> session = SessionManager::GetInstance().Find(job->sessionId);
            
            if (session) {

                GPacketHandler[job->protocolId](session, (BYTE*)job->rawData.data(), job->len);

            }
        }
        
    }
    
}

void ErrorHandling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}