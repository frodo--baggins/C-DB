#ifndef _CPPDB_MYSQL_DATASET_H
#define _CPPDB_MYSQL_DATASET_H

#include <my_global.h>
#include <mysql.h>
#include <memory>
#include "../resultset.h"
//#include "../support/mysql/assignment_info.h"
//#include "../support/mysql/query.h"
#include "../support/mysql/mysql_polymorphic_iterator.h"
#include "row.h"


namespace cppdb {
	
	class MySQLResultSet: public ResultSet {
		
		struct Data {
			MYSQL* conn;
			MYSQL_RES* res;
			size_t sz;
			MSROWPTR* rows;
			Data(MYSQL* c, MYSQL_RES* r);
			~Data();
		};

		std::shared_ptr<Data> data;
		
		MySQLResultSet(const MySQLResultSet& rhs): data(rhs.data) {}
	public:
		MySQLResultSet(MYSQL* c, MYSQL_RES* r);
		virtual ResultSet* clone() const { return new MySQLResultSet(*this); }

		virtual size_t size() const { return data->sz; }
		virtual bool empty() const { return data->sz == 0; }

		virtual const Row& operator[](size_t idx) const { return *(data->rows[idx]); }
		virtual const Row& at(size_t idx) const;
		virtual const Row& front() const { return *(data->rows[0]); }
		virtual const Row& back() const { return *(data->rows[data->sz - 1]); }

		virtual iterator begin() const {
			return iterator(new Support::mysql_polymorphic_iterator(data->rows, data->sz));
		}
		virtual iterator end() const {
			return iterator(new Support::mysql_polymorphic_iterator(data->rows + data->sz,
				data->sz));
		}
	};
}

#endif // _CPPDB_MYSQL_DATASET_H
