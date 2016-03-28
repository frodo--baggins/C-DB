#include "mysql.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>

int main() {
	try {
		cppdb::connection* conn = cppdb::connect<cppdb::mysql>("tcp://localhost:3306", "evan", "");
		conn->select_database("nbloggers");
		cppdb::statement* stmt = conn->make_statement();
		cppdb::result_set* res = stmt->query("SELECT * FROM users");
		
		for(auto it = res->begin(); it != res->end(); ++it) {
			std::cout << (*it)["password"] << '\n';		
		}
		
		delete res;
		delete stmt;
		delete conn;
		
	} catch(cppdb::db_exception& dbex) {
		std::cerr << "Error: " << dbex.what() << '\n';	
	}
}
