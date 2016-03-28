#ifndef _CPPDB_RAW_CONNECTION_H
#define _CPPDB_RAW_CONNECTION_H

#include <sys/types.h>
#include <sys/socket.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include "db_type.h"
#include "exception.h"

namespace cppdb {
	
	namespace _Bits {
		const unsigned int _BUFSIZ = 4096;	
	}
	
	template<db_type DB> class database;
	template<db_type DB> class result_set;
	
	template<db_type DB> class connection {
	public:
		connection(const char* hn, int p) : hostname_(hn), port_(p) { init(); }
		connection(const std::string& hn, int p) : hostname_(hn.c_str()), port_(p) { init(); }
		~connection() { close(sockfd); }
		
		const char* hostname() const { return hostname_; }
		int port() const { return port; }
		
		database<DB> get_database(const char* name);
		database<DB> get_database(const std::string& name);
		database<DB> create_database(const char* name);
		database<DB> create_database(const std::string& name);
		
		result_set<DB> query(const char* qry); // Only queries relating to entire database system, e.g. "SHOW databases"
		result_set<DB> query(const std::string& qry);
	private:
		int sockfd;
		const char* hostname_;
		int port_;
		
		void init() {
			struct addrinfo hints, *servinfo;
			std::memset(&hints, 0, sizeof(hints));
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;
			
			std::string portstring = to_string(port_);
			
			int err;
			
			if((err = getaddrinfo(hostname_, portstring.c_str(), &hints, &servinfo)) < 0) {
				std::string s = "Could not connect to MySQL server: ";
				s += gai_strerror(err);
				throw connect_error(s);
			}
			
			struct addrinfo* ai = NULL;
			
			for(ai = servinfo; ai != NULL; ai = ai->ai_next) {
				if((sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol)) == -1) {
					std::perror("Could not open socket");
					continue;
				}
				if(connect(sockfd, ai->ai_addr, ai->ai_addrlen) == -1) {
					std::perror("Could not connect to MySQL server");
					continue;
				}
				
				break; // connected successfully
			}
			
			freeaddrinfo(servinfo);
			
			if(ai == NULL) {
				throw connect_error("Could not connect to MySQL server");
			}
			
			char msg[_Bits::_BUFSIZ + 1]; // + 1 for null terminator
			read(sockfd, msg, _Bits::_BUFSIZ);
			std::printf("Message: %s\n", msg);
		}
				
		template<class T> std::string to_string(T arg) {
			std::ostringstream oss;
			oss << arg;
			return oss.str();
		}
	};
}

#endif