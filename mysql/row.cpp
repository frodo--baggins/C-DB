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
