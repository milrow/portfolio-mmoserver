#pragma once

#include <WinSock2.h>
#include <queue>
#include "RecvBuffer.h"
#include "SendBuffer.h"

#define BUF_SIZE 1024

enum IO_TYPE
{
    RECV,
    SEND
};

typedef struct PER_IO_DATA {
    WSAOVERLAPPED overlapped;
    WSABUF wsaBuf;
    char buffer[BUF_SIZE];
    IO_TYPE ioType;
};
using LPPER_IO_DATA = PER_IO_DATA*;

class SessionManager;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session() {
        memset(&_recvIoInfo, 0, sizeof(_recvIoInfo));
        _recvIoInfo.ioType = IO_TYPE::RECV;
        memset(&_sendIoInfo, 0, sizeof(_sendIoInfo));
        _sendIoInfo.ioType = IO_TYPE::SEND;
        _isSending = false;
    };
	~Session() { closesocket(_socket); }
    
    PER_IO_DATA _recvIoInfo;
    PER_IO_DATA _sendIoInfo;

    RecvBuffer _recvBuff;

    uint32_t _sessionId = 0;
    bool _isLoggedIn = false;
    SOCKET _socket = INVALID_SOCKET;

    atomic<uint32_t> _refCnt = 0;

    void AddRef() { ++_refCnt; }
    void ReleaseRef();

    void Disconnect() {
        if (_socket != INVALID_SOCKET) {
            closesocket(_socket);
            _socket = INVALID_SOCKET;
        }
    }

    void Reset() {
        _socket = INVALID_SOCKET;
        _sessionId = 0;
        _isLoggedIn = false;
        _recvBuff.Clean();
        memset(&_recvIoInfo.overlapped, 0, sizeof(OVERLAPPED));
    }

    queue<SendBufferRef> _sendQueue;
    vector<WSABUF> _wsaBufs;
    mutex _sendLock;

    void Send(SendBufferRef sendBuf) {
        lock_guard<mutex> lock(_sendLock);
        _sendQueue.push(sendBuf);

        if (_isSending == false) {
            FlushSend();
        }
    }

    void FlushSend();

    void ClearSending() {
        this->_isSending = false;
        this->_sendingList.clear();
    }


private:
    bool _isSending;
    vector<SendBufferRef> _sendingList;
};

