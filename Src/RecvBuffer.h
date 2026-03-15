#pragma once
#include <vector>

using namespace std;

class RecvBuffer
{
public:
	RecvBuffer(int buffSize = 1024) : _capacity(buffSize) {
		_buffer.resize(buffSize);
	}
	
	int DataSize() { return _writePos - _readPos; }
	int FreeSize() { return _capacity - _writePos; }
	bool OnWrite(int len) {
		if (len > FreeSize()) {
			return false;
		}
		_writePos += len;
		return true;
	}
	bool OnRead(int len) {
		if (len > DataSize()) {
			return false;
		}
		_readPos += len;
		return true;
	}
	void Clean() {
		int dataSize = DataSize();
		if (dataSize == 0) {
			_readPos = _writePos = 0;
		}
		else {
			copy(_buffer.begin() + _readPos, _buffer.begin() + _writePos, _buffer.begin());
			_readPos = 0;
			_writePos = dataSize;
		}
	}
	
	char* ReadPos() { return &_buffer[_readPos]; }
	char* WritePos() { return &_buffer[_writePos]; }

private: 
	int _capacity;
	int _readPos = 0;
	int _writePos = 0;
	 vector<char> _buffer;

};

