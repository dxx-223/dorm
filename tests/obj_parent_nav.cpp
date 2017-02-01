#include "DB.hpp"

#include "Test.hpp"
#include "Test/Frog.hpp"


char DB_URI[] = "unix:///tmp/mysql.sock";
char DB_USER[] = "test";
char DB_PASSWORD[] = "";
char DB_SCHEMA[] = "test";


int main() {
	DORM::DB::connect( DB_URI, DB_USER, DB_PASSWORD, DB_SCHEMA );

	DORM::DB::execute("drop table if exists Tests");
	DORM::DB::execute("create temporary table Tests ( testID serial, name varchar(255) not null, age int unsigned not null, primary key (testID) )");

	DORM::DB::execute("drop table if exists TestFrogs");
	DORM::DB::execute("create temporary table TestFrogs ( testID bigint unsigned not null, frog_flavour varchar(255) not null, primary key (testID) )");

	Test test;
	test.name("Fudge");
	test.age(5);
	test.save();

	TestFrog frog;
	frog.testID( test.testID() );
	frog.frog_flavour("chocolate");
	frog.save();

	Test tests;
	tests.search();

	while( auto test = tests.result() ) {
		std::cout << "testID: " << test->testID() << ", name: " << test->name() << ", age: " << test->age() << std::endl;
		test->foo();
	}

	TestFrog frogs;
	frogs.search();

	while( auto frog = frogs.result() ) {
		std::cout << "testID: " << frog->testID() << ", flavour: " << frog->frog_flavour() << std::endl;
		frog->foo();

		auto test = frog->test();
		std::cout << "testID: " << test->testID() << ", name: " << test->name() << ", age: " << test->age() << std::endl;
		test->foo();
	}
}
