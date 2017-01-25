#include "DB.hpp"
#include "Resultset.hpp"
#include "Query.hpp"

#include <mysql/mysql.h>
#include <cppconn/connection.h>
#include <cppconn/driver.h>
#include <time.h>
#include <stdlib.h>


namespace DORM {

	thread_local std::unique_ptr<sql::Connection> DB::conn;
	thread_local DB::ThreadGuard thread_guard;
	std::string DB::uri, DB::user, DB::password, DB::schema;
	bool DB::ThreadGuard::first_init = true;


	void DB::connect() {
		sql::Driver *driver = get_driver_instance();

		try {
			conn.reset( driver->connect(uri, user, password) );
		} catch( sql::SQLException &e ) {
			std::cerr << "[DORM] " << e.getErrorCode() << ": " << e.what() << std::endl;
			throw e;
		}

		bool opt_reconnect = true;
		conn->setClientOption("OPT_RECONNECT", &opt_reconnect);

		conn->setSchema( schema );

		execute("SET NAMES utf8");
		execute("SET @@session.tx_isolation = @@global.tx_isolation");
	}


	DB::ThreadGuard::ThreadGuard() {
		if (first_init) {
			#ifdef DORM_DB_DEBUG
				std::cout << "[DORM] FIRST thread guard" << std::endl;
			#endif

			first_init = false;

			// is this (and hence the whole "first_init" code) even needed?
			mysql_library_init(0, NULL, NULL);
		}

		#ifdef DORM_DB_DEBUG
			std::cout << "[DORM] New thread guard using " << uri << std::endl;
		#endif

		sql::Driver *driver = get_driver_instance();
		driver->threadInit();
	}


	DB::ThreadGuard::~ThreadGuard() {
		#ifdef DORM_DB_DEBUG
			std::cout << "[DORM] Destroying thread guard" << std::endl;
		#endif

		// We could do explicit disconnection here

		sql::Driver *driver = get_driver_instance();
		driver->threadEnd();
	}


	void DB::connect( const std::string db_uri, const std::string db_user, const std::string db_password, const std::string db_schema ) {
		#ifdef DORM_DB_DEBUG
			std::cout << "[DORM] Connect" << std::endl;
		#endif

		// these must be set before any potential threading happens
		uri = db_uri;
		user = db_user;
		password = db_password;
		schema = db_schema;

		connect();

		#ifdef DORM_DB_DEBUG
			std::cout << "[DORM] Connected" << std::endl;
		#endif
	}


	void DB::connect( sql::Connection *new_conn ) {
		conn.reset(new_conn);
	}


	bool DB::check_connection() {
		if (!conn)
			connect();

		return conn && conn->isValid();
	}


	Resultset *DB::select( const Query &query ) {
		std::string sql = query.to_string();

		try {
			#ifdef DORM_DB_DEBUG
				std::cout << "[DORM] Prepare SQL: " << sql << std::endl;
			#endif

			std::unique_ptr<sql::PreparedStatement> pstmt( conn->prepareStatement(sql) );

			unsigned int bind_offset = 1;

			query.bind( *pstmt, bind_offset );

			return static_cast<Resultset *>( pstmt->executeQuery() );
		} catch (sql::SQLException &e) {
			std::cerr << "[DORM] " << e.getErrorCode() << ": " << e.what() << std::endl;
			std::cerr << "[DORM] " << sql << std::endl;
			throw(e);
		}
	}


	uint64_t DB::fetch_uint64( const Query &query ) {
		std::unique_ptr<Resultset> results( select(query) );

		if ( results && results->next() )
			return results->getUInt64(1);

		return 0;
	}


	int64_t DB::fetch_int64( const Query &query ) {
		std::unique_ptr<Resultset> results( select(query) );

		if ( results && results->next() )
			return results->getInt64(1);

		return 0;
	}


	double DB::fetch_double( const Query &query ) {
		std::unique_ptr<Resultset> results( select(query) );

		if ( results && results->next() )
			return results->getDouble(1);

		return 0.0;
	}


	std::string DB::fetch_string( const Query &query ) {
		std::unique_ptr<Resultset> results( select(query) );

		if ( results && results->next() )
			return results->getString(1);

		return "";
	}


	int DB::writerow( const std::string &table, const std::vector<Where> &inserts_and_updates ) {
		// the same data is used in the INSERT clause as the ON UPDATE clause
		return writerow( table, inserts_and_updates, inserts_and_updates );
	}


	int DB::writerow( const std::string &table, const std::vector<Where> &inserts, const std::vector<Where> &updates ) {
		std::string sql;

		// if there are no columns to set/update then use special form of SQL:
		if ( inserts.empty() ) {
			sql = "INSERT INTO " + table + " VALUES ()";
		} else {
			if ( updates.empty() )
				sql = "INSERT IGNORE INTO " + table + " SET ";
			else
				sql = "INSERT INTO " + table + " SET ";

			for(auto &insert : inserts)
				sql += insert.to_string() + ", ";

			sql.pop_back();
			sql.pop_back();

			if ( !updates.empty() ) {
				sql += " ON DUPLICATE KEY UPDATE ";

				for(auto &update : updates)
					sql += update.to_string() + ", ";

				sql.pop_back();
				sql.pop_back();
			}
		}

		try {
			std::unique_ptr<sql::PreparedStatement> pstmt( conn->prepareStatement(sql) );

			unsigned int bind_offset = 1;

			for(auto &insert : inserts)
				insert.bind( *pstmt, bind_offset );

			for(auto &update : updates)
				update.bind( *pstmt, bind_offset );

			return pstmt->executeUpdate();
		} catch (sql::SQLException &e) {
			std::cerr << "[DORM] " << e.getErrorCode() << ": " << e.what() << std::endl;
			std::cerr << "[DORM] " << sql << std::endl;
			throw(e);
		}
	}


	int DB::deleterow(const std::string &table, const Where &where_clause) {
		std::string sql = "DELETE FROM " + table + " WHERE " + where_clause.to_string();

		try {
			std::unique_ptr<sql::PreparedStatement> pstmt( conn->prepareStatement(sql) );

			unsigned int bind_offset = 1;

			where_clause.bind( *pstmt, bind_offset );

			return pstmt->executeUpdate();
		} catch (sql::SQLException &e) {
			std::cerr << "[DORM] " << e.getErrorCode() << ": " << e.what() << std::endl;
			std::cerr << "[DORM] " << sql << std::endl;
			throw(e);
		}
	}


	int DB::execute(const std::string &sql) {
		try {
			std::unique_ptr<sql::Statement> stmt( conn->createStatement() );

			return stmt->execute(sql);
		} catch (sql::SQLException &e) {
			std::cerr << "[DORM] " << e.getErrorCode() << ": " << e.what() << std::endl;
			std::cerr << "[DORM] " << sql << std::endl;
			throw(e);
		}
	}


	std::string DB::from_unixtime( const time_t t ) {
		char buf[80];
		struct tm tm;

		strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime_r( &t, &tm ) );

		return std::string( buf );
	}


	std::string DB::from_unixtime( const struct timeval &tv ) {
		char buf[80];
		struct tm tm;

		strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime_r( &tv.tv_sec, &tm ) );
		sprintf( buf + strlen(buf), ".%06ld", tv.tv_usec );

		return std::string( buf );
	}


	time_t DB::unix_timestamp( const std::string &ts ) {
		if (ts == "0000-00-00 00:00:00")
			return (time_t) 0;

		struct tm tm;

		tm.tm_year = strtoul( ts.substr(0, 4).c_str(), NULL, 10 ) - 1900;
		tm.tm_mon = strtoul( ts.substr(5, 2).c_str(), NULL, 10) - 1;
		tm.tm_mday = strtoul( ts.substr(8, 2).c_str(), NULL, 10);
		tm.tm_hour = strtoul( ts.substr(11, 2).c_str(), NULL, 10);
		tm.tm_min = strtoul( ts.substr(14, 2).c_str(), NULL, 10);
		tm.tm_sec = strtoul( ts.substr(17, 2).c_str(), NULL, 10);

		// get mktime() to work out if DST is in effect
		tm.tm_isdst = -1;

		return mktime( &tm );
	}


	struct timeval DB::unix_timeval( const std::string &ts ) {
		struct timeval tv;

		tv.tv_sec = unix_timestamp(ts);
		tv.tv_usec = 0;

		// convert microseconds ?
		if ( ts.size() > 19 )
			tv.tv_usec = strtoul( ts.substr(20, 6).c_str(), NULL, 10);

		return tv;
	}


	template<>
	void DB::bind<int>(sql::PreparedStatement &pstmt, unsigned int &bind_offset, int value) {
		pstmt.setInt(bind_offset++, value);
	}


	template<>
	void DB::bind<std::string>(sql::PreparedStatement &pstmt, unsigned int &bind_offset, std::string value) {
		pstmt.setString(bind_offset++, value);
	}

}
