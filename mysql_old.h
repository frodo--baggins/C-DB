#ifndef _CPPDB_MYSQL_H
#define _CPPDB_MYSQL_H

#if  !defined(_CPPDB_CONNECTION_H) || !defined(_CPPDB_DATABASE_H) || !defined(_CPPDB_RESULT_SET_H)
#error "<cppdb/connection.h>, <cppdb/result_set.h> and <cppdb/database.h> must be included before including <cppdb/mysql.h>"
#endif

#include "exception.h"
#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <boost/shared_ptr.hpp>
#include <string>

namespace cppdb {
	class mysql_connection: public connection {
		sql::Driver* driver;
		boost::shared_ptr<sql::Connection> conn;
	public:
		connection(const char* url, const char* username, const char* password) : driver(NULL), conn(NULL) {
			driver = get_driver_instance();
			conn = driver->connect(url, username, password);
		}
	};
	
	class mysql_result_set: public result_set {
		boost::shared_ptr<sql::ResultSet> res;
		result_set(sql::Statement* stmt, const char* qry) : res(stmt->executeQuery(qry)) {
			delete stmt;
		}
	public:
		class row {
			boost::shared_ptr<sql::ResultSet> res;
			size_t rownum;
		public:
		
			row(const boost::shared_ptr<sql::ResultSet>& pr, size_t rn) : res(pr), rownum(rn) {}
			std::string operator[](size_t i) {
				return res->getInt(s);
			}
			std::string operator[](const char* col) {
				return res->getString(col);			
			}
			
			size_t rows() const { return rownum; }
		};
		
		class iterator {
			friend class row;
			row* rw;
		public:
			iterator(row* r) : rw(r) {}
			
			iterator& operator++() {
				if(rw) {
					if(!(rw->res->next())) {
						rw = NULL;					
					}
				}
			}
			
			const iterator operator++(int) {
				iterator it = *this;
				++(*this);
				return it;			
			}
			
			row& operator*()  { return *rw; }
			row* operator->() { return rw;  }
		};
	};
	
	class mysql_database: public database {
		friend class connection; // Access connection<mysql>::conn variable
		friend class result_set; // Access result_set constructor
		boost::shared_ptr<sql::Connection> conn;
		const char* nme;
	public:
		database(const connection& c, const char* db_name) : conn(c.conn), nme(db_name) {
			conn->setSchema(nme);
		}
		database(const connection& c, const std::string& db_name) : conn(c.conn), nme(db_name.c_str()) {
			conn->setSchema(nme);		
		}
		
		const char* name() const { return nme; }
		
		result_set<mysql> query(const char* qry) {
			sql::Statement* stmt = conn->createStatment();
			return result_set<mysql>(stmt, qry);
		}
	}
}

#endif // _CPPDB_MYSQL_H
