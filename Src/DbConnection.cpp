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

bool DbConnection::Prepare(const wchar_t* query)
{
	::SQLFreeStmt(hStmt, SQL_CLOSE);
	::SQLFreeStmt(hStmt, SQL_UNBIND);

	SQLRETURN ret = ::SQLPrepareW(hStmt, (SQLWCHAR*)query, SQL_NTS);
	return (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO);
}

bool DbConnection::ExcutePrepared()
{
	SQLRETURN ret = ::SQLExecute(hStmt);
	return (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO);
}

bool DbConnection::Excute(const wchar_t* query)
{
	::SQLFreeStmt(hStmt, SQL_CLOSE);
	::SQLFreeStmt(hStmt, SQL_UNBIND);

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

int DbConnection::GetIntData(int columnIdx)
{
	int value = 0;
	SQLLEN len = 0;

	::SQLGetData(hStmt, columnIdx, SQL_C_SLONG, &value, 0, &len);

	return value;
}

int64_t DbConnection::GetInt64Data(int columnIdx)
{
	int64_t value = 0;
	SQLLEN len = 0;

	::SQLGetData(hStmt, columnIdx, SQL_C_SBIGINT, &value, 0, &len);

	return value;
}

float DbConnection::GetFloatData(int columnIdx)
{
	float value = 0.0f;
	SQLLEN len = 0;

	::SQLGetData(hStmt, columnIdx, SQL_C_FLOAT, &value, 0, &len);

	return value;
}

string DbConnection::GetStringData(int columnIdx)
{
	wchar_t buffer[256] = {};
	SQLLEN len = 0;

	SQLRETURN  ret = ::SQLGetData(hStmt, columnIdx, SQL_C_WCHAR, &buffer, sizeof(buffer), &len);

	if(ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		return "";
	}

	if (len == SQL_NULL_DATA)
	{
		return "";
	}

	int utf8Size = ::WideCharToMultiByte(CP_UTF8, 0, buffer, -1, nullptr, 0, nullptr, nullptr);

	if(utf8Size <= 0) {
		return "";
	}

	string value(utf8Size - 1, '\0');

	::WideCharToMultiByte(CP_UTF8, 0, buffer, -1, value.data(), utf8Size, nullptr, nullptr);

	return value;
}

SQL_TIMESTAMP_STRUCT DbConnection::GetTimestampData(int columnIdx)
{
	SQL_TIMESTAMP_STRUCT value;
	SQLLEN len = 0;

	::SQLGetData(hStmt, columnIdx, SQL_C_TYPE_TIMESTAMP, &value, sizeof(value), &len);

	return value;
}

void DbConnection::BindTimestampParameter(google::protobuf::Timestamp* timestamp, const SQL_TIMESTAMP_STRUCT& sqlTimestamp)
{
	tm timeinfo = {};
	timeinfo.tm_year = sqlTimestamp.year - 1900;
	timeinfo.tm_mon = sqlTimestamp.month - 1;
	timeinfo.tm_mday = sqlTimestamp.day;
	timeinfo.tm_hour = sqlTimestamp.hour;
	timeinfo.tm_min = sqlTimestamp.minute;
	timeinfo.tm_sec = sqlTimestamp.second;

	time_t seconds = mktime(&timeinfo);

	timestamp->set_seconds(static_cast<int64_t>(seconds));
	timestamp->set_nanos(static_cast<int32_t>(sqlTimestamp.fraction));
}





