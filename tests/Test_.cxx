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


const std::vector< std::function< void(const DORM::Resultset &result, Test_ &obj) > > Test_::column_resultset_function = {
		[](const DORM::Resultset &result, Test_ &obj){ obj.testID( result.getUInt64(1) ); },
		[](const DORM::Resultset &result, Test_ &obj){ obj.name( result.getString(2) ); },
		[](const DORM::Resultset &result, Test_ &obj){ obj.age( result.getUInt(3) ); },
};


void Test_::clear() {
	// explictly [re]set to "empty" values
	_testID = 0;
	_name.clear();
	_age = 0;
}


std::unique_ptr<DORM::Object> Test_::make_unique() {
	return std::make_unique<Test>();
}


void Test_::set_from_resultset( const DORM::Resultset &result ) {
	const int n_columns = column_info.size();

	int i;

	try {
		for(i=0; i<n_columns; ++i)
			// check for NULLs first
			if ( result.isNull(i+1) )
				undefined_column_state(i+1);
			else
				( column_resultset_function[i] )( result, *this );
	} catch (sql::SQLException &e) {
		std::cerr << "[DORM] " << e.getErrorCode() << ": " << e.what() << std::endl;
		std::cerr << "[DORM] " << get_table_name() << " column index " << i+1 << std::endl;
		throw(e);
	}
}


std::unique_ptr<Test> Test_::load() {
	DORM::Object::search();
	return result();
}


std::unique_ptr<Test> Test_::load(uint64_t key_testID) {
	Test obj;
	obj.testID( key_testID );
	return obj.load();
}


std::unique_ptr<Test> Test_::load(const DORM::Object &obj) {
	// XXX TO DO
}


std::unique_ptr<Test> Test_::result() {
	if ( resultset && resultset->next() ) {
		auto obj = std::make_unique<Test>();
		obj->set_from_resultset( *resultset );
		return obj;
	}

	resultset.reset();

	return std::unique_ptr<Test>();
}
