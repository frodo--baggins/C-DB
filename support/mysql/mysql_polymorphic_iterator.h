#ifndef CPPDB_SUPPORT_MYSQL_MYSQL_POLYMORPHIC_ITERATOR_H
#define CPPDB_SUPPORT_MYSQL_MYSQL_POLYMORPHIC_ITERATOR_H

/*
 * mysql_polymorphic_iterator.h
 * 
 * Provides a class mysql_polymorphic_iterator, deriving from polymorphic_iterator. This class implements
 * the pure virtual functions that polymorphic_iterator provides, implementing them for use with a MySQL 
 * database distribution. See beginning comment at <cppdb/support/polymorphic_iterator.h> for more on
 * this.
 *
 *
 * THIS IS A SUPPORT FILE. This file, along with all other files in the support directory, should NOT be
 * included directly by library clients! All support files are used to provide necessary classes,
 * functions, etc. for use by the library headers and implementation files and are included automatically
 * by those aforementioned headers and files. If a client directly includes a support file, the behavior
 * is undefined.
 *
 */

#include "../polymorphic_iterator.h"
#include "../../mysql/row.h"

namespace cppdb {
	class MySQLRow;
	typedef MySQLRow* MSROWPTR;
	
	namespace Support {
		class mysql_polymorphic_iterator: public polymorphic_iterator {
			MSROWPTR* arr;
			size_t sz;
		public:
			mysql_polymorphic_iterator(MSROWPTR* a, size_t s): arr(a), sz(s) {}
			virtual ~mysql_polymorphic_iterator() {}
			virtual polymorphic_iterator* clone() const {
				return new mysql_polymorphic_iterator(*this);
			}

			virtual Row& get_reference() { return **arr; } // the data type MSROWPTR* is
								       // equal to MySQLRow**

			virtual Row* get_pointer() { return *arr; }
			virtual void next() { ++arr; }
			virtual void prev() { --arr; }
			virtual bool equal_to(const polymorphic_iterator* pi) const {
				if(const mysql_polymorphic_iterator* mpi = dynamic_cast<const mysql_polymorphic_iterator*>(pi)) {
					for(size_t i = 0; i < sz; ++i) {
						if(arr[i] != mpi->arr[i]) return false;
					}
					return true;
				}
			}
		};
	}
}

#endif // CPPDB_SUPPORT_MYSQL_MYSQL_POLYMORPHIC_ITERATOR_H
