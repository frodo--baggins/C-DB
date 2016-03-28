#include "result_set.h"

namespace cppdb {
	class has_data {
		const char* data;
	public:
		has_data(const char* d) : data(d) {}
		
		bool operator()(const row& rw) {
			for(size_t i = 0; i < rw.size(); ++i) {
				if(rw[i] == data) return true;			
			}
			return false;
		}
	};
	
	class column_has_data {
		const char* column;
		const char* data;
	public:
		column_has_data(const char* c, const char* d) : column(c), data(d) {}
		
		bool operator()(const row& rw) {
			return rw.at(column) == data;		
		}
	};
	
	class index_has_data {
		size_t idx;
		const char* data;
	public:
		index_has_data(size_t i, const char* d): idx(i), data(d) {}
		bool operator()(const row& rw) {
			return rw.at(idx) == data;		
		}
	};
}