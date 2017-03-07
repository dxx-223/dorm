#include "DB.hpp"
#include "Query.hpp"
#include "Resultset.hpp"

#include "sql/sqlEq.hpp"
#include "sql/sqlAnd.hpp"

#include "Test.hpp"

char DB_URI[] = "unix:///tmp/mysql.sock";
char DB_USER[] = "test";
char DB_PASSWORD[] = "";
char DB_SCHEMA[] = "test";


void test_query(DORM::Query query) {
	std::unique_ptr<DORM::Resultset> results( DORM::DB::select(query) );

	if (results)
		while( results->next() )
			std::cout << "testID: " << results->getUInt64(1) << ", name: " << results->getString(2) << ", age: " << results->getInt(3)
						<< ", simple_time: " << results->getString(4) << ", complex_time: " << results->getString(5) << std::endl;
}


int main() {
	DORM::DB::connect( DB_URI, DB_USER, DB_PASSWORD, DB_SCHEMA );

	DORM::DB::execute("drop table if exists Tests");
	DORM::DB::execute("create temporary table Tests ( testID serial, name varchar(255) not null, age int unsigned not null, simple_time timestamp, complex_time timestamp(6), primary key (testID) )");

	struct timeval tv = { time(nullptr), 123456 };

	Test t;
	t.name("Fudge");
	t.age(5);
	t.simple_time( time(nullptr) );
	t.complex_time( tv );
	t.save();

	DORM::Query query;
	query.cols.push_back("*");
	DORM::Tables test_tables("Tests");
	query.tables = test_tables;

	test_query(query);

	t.name("Sludge");
	t.age(6);
	t.simple_time( time(nullptr) );
	t.complex_time( time(nullptr) );
	t.save();

	std::cout << "t's autoinc column value: " << t.testID() << std::endl;

	test_query(query);
}
