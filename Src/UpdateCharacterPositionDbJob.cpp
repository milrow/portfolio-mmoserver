#include "UpdateCharacterPositionDbJob.h"

const wchar_t* UpdateCharacterPositionDbJob::GetQuery()
{
    return L"{Call SP_UpdateCharacterPosition (?, ?, ?, ?, ?)}";
}

void UpdateCharacterPositionDbJob::BindParameters(SQLHSTMT hStmt)
{
    ::SQLBindParameter(
        hStmt,
        SQL_PARAM_INPUT,
        SQL_C_SBIGINT,
        SQL_BIGINT,
        1,
        0,
        0,
        &_characterId,
        0,
        &_characterIdLen
        );

    ::SQLBindParameter(
        hStmt,
        SQL_PARAM_INPUT,
        SQL_C_SLONG,
        SQL_INTEGER,
        2,
        0,
        0,
        &_mapId,
        0,
        &_mapIdLen
    );

    ::SQLBindParameter(
        hStmt,
        SQL_PARAM_INPUT,
        SQL_C_FLOAT,
        SQL_FLOAT,
        3,
        0,
        0,
        &_position.posx,
        0,
        &_posXLen
    );

    ::SQLBindParameter(
        hStmt,
        SQL_PARAM_INPUT,
        SQL_C_FLOAT,
        SQL_FLOAT,
        4,
        0,
        0,
        &_position.posy,
        0,
        &_posYLen
    );

    ::SQLBindParameter(
        hStmt,
        SQL_PARAM_INPUT,
        SQL_C_FLOAT,
        SQL_FLOAT,
        5,
        0,
        0,
        &_position.posz,
        0,
        &_posZLen
    );

    ::SQLBindParameter(
        hStmt,
        SQL_PARAM_INPUT,
        SQL_C_FLOAT,
        SQL_FLOAT,
        6,
        0,
        0,
        &_position.yaw,
        0,
        &_yawLen
    );
}

void UpdateCharacterPositionDbJob::Excute(DbConnection* conn)
{
    BindParameters(conn->GetStmtHandle());

    if (conn->Prepare(GetQuery()) && conn->ExcutePrepared() && conn->Fetch())
    {

    }
}
