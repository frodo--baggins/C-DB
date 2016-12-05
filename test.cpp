#include <my_global.h>
#include <mysql.h>
#include <iostream>

int main() {
	MYSQL* conn = mysql_init(NULL);
	if(mysql_real_connect(conn, "127.0.0.1", "evan", "WyllaisWeird24", NULL, 0, NULL, 0) == NULL) {
		std::cerr << "Error: Could not connect to database: " << mysql_error(conn) << '\n';
		mysql_close(conn);
		return 1;
	}

	if(mysql_select_db(conn, "Dblib")) {
		std::cerr << "Error: Could not select database: " << mysql_error(conn) << '\n';
		mysql_close(conn);
		return 1;
	}

	if(mysql_query(conn, "SELECT * FROM Users")) {
		std::cerr << "Error: Could not execute query: " << mysql_error(conn) << '\n';
		mysql_close(conn);
		return 1;
	}

	if(MYSQL_RES* res = mysql_store_result(conn)) {
		for(int i = 0; i < mysql_num_rows(res); ++i) {
			MYSQL_ROW rw = mysql_fetch_row(res);
			for(int j = 0; j < mysql_num_fields(res); ++j) {
				std::cout << rw[j] << ' ';
			}
			std::cout << '\n';
		}

		mysql_free_result(res);
	} else {
		std::cerr << "Error: Could not fetch result set: " << mysql_error(conn) << '\n';
		mysql_close(conn);
		return 1;
	}

	mysql_close(conn);
	return 0;
}
