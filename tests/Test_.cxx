// GENERATED - DO NOT EDIT!

#include "Test_.hxx"
#include "Test.hpp"

#include <memory>

const std::vector< std::function< SPC<DORM::Where>(const Test_ &obj) > > Test_::column_eq_function = {
		[](const Test_ &obj){ return DORM::sqlEq<uint64_t>("testID", obj.testID()).make_shared(); },
		[](const Test_ &obj){ return DORM::sqlEq<std::string>("name", obj.name()).make_shared(); },
		[](const Test_ &obj){ return DORM::sqlEq<unsigned int>("age", obj.age()).make_shared(); }
};


const std::vector<DORM::Object::Info> Test_::column_info = {
	{ "testID", 1, UInt64, true, true, false },
	{ "name", 2, String, false, true, false },
	{ "age", 3, UInt, false, true, false }
};


const DORM::Object::column_index_by_name_t Test_::column_index_by_name = {
	{ "name", 2 },
	{ "age", 3 }
};


std::unique_ptr<Test> Test_::make_from_resultset( std::unique_ptr<DORM::Resultset> &resultset ) {
	auto obj = std::make_unique<Test>();

	obj->_testID = resultset->getUInt64(1);
	obj->_name = resultset->getString(2);
	obj->_age = resultset->getUInt(3);

	return obj;
}


std::unique_ptr<Test> Test_::result() {
	if ( resultset && resultset->next() )
		return make_from_resultset(resultset);

	resultset.reset();

	return std::unique_ptr<Test>();
}
