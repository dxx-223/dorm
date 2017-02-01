#include "DB.hpp"

#include "Test.hpp"
#include "Test/Single.hpp"


char DB_URI[] = "unix:///tmp/mysql.sock";
char DB_USER[] = "test";
char DB_PASSWORD[] = "";
char DB_SCHEMA[] = "test";


int main() {
	DORM::DB::connect( DB_URI, DB_USER, DB_PASSWORD, DB_SCHEMA );

	DORM::DB::execute("drop table if exists Tests");
	DORM::DB::execute("create temporary table Tests ( testID serial, name varchar(255) not null, age int unsigned not null, primary key (testID) )");

	DORM::DB::execute("drop table if exists TestSingles");
	DORM::DB::execute("create temporary table TestSingles ( testID bigint unsigned not null, blah varchar(255) not null, primary key (testID) )");

	Test test;
	test.name("Fudge");
	test.age(5);
	test.save();

	TestSingle single;
	single.testID( test.testID() );
	single.blah("BLAH!");
	single.save();

	Test tests;
	tests.search();

	while( auto test = tests.result() ) {
		std::cout << "testID: " << test->testID() << ", name: " << test->name() << ", age: " << test->age() << std::endl;
		test->foo();

		auto single = test->test_single();
		std::cout << "testID: " << single->testID() << ", blah: " << single->blah() << std::endl;
		single->foo();
	}
}
