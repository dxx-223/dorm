#include "DB.hpp"

#include "Test.hpp"

char DB_URI[] = "unix:///tmp/mysql.sock";
char DB_USER[] = "test";
char DB_PASSWORD[] = "";
char DB_SCHEMA[] = "test";



int main() {
	DORM::DB::connect( DB_URI, DB_USER, DB_PASSWORD, DB_SCHEMA );

	DORM::DB::execute("drop table if exists Tests");
	DORM::DB::execute("create temporary table Tests ( testID serial, name varchar(255) not null, age int unsigned not null, primary key (testID) )");

	Test t;
	t.name("Fudge");
	t.age(5);
	t.save();

	Test tests;
	tests.age(5);

	auto test = tests.load();
	std::cout << "testID: " << test->testID() << ", name: " << test->name() << ", age: " << test->age() << std::endl;
	test->foo();

	test = Test::load(1);
	std::cout << "testID: " << test->testID() << ", name: " << test->name() << ", age: " << test->age() << std::endl;
	test->foo();
}
