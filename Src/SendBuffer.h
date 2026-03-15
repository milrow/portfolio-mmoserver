#pragma once

#include <vector>
#include <mutex>

using namespace std;

class SendBuffer
{
public:
	SendBuffer(uint16_t size) { _buffer.resize(size); }
	char* Buffer() { return _buffer.data(); }
	uint16_t Size() { return _size; }
	void Close(uint16_t size) {
		_size = size;
	}

private:
	vector<char> _buffer;
	uint16_t _size = 0;
	
};

using SendBufferRef = shared_ptr<SendBuffer>;

