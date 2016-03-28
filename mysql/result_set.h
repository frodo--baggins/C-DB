#ifndef CPPDB_MYSQL_RESULT_SET_H
#define CPPDB_MYSQL_RESULT_SET_H

#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/connection.h>
#include <string>
#include <vector>
#include <sstream>
#include <memory>
#include <mutex>
#include "../result_set.h"
#include "../exception.h"

namespace cppdb {
	
	namespace _MySQLPrivateNS {
		struct primary_key_info {
			std::string table_name;
			std::string key;
			int key_col;
			primary_key_info(const std::string& tn, const std::string& k, int kc) : table_name(tn), key(k), key_col(kc) {}
		};
		
		primary_key_info get_primary_key_info(sql::Connection* conn, sql::ResultSet* res);
		
		template<class T> std::string to_string(const T& value) {
			std::ostringstream oss;
			oss << value;
			return oss.str();
		}
		
		template<class Container> void basic_update_query_container(sql::Connection* conn, const Container& ctr) {
			std::string str;
			for(auto p = ctr.begin(); p != ctr.end(); ++p) {
				str += *p;	
			}
			std::unique_ptr<sql::Statement> stmt(conn->createStatement());
			stmt->executeUpdate(str);
		}
		
		void basic_update_query_initlist(sql::Connection* conn, std::initializer_list<std::string> il) {
			std::string str;
			for(auto p = il.begin(); p != il.end(); ++p) {
				str += *p;	
			}
			std::unique_ptr<sql::Statement> stmt(conn->createStatement());
			stmt->executeUpdate(str);
		}
	}
	
	
	class mysql_row: public row {
	public:
		class iterator;
		class const_iterator;
	private:
		friend class mysql_result_set; // To call protected member functions
		
		size_t rownum; // Current row number in database
		sql::ResultSet* res; // Result set
		sql::ResultSetMetaData* resmeta; // Meta data for result set
		sql::Connection* conn; // Connection pointer (to make queries)
		size_t cols; // Number of columns in database table
		std::string* arr;
		
	protected:
		mysql_row(size_t rn = 0, sql::ResultSet* r = nullptr, sql::Connection* c = nullptr): rownum(rn), res(r), resmeta(res->getMetaData()),
		conn(c), cols(resmeta->getColumnCount()), arr(new std::string[cols]) {
			for(size_t i = 0; i < cols; ++i) {
				arr[i] = res->getString(i + 1); // MySQL Connector C++ operates in "1-based" indices (as opposed to 0-based, as arrays)
			}
		}
		
		static void adjust_res(sql::ResultSet* r, int x) {
			int curr_row = r->getRow();
			if(curr_row != x) r->relative(x - curr_row);
			r->relative(1); // MySQL Connector C++ does not operate in array-index style
		}
		
		class MysqlProxyImpl: public row::ProxyImpl {
			std::string str;
			sql::Connection* conn;
			sql::ResultSet* res;
			std::string rowname;
		public:
			MysqlProxyImpl(const std::string& s, sql::Connection* c, sql::ResultSet* r, const std::string& rn) 
			: str(s), conn(c), res(r), rowname(rn) {}
			virtual ~MysqlProxyImpl() {}
			
			virtual void assign(const std::string&);
			
			virtual std::string to_string() { return str; }
		};
		
	public:
		typedef row::size_type size_type;			
		
		virtual size_t size() const { // Get number of columns in database
			return cols;
		}
		
		mysql_row(const row& rhs, sql::Connection* c, sql::ResultSet* r, size_t rn);
		
		mysql_row(const mysql_row& rhs);
		
		mysql_row(mysql_row&& rhs) = default;
		mysql_row& operator=(mysql_row&& rhs) = default;
		
		virtual mysql_row& operator=(const row&);
		
		mysql_row& operator=(const mysql_row& rhs) {
			return operator=(rhs); // operator=(const row&)
		}

		virtual ~mysql_row() { delete[] arr; }
		
		
		class iterator {
			friend class mysql_row;
			sql::Connection* conn;
			sql::ResultSet* res;
			size_t curr_col;
		protected:
			iterator(sql::Connection* c, sql::ResultSet* r, size_t cc): conn(c), res(r), curr_col(cc) {}
		public:
			iterator(const iterator&) = default;
			iterator& operator=(const iterator&) = default;
			~iterator() {}
		};
		
		virtual std::string operator[](size_type idx) const { adjust_res(res, rownum); return res->getString(idx); }
		virtual std::string operator[](const std::string& key) const { adjust_res(res, rownum); return res->getString(key); }
		virtual std::string at(size_type idx) const {
			try {
				adjust_res(res, rownum);
				return res->getString(idx);
			} catch(sql::SQLException& ex) {
				std::ostringstream err("result_set::row::at(size_type): Column ");
				err << idx;
				err << " is out of bounds";
				throw bad_column(err.str());
			}
		}
		
		virtual std::string at(const std::string& key) const {
			try {
				adjust_res(res, rownum);
				return res->getString(key);
			} catch(sql::SQLException& ex) {
				std::string err = "result_set::row::at(const std::string&): No column exists with name \"";
				err += key;
				err += "\"";
				throw bad_column(err);
			}
		}
		
		virtual db_proxy operator[](const std::string& key) {
			adjust_res(res, rownum);
			std::string val = res->getString(key);
			return db_proxy(new MysqlProxyImpl(val, conn, res, key));
		}
		
		virtual db_proxy at(const std::string& key) {
			try {
				adjust_res(res, rownum);
				std::string val = res->getString(key);
				return db_proxy(new MysqlProxyImpl(val, conn, res, key));
			} catch(sql::SQLException& ex) {
				std::string err = "result_set::row::at(const std::string&): No column exists with name \"";
				err += key;
				err += "\" exists";
				throw bad_column(err);
			}
		}
		
		virtual row* clone() const { return new mysql_row(*this); }
	};
	
	
	/*
	 * End class mysql_row
	 */
	
	
	
	
	
	
	
	class mysql_result_set: public result_set, private result_set::const_iterator {
		// Private inheritance used to call protected member function [const_]iterator::get_raw_iter()
	public:
		typedef result_set::iterator iterator;
		typedef result_set::const_iterator const_iterator;
		typedef row::db_proxy db_proxy;
		
	private:
		struct Node {
			Node* prev;
			Node* next;
			mysql_row rw;
			Node(Node* p, Node* n, const mysql_row& r) : prev(p), next(n), rw(r) {}
		}; // end struct Node; continue class mysql_result_set
		
	public:
			
		class _mysql_iterator : public result_set::polymorphic_iterator {
			friend class mysql_result_set;
			sql::ResultSet* res;
			Node* curr_node;
			Node* first;
			Node* last;
			_mysql_iterator(sql::ResultSet* r, Node* n, Node* f, Node* l) : res(r), curr_node(n), first(f), last(l) {}
		protected:
			Node* get_current_node() {
				return curr_node;
			}
		public:
			_mysql_iterator(const _mysql_iterator&) = default;
			virtual ~_mysql_iterator() {}
			virtual polymorphic_iterator* clone() const { return new _mysql_iterator(*this); }
			
			virtual row& get_reference() {
				return curr_node->rw;
			}
			virtual row* get_pointer() {
				return &curr_node->rw;
			}
			virtual void next() {
				if(curr_node == nullptr) curr_node = first;
				else curr_node = curr_node->next;
			}
			virtual void prev() {
				if(curr_node == nullptr) curr_node = last;
				curr_node = curr_node->prev;
			}
			virtual bool equal_to(const result_set::polymorphic_iterator* ptr) const {
				if(_mysql_iterator* tmp = dynamic_cast<_mysql_iterator*>(ptr)) {
					return curr_node == tmp->curr_node;
				}
				throw db_exception("Implemeter error: Invalid iterator");
				return false; // Dummy return statement
			}
		}; // end class mysql_iterator; continue class mysql_result_set
		
	private:
		sql::Connection* conn;
		sql::ResultSet* res;
		Node* head;
		Node* tail;
		mutable char* tblnam;
		char* primkey_name;
		size_t sz;
		
		_mysql_iterator* get_mysql_iterator(const_iterator it) {
			if(_mysql_iterator* ptr = dynamic_cast<_mysql_iterator*>(it.get_raw_iter()) {
				return ptr;			
			}
			throw db_exception("Implemeter error: Invalid iterator");
			return nullptr; // Dummy return statement
		}
		
		void get_table_name() const {
			if(tblnam != nullptr) return;
			try {
				sql::ResultSetMetaData* meta_data = res->getMetaData();
				std::string s = meta_data->getTableName(1);
				tblnam = new char[s.length() + 1];
				for(size_t i = 0; i < s.length(); ++i) {
					tblnam[i] = s[i];				
				}
			} catch(sql::SQLException& ex) {
				std::cout << "Exception caught (get_table_name): " << ex.what() << '\n';
			}
		}
		
		void get_primary_key_name() {
			if(primkey_name != nullptr) return;
			try {
				// Build the query
				std::string qry = "SHOW KEYS FROM ";
				get_table_name();
				qry += tblnam;
				qry += " WHERE Key_name = 'PRIMARY'";
			
				std::unique_ptr<sql::ResultSet> table_res(basic_query(qry));
				table_res->next();
				std::string s = table_res->getString("Column_name");
				primkey_name = new char[s.length() + 1];
				for(size_t i = 0; i < s.length(); ++i) {
					primkey_name[i] = s[i];				
				}
			} catch(sql::SQLException& ex) {
				std::cout << "Exception caught (get_primary_key_name): " << ex.what() << '\n';
			}
		}
		
		sql::ResultSet* basic_query(const std::string& qry) {
			std::unique_ptr<sql::Statement> stmt(conn->createStatement());
			if(stmt->execute(qry)) {
				return stmt->getResultSet();
			} else {
				return nullptr;			
			}
		}
		
		void deallocate_linked_list() {
			for(Node* curr = head; curr != nullptr; ) {
				Node* temp = curr->next;
				delete curr;
				curr = temp;
			}
		}
		
	protected:
		friend class mysql_statement; // To call constructor
		explicit mysql_result_set(sql::Connection* c, sql::ResultSet* r)
		: conn(c), res(r), tblnam(nullptr), primkey_name(nullptr) {
			if(!res->isBeforeFirst()) { // Check whether result set is empty
				head = tail = nullptr;
				sz = 0;
			} else {
				size_t rownum = 0;
				sz = res->getRow();
				res->first();
				head = new Node(nullptr, nullptr, mysql_row(rownum, res, conn));
				Node* last_node = head;
				while(res->next()) {
					last_node->next = new Node(last_node, nullptr, mysql_row(++rownum, res, conn));
					last_node = last_node->next;
				}
				tail = last_node;
				res->first();
			}
		}
	public:
		
		typedef result_set::value_type value_type;
		typedef result_set::pointer pointer;
		typedef result_set::const_pointer const_pointer;
		typedef result_set::reference reference;
		typedef result_set::const_reference const_reference;
		typedef result_set::size_type size_type;
		
		
		// virtual result_set* clone() const { return new mysql_result_set(*this); }
		virtual ~mysql_result_set() {
			delete[] tblnam;
			delete[] primkey_name;
			deallocate_linked_list();
			delete res;
		}
		
		virtual size_t size() const { return sz; }
		virtual bool empty() const { return sz == 0; }
		
		virtual const char* name() const {
			get_table_name();
			return tblnam;
		}
		
		virtual iterator begin() {
			return iterator(new _mysql_iterator(res, head, head, tail));
		}
		
		virtual iterator end() {
			return iterator(new _mysql_iterator(res, nullptr, head, tail));
		}
		
		virtual void insert(const value_type& val) {
			tail->next = new Node(tail, nullptr, mysql_row(val, conn, res, ++sz));
			tail = tail->next;
		}

		virtual void insert(const_iterator hint, const value_type& val) {
			_mysql_iterator* ptr = get_mysql_iterator(hint);
			Node* node = ptr->get_current_node();
			Node* new_node = new Node(node->prev, node->next, mysql_row(val, conn, res, ++sz));
		}
		
		virtual iterator erase(iterator pos) {
			_mysql_iterator* ptr = get_mysql_iterator(pos);
			std::string qry = "DELETE FROM ";
			get_table_name(); // Fill tblnam
			get_primary_key_name(); // Fill primkey_name
			std::string primary_key_val = (*pos)[primkey_name];
			
			qry += tblnam; // In effect: qry = "DELETE FROM table_name WHERE primary_key=primary_key_value"
			qry += " WHERE ";
			qry += primkey_name;
			qry += "='";
			qry += primary_key_val;
			qry += "'";
			
			--sz;
			++pos;
			
			basic_query(qry);
			
			return pos;
		}
		
		virtual void clear() {
			std::string qry = "DELETE FROM ";
			get_table_name();
			qry += tblnam;
			qry += " WHERE 1=1";
			deallocate_linked_list();
			head = tail = nullptr;
			sz = 0;
			basic_query(qry);
		}
	};
}


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

#endif // CPPDB_MYSQL_RESULT_SET_H