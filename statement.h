#ifndef _CPPDB_STATEMENT_H
#define _CPPDB_STATEMENT_H

#include <vector>
#include <memory>
#include <string>
#include <utility>
#include "result_set.h"

namespace cppdb {
	class result_set;
	class mysql_statement;
	
	class multi_query_t {
		friend class mysql_statement;
		typedef std::vector<result_set*> arr_type;
		arr_type arr;
		
		result_set*& operator[](size_t idx) { return arr[idx]; }
		result_set*& at(size_t idx) { return arr.at(idx); }
	public:
		multi_query_t(size_t sz) : arr(sz) {}
		result_set* operator[](size_t idx) const { return arr[idx]; }
		result_set* at(size_t idx) const { return arr.at(idx); }
		size_t size() const { return arr.size(); }
		bool empty() const  { return arr.empty(); }
		
		typedef arr_type::const_iterator iterator; // Disallow modification of elements
		iterator begin() const { return arr.begin(); }
		iterator end() const { return arr.end(); }
	};
	
	class statement {
	public:
		virtual ~statement() {}
		virtual result_set* query(const std::string&) = 0;
		virtual multi_query_t multi_query(const std::string&) = 0;
	};
	
	class prepared_statement: public statement {
	public:
		virtual ~prepared_statement() {}
		//template<class T, class... Args> virtual void set_placeholders(T, Args...) = 0;
	};
	
	
	namespace _Impl {
		std::vector<std::string>* split(const std::string& str, char split_char) {
			// Create a heap-allocated vector.
			// This is to avoid the possible overhead of copying the entire vector when the function returns by value
			std::vector<std::string>& arr = *new std::vector<std::string>;
			std::string tmp;
			for(size_t i = 0; i < str.length(); ++i) {
				if(str[i] == split_char) {
					arr.push_back(tmp);
					tmp = "";				
				} else {
					tmp.push_back(str[i]);
				}
			}
			arr.push_back(tmp);
			return &arr;
		}
	}
}

#endif // _CPPDB_STATEMENT_H
