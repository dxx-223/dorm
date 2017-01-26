#include "DB.hpp"
#include "Query.hpp"
#include "Resultset.hpp"

#include "sql/sqlEq.hpp"
#include "sql/sqlAnd.hpp"


char DB_URI[] = "unix:///tmp/mysql.sock";
char DB_USER[] = "test";
char DB_PASSWORD[] = "";
char DB_SCHEMA[] = "test";


void test_query(DORM::Query query) {
	std::unique_ptr<DORM::Resultset> results( DORM::DB::select(query) );

	if (results)
		while( results->next() )
			std::cout << "Name: " << results->getString(1) << ", age: " << results->getInt(2) << std::endl;
}


void test() {
	DORM::DB::connect( DB_URI, DB_USER, DB_PASSWORD, DB_SCHEMA );

	DORM::DB::execute("drop table if exists DORM_test");
	DORM::DB::execute("create temporary table DORM_test ( name varchar(255), age int )");
	DORM::DB::execute("insert into DORM_test values ('Dom', 43),('Melody',600000000)");

	DORM::sqlEq<int> age_eq("age", 5);
	DORM::sqlEq<std::string> name_eq("name", "Fudge");

	std::vector< SPC<DORM::Where> > inserts{ age_eq.make_shared(), name_eq.make_shared() };

	DORM::DB::writerow("DORM_test", inserts);
	
	DORM::Query query;
	
	query.cols.push_back("*");
	
	DORM::Tables test_tables("DORM_test");
	query.tables = test_tables;

	test_query(query);

	DORM::DB::deleterow("DORM_test", age_eq);

	test_query(query);
}


int main() {
	test();

	exit(0);
}
