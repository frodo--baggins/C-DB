#ifndef CPPDB_SSLCONNECTION_H
#define CPPDB_SSLCONNECTION_H

#include "connection.h"

namespace cppdb {
	
	class SSLConnection: public DBConnection {
	public:
		virtual ~SSLConnection() {}
		virtual cipher_type ciphers() = 0;
		virtual std::string ca_path() = 0;
		virtual std::string ca_pathname() = 0;
		virtual std::string key_pathname() = 0;
		virtual std::string certificate_pathname() = 0;
	};
}

#endif // CPPDB_SSLCONNECTION_H
