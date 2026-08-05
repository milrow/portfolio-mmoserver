#include "PacketHandlerUtility.h"

wstring Utf8ToWstring(const string& str)
{
	if (str.empty()) return wstring();

	// 필요한 크기 계산
	int len = ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	wstring res(len, 0);

	// 실제 변환
	::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &res[0], len);
	return res;
}
