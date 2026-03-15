#pragma once

#include <queue>
#include <mutex>
#include <map>
#include "Session.h"

using namespace std;

class SessionManager
{
public:
	static SessionManager& GetInstance() {
		static SessionManager _instance;
		return _instance;
	}
	
	void Init(int maxSessionCnt) {
		lock_guard<mutex> lock(_mutex);
		for (int i = 0; i < maxSessionCnt; i++) {
			shared_ptr<Session> session = make_shared<Session>();
			_sessions.push(session);
		}
	}

	shared_ptr<Session> Pop() {
		lock_guard<mutex> lock(_mutex);
		if (_sessions.empty()) return nullptr;
		shared_ptr<Session> session = _sessions.front();
		_sessions.pop();
		session->_sessionId = _sessionIdGenerator.fetch_add(1);
		if (session->_sessionId == 0) session->_sessionId = _sessionIdGenerator.fetch_add(1);
		
		_activeSessions.insert({session->_sessionId, session});

		//printf("Current Queue Size: %llu\n", _sessions.size());

		if (_sessions.empty()) {
			//printf("Critical Error: Session Pool is Empty!\n");
			return nullptr;
		}
		
		return session;
	}

	void Push(shared_ptr<Session> session) {
		lock_guard<mutex> lock(_mutex);
		_activeSessions.erase(session->_sessionId);
		session->Reset();
		_sessions.push(session);
	}

	shared_ptr<Session> Find(uint32_t sessionId) {
		lock_guard<mutex> lock(_mutex);
		auto it = _activeSessions.find(sessionId);
		if (it != _activeSessions.end()) {
			//printf("session");
			return it->second;
		}
		//printf("session null");
		return nullptr;
	}

	void Broadcast(SendBufferRef sendBuffer) {
		lock_guard<mutex> lock(_mutex);
		for (auto& it : _activeSessions) {
			it.second->Send(sendBuffer);
		}
	}

	void Send(SendBufferRef sendBuffer, uint32_t sessionId) {
		lock_guard<mutex> lock(_mutex);
		auto it = _activeSessions.find(sessionId);
		if (it != _activeSessions.end()) {
			it->second->Send(sendBuffer);
		}
	}

	size_t Lenth() {
		return (_sessions.size());
	}

	/*void Close(Session* session) {
		closesocket(session->_socket);
		this->Push(session);
	}*/

private:
	mutex _mutex;
	queue<shared_ptr<Session>> _sessions;
	map<uint32_t, shared_ptr<Session>> _activeSessions;
	atomic<uint32_t> _sessionIdGenerator = 1;
};

