#include "Session.h"
#include "SessionManager.h"

void Session::ReleaseRef() {
	if (--_refCnt == 0) {
		SessionManager::GetInstance().Push(shared_from_this());
	}
}

void Session::FlushSend()
{
	//lock_guard<mutex> lock(_sendLock);
	if (_sendQueue.empty()) return;

	_isSending = true;
	
	_wsaBufs.clear();

	DWORD sendByes;
	DWORD flags = 0;

	while (!_sendQueue.empty()) {
		SendBufferRef buf = _sendQueue.front();
		_sendQueue.pop();

		_sendingList.push_back(buf);

		//char* ptr = buf->Buffer();
		//uint16_t* pSize = reinterpret_cast<uint16_t*>(ptr);
		//uint16_t* pId = reinterpret_cast<uint16_t*>(ptr + 2);

		//// [중요] 여기서 찍히는 값이 유니티 로그와 일치해야 합니다.
		//printf("[Final Check] SendAddr: %p, PacketSize: %d, ProtocolID: %d\n", ptr, *pSize, *pId);

		WSABUF wsaBuf;
		wsaBuf.buf = buf->Buffer();
		wsaBuf.len = buf->Size();
		_wsaBufs.push_back(wsaBuf);
	}
	
	this->AddRef();
	int result = WSASend(_socket, _wsaBufs.data(), _wsaBufs.size(), &sendByes, flags, (LPWSAOVERLAPPED)&_sendIoInfo.overlapped, 0);

	if (result == SOCKET_ERROR) {
		int errorCode = WSAGetLastError();
		if (errorCode != ERROR_IO_PENDING) {
			_isSending = false;
			_sendingList.clear();
			this->ReleaseRef();
		}
	}
}
