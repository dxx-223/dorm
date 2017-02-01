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
	DORM::DB::execute("insert into Tests values (null, 'Fudge', 5), (null, 'Dominic', 43), (null, 'Georgie', 5)");

	Test tests;
	tests.age(5);

	bool is_found = tests.present();

	std::cout << "Row with age=5 present: " << (is_found ? "YES" : "NO") << std::endl;
}
