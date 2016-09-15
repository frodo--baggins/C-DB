#ifndef _CPPDB_SUPPORT_ITERATOR_BASE_H
#define _CPPDB_SUPPORT_ITERATOR_BASE_H

/*
 * iterator_base.h
 * 
 * Provides the class iterator_base, which acts as a base class for the classes result_set::iterator
 * and result_set::const_iterator. This base class is needed only so that one can compare iterators and
 * const_iterators.
 *
 * THIS IS A SUPPORT FILE. This file, along with all other files in the support directory, should NOT be
 * included directly by library clients! All support files are used to provide necessary classes,
 * functions, etc. for use by the library headers and implementation files and are included automatically
 * by those aforementioned headers and files. If a client directly includes a support file, the behavior
 * is undefined.
 *
 */

#include "polymorphic_iterator.h"

namespace cppdb {
	namespace Support {
		class iterator_base {
		protected:
			polymorphic_iterator* pi;
		public:
			iterator_base(polymorphic_iterator* p): pi(p) {}
			iterator_base(const iterator_base& rhs): pi(rhs.pi->clone()) {}
			~iterator_base() { delete pi; }
			bool equal_to(const iterator_base& ib) const {
				return pi->equal_to(ib.pi);
			}
		};
	}
}

#endif // _CPPDB_SUPPORT_ITERATOR_BASE_H
