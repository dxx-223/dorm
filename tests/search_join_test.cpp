#include "DB.hpp"
#include "Query.hpp"
#include "Resultset.hpp"

#include "sql/sqlEq.hpp"
#include "sql/sqlAnd.hpp"

#include "Test.hpp"
#include "Test/Frog.hpp"

char DB_URI[] = "unix:///tmp/mysql.sock";
char DB_USER[] = "test";
char DB_PASSWORD[] = "";
char DB_SCHEMA[] = "test";


void test_query(DORM::Query query) {
	std::unique_ptr<DORM::Resultset> results( DORM::DB::select(query) );

	if (results)
		while( results->next() )
			std::cout << "testID: " << results->getUInt64(1) << ", name: " << results->getString(2) << ", age: " << results->getInt(3) << std::endl;
}


int main() {
	DORM::DB::connect( DB_URI, DB_USER, DB_PASSWORD, DB_SCHEMA );

	DORM::DB::execute("drop table if exists Tests");
	DORM::DB::execute("create temporary table Tests ( testID serial, name varchar(255) not null, age int unsigned not null, primary key (testID) )");
	DORM::DB::execute("insert into Tests values (null, 'Fudge', 5), (null, 'Dominic', 43), (null, 'Georgie', 5)");

	DORM::DB::execute("drop table if exists TestFrogs");
	DORM::DB::execute("create temporary table TestFrogs ( testID bigint unsigned not null, frog_flavour varchar(255) not null, primary key (testID) )");
	DORM::DB::execute("insert into TestFrogs values (1,'chocolate'), (2,'vanilla'), (3,'strawberry')");

	Test tests;
	tests.age(5);

	TestFrog frogs;
	frogs.like( "choc" );

	const uint64_t found_rows = tests.search( { frogs } );

	std::cout << "Found rows: " << found_rows << std::endl;

	while( auto test = tests.result() ) {
		std::cout << "testID: " << test->testID() << ", name: " << test->name() << ", age: " << test->age() << std::endl;
		test->foo();
	}
}
