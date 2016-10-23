#ifndef _CPPDB_SUPPORT_MYSQL_PRIMARY_KEY_INFO_H
#define _CPPDB_SUPPORT_MYSQL_PRIMARY_KEY_INFO_H

/*
 * assignment_info.h
 * 
 * Provides the struct AssignmentInfo and the function get_assignment_info(), which are used together
 * to fetch and read the information needed to assign to a particular field.
 *
 * THIS IS A SUPPORT FILE. This file, along with all other files in the support directory, should NOT be
 * included directly by library clients! All support files are used to provide necessary classes,
 * functions, etc. for use by the library headers and implementation files and are included automatically
 * by those aforementioned headers and files. If a client directly includes a support file, the behavior
 * is undefined.
 *
 */

#include <my_global.h>
#include <mysql.h>
#include "../../exception.h"

namespace cppdb {
	namespace Support {
		struct AssignmentInfo {
			std::string table_name;
			
			std::string key_name;
			int key_col;

			std::string assign_col_name;
			
			AssignmentInfo(): table_name(""), key_name(""), key_col(-1),
				assign_col_name("") {}
			AssignmentInfo(const std::string& tn, const std::string& kn, unsigned int kc,
				const std::string& acn): table_name(tn), key_name(kn), key_col(kc),
				assign_col_name(acn) {}
		};

		AssignmentInfo get_assignment_info(MYSQL_RES* res, int colnum) {
			// Get all the information needed to perform an assignment to a database value
			// This assignment is performed in the background; the programmer does not need
			// to write explicit queries.
			//
			// MYSQL_RES* res: the result set
			// int colnum: the column number (0-indexed) for the desired field to modify


			MYSQL_FIELD* field;
			AssignmentInfo ai;	
			for(int i = 0; field = mysql_fetch_field(res); ++i) {
				ai.table_name = field->table;
				if(IS_PRI_KEY(field->flags)) {
					ai.key_name = field->name;
					ai.key_col = i;
				} else if(i == colnum) {
					ai.assign_col_name = field->name;
				} else if(IS_PRI_KEY(field->flags) && i == colnum) {
					throw cppdb::AssignmentException("Cannot modify primary key");
				}
			}
			
			return ai;
		}
	}
}

#endif // _CPPDB_SUPPORT_MYSQL_PRIMARY_KEY_INFO_H
