#include "mysql.h"

mysql_result_set::iterator mysql_result_set::begin() {
	if(!res->first()) throw db_exception();
	boost::shared_ptr<mysql_row> rw(new mysql_row(*this));
	boost::shared_ptr<polymorphic_iterator> tmp(static_cast<polymorphic_iterator*>(new _mysql_iterator(rw)));
	return iterator(tmp);
}

mysql_result_set::iterator mysql_result_set::end() {
	return end_iter;
}