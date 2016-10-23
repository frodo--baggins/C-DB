#include <my_global.h>
#include <mysql.h>
#include <iostream>
#include <cstdlib>
#include "support/mysql/assignment_info.h"

int main() {
	MYSQL* conn = mysql_init(NULL);
	if(mysql_real_connect(conn, "127.0.0.1", "evan", "WyllaisWeird24", NULL, 0, NULL, 0) == NULL) {
		std::cerr << "Error: Could not connect to database\n";
		mysql_close(conn);
		return 1;
	}
	if(mysql_select_db(conn, "Dblib") != 0) {
		std::cerr << "Error: Could not select DB\n";
		mysql_close(conn);
		return 1;
	}
	if(mysql_query(conn, "SELECT * FROM Users") != 0) {
		std::cerr << "Error: Query failure";
		mysql_close(conn);
		return 1;
	}

	if(MYSQL_RES* res = mysql_store_result(conn)) {
		cppdb::Support::assignment_info ai = cppdb::Support::get_assignment_info(res, 1);
		std::cout << ai.table_name << ' ' << ai.key_name << ' ' << ai.key_col << '\n';
		std::cout << ai.assign_col_name << '\n';
		mysql_close(conn);
		return 0;
	} else {
		mysql_close(conn);
		return 1;
	}
}
