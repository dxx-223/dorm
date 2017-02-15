#include "DB.hpp"
#include "Query.hpp"

#include <mysql/mysql.h>
#include <cppconn/driver.h>

#include <thread>
#include <sstream>
#include <unistd.h>


char DB_URI[] = "unix:///tmp/mysql.sock";
char DB_USER[] = "root";
char DB_PASSWORD[] = "";
char DB_SCHEMA[] = "test";


void processlist( std::string prefix = "") {
	DORM::Query query;
	query.cols.push_back( "count(*) FROM information_schema.PROCESSLIST" );
	
	std::stringstream ss;
	ss << prefix << "Connections: " << DORM::DB::fetch_int64(query) << std::endl;
	std::cout << ss.str();
}


int main() {
	DORM::DB::connect( DB_URI, DB_USER, DB_PASSWORD, DB_SCHEMA );
	
	DORM::DB::execute("SET @@GLOBAL.wait_timeout=1");

	processlist();
	
	sleep(5);
	
	DORM::DB::check_connection();

	processlist();

	sleep(5);

	processlist();		// will fail

	exit(0);
}
