#include "result_set.h"
#include <sstream>
#include <string>

cppdb::_MySQLPrivateNS::primary_key_info cppdb::_MySQLPrivateNS::get_primary_key_info(sql::Connection* conn, sql::ResultSet* res) {
	sql::ResultSetMetaData* metadata = res->getMetaData();
	std::string table_name = metadata->getTableName(1);
	std::string qry = "SHOW KEYS FROM ";
	qry += table_name;
	qry += " WHERE Key_name = 'PRIMARY'";
	std::unique_ptr<sql::Statement> stmt(conn->createStatement());
	std::unique_ptr<sql::ResultSet> res2(stmt->executeQuery(qry));
	std::string key = res2->getString("Column_name");
	//std::string colnum = res2->getString("Seq_in_index");
	//std::istringstream iss(colnum);
	int i = -2;
	//iss >> i;
	return primary_key_info(table_name, key, i);
}

void cppdb::mysql_row::MysqlProxyImpl::assign(const std::string& s) {
	using _MySQLPrivateNS::primary_key_info;
	using _MySQLPrivateNS::get_primary_key_info;
	
	primary_key_info keyinfo = get_primary_key_info(conn, res);
	// std::cout << keyinfo.key << '\n';
	std::string qry = "UPDATE "; // Query will be: "UPDATE tablename SET rowname='s' WHERE primary_key='primary_key_value'"
	qry += keyinfo.table_name;
	qry += "SET ";
	qry += rowname;
	qry += "='";
	qry += s;
	qry += "' WHERE ";
	qry += keyinfo.key;
	qry += "='";
	//std::cout << keyinfo.key << std::endl;
	qry += res->getString(keyinfo.key);
	qry += '\'';
	std::cout << qry << '\n';
	std::unique_ptr<sql::Statement> stmt(conn->createStatement());
	stmt->execute(qry);
}

cppdb::mysql_row::mysql_row(const row& rhs, sql::Connection* c, sql::ResultSet* r, size_t rn): rownum(rn), res(r),
resmeta(res->getMetaData()), conn(c), cols(rhs.size()), arr(new std::string[cols]) {
	
	using _MySQLPrivateNS::basic_update_query_container;
	
	// Create a vector to hold the insert query
	std::vector<std::string> vec = {"INSERT INTO ", resmeta->getTableName(0), " VALUES ("};
	vec.reserve(cols + 1); // Reserve enough space to hold all fields retrieved from rhs, plus the closing parenthesis
	for(size_t i = 0; i < cols; ++i) {
		arr[i] = rhs[i];
		if(i < cols - 1) {
			vec.push_back(std::string("'") + arr[i] + std::string("',")); // Note comma after closing quote
		} else { // we're at the last row, so don't add the comma at the end
			vec.push_back(std::string("'") + arr[i] + std::string("'"));
		}
	}
	vec.push_back(")");
	
	basic_update_query_container(conn, vec); // Add the new row to the database, using vec to retrieve the query pieces
}

cppdb::mysql_row::mysql_row(const mysql_row& rhs): rownum(rhs.rownum), res(rhs.res), resmeta(rhs.resmeta), conn(rhs.conn), cols(rhs.cols),
arr(new std::string[cols]) {
	for(size_t i = 0; i < cols; ++i) {
		arr[i] = rhs.arr[i];
	}
}

auto cppdb::mysql_row::operator=(const row& rhs) -> mysql_row& {
	using _MySQLPrivateNS::basic_update_query_initlist;
	
	if(this == &rhs) return *this;
	cols = rhs.size();
	delete[]	arr;
	arr = new std::string[cols];
	
	bool hit_primary_key = false; // See below
	std::string primkey_name = _MySQLPrivateNS::get_primary_key_info(conn, res).key; // See below
	
	for(size_t i = 0; i < cols; ++i) { // Assign rhs.arr[] to this->arr[] and to database
		if(!hit_primary_key) { // If we haven't found the primary key, check to make sure the current column isn't the primary key
			std::string colname = resmeta->getColumnName(i);
			if(colname == primkey_name) { // Not assigning to the primary key will prevent problems and confusion in the
				hit_primary_key = true;		// long (and short!) term. See the docs [TODO make docs] for a more detailed explanation.
				continue;
			}
			basic_update_query_initlist(conn, {"UPDATE ", resmeta->getTableName(i), " SET ", resmeta->getColumnName(i), " WHERE ", });
		}
	}
	return *this;
}
