#pragma once

#include "Resource/LoginServerResource.h"

//#pragma comment(lib, "mysqlclient.lib")
#pragma comment(lib, "libmysql.lib")

#include "mysql.h"
#include "errmsg.h"

class CDBConnector
{
public:
	MYSQL conn;
	MYSQL* connection = NULL;
	MYSQL_RES* sql_result;
	MYSQL_ROW sql_row;
	int query_stat;



	CDBConnector()
	{
		mysql_init(&conn);

		//todo//read config ����
		// ��� ������ DB ip ���� �� ����
		//connection �ٷ� ����

		connection = mysql_real_connect(&conn, "127.0.0.1", "root", "12341234", "test", 3306, (char*)NULL, 0);

		if (connection == NULL)
		{
			// mysql_errno(&_MySQL);
			fprintf(stderr, "Mysql connection error : %s", mysql_error(&conn));
			__debugbreak();
		}
	}

	~CDBConnector()
	{
		mysql_close(connection);
	}

	bool DoQuery(char* query)
	{
		query_stat = mysql_query(connection, query);
		if (query_stat != 0)
		{
			printf("Mysql query error : %s", mysql_error(&conn));
			__debugbreak();
			return false;
		}
	}

	MYSQL_RES* StoreResult()
	{
		sql_result = mysql_store_result(connection);

		return sql_result;
	}

	bool FreeResult()
	{
		mysql_free_result(sql_result);
		return true;
	}


};

/*
--------------------------------------------------------------------------


C Connector ����  Mysql 8.0 ����� �Ʒ� ������ ���� �ݿ��Ͽ�
�н����� ����� ���� ������� ���� �ؾ� �մϴ�.

ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY 'password';


// ������
sql_result = mysql_store_result(connection);		// ��� ��ü�� �̸� ������
//	sql_result=mysql_use_result(connection);		// fetch_row ȣ��� 1���� ������

while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
{
	printf("%2s %2s %s\n", sql_row[0], sql_row[1], sql_row[2]);
}
mysql_free_result(sql_result);

// DB ����ݱ�
mysql_close(connection);




//	int a = mysql_insert_id(connection);


//	query_stat = mysql_set_server_option(connection, MYSQL_OPTION_MULTI_STATEMENTS_ON);
//	mysql_next_result(connection);				// ��Ƽ���� ���� ���� ��� ���
//	sql_result=mysql_store_result(connection);	// next_result �� ��� ����

*/


class CDBManager : public CDBConnector 
{


public:

	bool LoginDataRequest(CPacket* message, ULONG64 characterKey);


};


