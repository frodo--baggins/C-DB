#include "resultset.h"

cppdb::MySQLResultSet::Data::Data(MYSQL* c, MYSQL_RES* r): conn(c), res(r), sz(mysql_num_rows(res)),
	rows(new MSROWPTR[sz]) {

	MYSQL_ROW rw;
	for(size_t i = 0; rw = mysql_fetch_row(res); ++i) {
		rows[i] = new MySQLRow(conn, res, rw, i + 1);
	}
}

cppdb::MySQLResultSet::Data::~Data() {
	for(size_t i = 0; i < sz; ++i) {
		delete rows[i]; // Delete the MySQLRow objects in rows
	}

	delete[] rows; // Now delete the rows array itself

	mysql_free_result(res);
}

cppdb::MySQLResultSet::MySQLResultSet(MYSQL* c, MYSQL_RES* r): data(std::make_shared<Data>(c, r)) {}

const cppdb::Row& cppdb::MySQLResultSet::at(size_t idx) const {
	if(idx >= data->sz) throw BadColumnException("Out of range error");
	return *(data->rows[idx]);
}
