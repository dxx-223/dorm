#include "DB.hpp"
#include "Query.hpp"

#include <mysql/mysql.h>
#include <cppconn/driver.h>

#include <thread>


char DB_URI[] = "unix:///tmp/mysql.sock";
char DB_USER[] = "root";
char DB_PASSWORD[] = "";
char DB_SCHEMA[] = "test";


void processlist() {
	DORM::Query query;
	query.cols.push_back( "count(*) FROM information_schema.PROCESSLIST" );
	
	std::cout << "Connections: " << std::to_string( DORM::DB::fetch_int64(query) ) << std::endl;
}


void new_db_thread(int i) {
	std::cout << "new thread [" << i << "]" << std::endl;

	DORM::DB::check_connection();

	processlist();

	std::cout << "exit thread [" << i << "]" << std::endl;
}


int main() {
	std::cout << "main start" << std::endl;

	DORM::DB::connect( DB_URI, DB_USER, DB_PASSWORD, DB_SCHEMA );
	
	processlist();
	
	std::cout << "main connected - creating threads" << std::endl;
	
	std::vector<std::thread> threads;
	
	for(int i=0; i<3; i++) {
		std::thread t( new_db_thread, i );
		threads.push_back( std::move(t) );
	}

	std::cout << "main waiting for threads" << std::endl;

	for(int i=0; i<threads.size(); i++) {
		std::thread &t( threads[i] );
		t.join();
		
		processlist();
	}
	
	std::cout << "main exit" << std::endl;
		
	exit(0);
}