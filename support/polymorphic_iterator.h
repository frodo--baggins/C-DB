#ifndef _CPPDB_SUPPORT_POLYMORPHIC_ITERATOR_H
#define _CPPDB_SUPPORT_POLYMORPHIC_ITERATOR_H

/*
 * polymorphic_iterator.h
 * 
 * Provides the class polymorphic_iterator. 
 * This class acts as a base class. Each database distribution's result_set class contains
 * polymorphic iterator classes that derive from these two classes. Polymorphic iterators act as the guts
 * of the STL-style iterators provided by the (base) result_set class, taking care of the internals of
 * incrementing, decrementing, and dereferencing. This allows one to write code such as
 * 
 * 	cppdb::ResultSet::iterator i = db_distribution_result_set_ptr->begin();
 *
 * and still get a valid iterator i that works properly for that database distribution.
 *
 *
 * THIS IS A SUPPORT FILE. This file, along with all other files in the support directory, should NOT be
 * included directly by library clients! All support files are used to provide necessary classes,
 * functions, etc. for use by the library headers and implementation files and are included automatically
 * by those aforementioned headers and files. If a client directly includes a support file, the behavior
 * is undefined.
 *
 */

#include "../row.h"

namespace cppdb {

	class Row;

	namespace Support {

		class polymorphic_iterator {
		public:
			virtual ~polymorphic_iterator() {}
                        virtual polymorphic_iterator* clone() const = 0;
                        virtual Row& get_reference() = 0;
                        virtual Row* get_pointer() = 0;
                        virtual void next() = 0;
                        virtual void prev() = 0;
                        virtual bool equal_to(const polymorphic_iterator*) const = 0;
		};
	}
}

#endif // _CPPDB_SUPPORT_POLYMORPHIC_ITERATOR_H
