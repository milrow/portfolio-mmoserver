#pragma once
#include <Windows.h>
#include <sql.h>
#include <sqlext.h>

class DbConnection {
public:
	DbConnection() {}
	~DbConnection() {
		Disconnect();
	}

	bool Connect(SQLHENV hEnv, const wchar_t* connectionString);
	void Disconnect();

	bool Excute(const wchar_t* query);
	bool Fetch();

	void BeginTransaction() { ::SQLSetConnectAttr(hDbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, 0); }
	void EndTransaction(bool success) {
		::SQLEndTran(SQL_HANDLE_DBC, hDbc, success ? SQL_COMMIT : SQL_ROLLBACK);
		::SQLSetConnectAttr(hDbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, 0);
	}

	SQLHSTMT GetStmtHandle() { return hStmt; };

private:
	SQLHDBC hDbc = SQL_NULL_HDBC;
	SQLHSTMT hStmt = SQL_NULL_HSTMT;
};