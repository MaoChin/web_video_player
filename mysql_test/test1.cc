#include <iostream>
#include <mysql/mysql.h>

using namespace std;
#define HOST "127.0.0.1"
#define USER "root"
#define PASSWARD ""
#define DBNAME "test_db"
#define PORT 3306
#define CHARACTER "utf8"

void insert(MYSQL* mysql)
{
	const char* sql = "insert into Stu values (1, 19, 'zhangsan', 11.1), (2, 22, 'lisi', 22.2);";
	int ret = mysql_query(mysql, sql);
	if(ret != 0)
	{
		cout << "insert: mysql_query error: " << mysql_error(mysql) << endl;
		return;
	}
}


int main()
{
	// 创建 mysql 句柄
	MYSQL mysql;
  mysql_init(&mysql);

	// if(mysql == nullptr)
	// {
	// 	cout << "mysql_init error: " << mysql_error(&mysql) << endl;
	// 	return 1;
	// }
	// 连接服务器
	if(!mysql_real_connect(&mysql, HOST, USER, PASSWARD, DBNAME, 0, NULL, 0))
	{
		cout << "mysql_real_connect error: " << mysql_error(&mysql) << endl;
		return 2;
	}
	// 设置字符集
	mysql_set_character_set(&mysql, CHARACTER);

	// 选择操作的数据库，可以省略，因为上面连接数据库时已经选择了
	mysql_select_db(&mysql, DBNAME);

	// insert(&mysql);
	mysql_close(&mysql);

	return 0;
}
