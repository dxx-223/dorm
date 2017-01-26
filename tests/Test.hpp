#ifndef TESTOBJECT_HPP
#define TESTOBJECT_HPP

/*
	create table Tests (
		testID				serial,
		name				varchar(255) not null,
		age					int unsigned not null,
		primary key			(testID)
	);
*/

#include "Test_.hxx"


class Test: public Test_ {
};


#endif
