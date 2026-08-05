#pragma once

#include <string>
#include <sodium.h>
#include <stdexcept>

using namespace std;



class SecurityManager
{
public:
	static SecurityManager& GetInstance()
	{
		static SecurityManager _instance;
		return _instance;
	}

	void Init();
	string HashPassword(string_view password);
	bool VerifyPassword(string_view password, string_view storedHash);
	bool NeedsRehash(string_view storedHash);


	



};
