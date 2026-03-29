#include "DbConnection.h"
#include <iostream>

bool DbConnection::Connect(SQLHENV hEnv, const wchar_t* connectionString)
{
	if (SQL_SUCCESS != ::SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc)) {
		return false;
	}

	// connectionString 예시: L"Driver={ODBC Driver 17 for SQL Server};Server=localhost;Database=MyGameDB;UID=sa;PWD=your_password;"

	SQLWCHAR outConnStr[1024];
	SQLSMALLINT outConnStrLen;

	SQLRETURN ret = ::SQLDriverConnectW(
		hDbc,
		NULL,
		(SQLWCHAR*)connectionString,
		SQL_NTS,
		outConnStr,
		1024,
		&outConnStrLen,
		SQL_DRIVER_NOPROMPT
	);

	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		return false;
	}

	if (SQL_SUCCESS != ::SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) {
		return false;
	}
	
	return true;
}

void DbConnection::Disconnect()
{
	if (hStmt != SQL_NULL_HSTMT) {
		::SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		hStmt = SQL_NULL_HSTMT;
	}

	if (hDbc != SQL_NULL_HDBC) {
		::SQLDisconnect(hDbc);

		::SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
		hDbc = SQL_NULL_HDBC;
	}

	
}

bool DbConnection::Excute(const wchar_t* query)
{
	::SQLFreeStmt(hStmt, SQL_CLOSE);
	::SQLFreeStmt(hStmt, SQL_UNBIND);
	//::SQLFreeStmt(hStmt, SQL_RESET_PARAMS);

	SQLRETURN ret = ::SQLExecDirectW(hStmt, (SQLWCHAR*)query, SQL_NTS);

	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		return true;
	}

	return false;
}

bool DbConnection::Fetch()
{
	SQLRETURN ret = ::SQLFetch(hStmt);

	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		return true;
	}

	if (ret == SQL_NO_DATA) {
		return false;
	}

	return false;
}

