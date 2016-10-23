#include "row.h"

void cppdb::MySQLRow::fill_map() const { // the map is mutable, but only so that it can be filled
					 // the first time
	if(ktrn != nullptr) return; // We've already filled the map

	MYSQL_FIELD* field = mysql_fetch_field(res);
	
	ktrn = new MapType();
	for(size_t i = 0; field = mysql_fetch_field(res); ++i) {
		(*ktrn)[field->name] = i;
		(*ktrn)[field->org_name] = i; // Handle aliases
	}
}

void cppdb::MySQLRow::range_check(size_t idx) const {
	if(idx >= Row::size()) {
		throw BadColumnException("Out of range error");
	}
}

cppdb::MySQLRow& cppdb::MySQLRow::operator=(const cppdb::Row& rhs) {
	if(this == &rhs) return *this;
			
	// Prepare for the database assignment
	AssignmentInfo ai = Support::get_assignment_info(res, 0)
	MYSQL_FIELD* field = mysql_fetch_field(res);
	std::string key_val = Row::fetch_from_array(ai.key_col); // Get the primary key value using ai

	Row::operator=(rhs);

	Row::assign_to_array(ai.key_col, key_val); // We want the primary key value to
								   // stay the same
	
	for(int i = 0; field = mysql_fetch_field(res); ++i) {
	// Assign to values in database (excepting the primary key)
		Support::query_mysql_update(conn, {"UPDATE ", ai->table_name, " SET ",
		ai->key_name, "='", Row::fetch_from_array(i), "' WHERE ", ai->key_name,
		"='", Row::fetch_from_array(ai.key_col), "'"});
	}

	return *this;
}
