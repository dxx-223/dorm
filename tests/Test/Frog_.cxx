// GENERATED - DO NOT EDIT!

#include "Test/Frog_.hxx"
#include "Test/Frog.hpp"

#include <memory>

const std::vector< std::function< SPC<DORM::Where>(const TestFrog_ &obj) > > TestFrog_::column_eq_function = {
		[](const TestFrog_ &obj){ return DORM::sqlEq<uint64_t>("testID", obj.testID()).make_shared(); },
		[](const TestFrog_ &obj){ return DORM::sqlEq<std::string>("frog_flavour", obj.frog_flavour()).make_shared(); }
};


const std::vector<DORM::Object::Info> TestFrog_::column_info = {
	{ "testID", 1, UInt64, true, true, false },
	{ "frog_flavour", 2, String, false, true, false }
};


const DORM::Object::column_index_by_name_t TestFrog_::column_index_by_name = {
	{ "name", 2 },
	{ "age", 3 }
};


const std::vector< std::function< void(const DORM::Resultset &result, TestFrog_ &obj) > > TestFrog_::column_resultset_function = {
		[](const DORM::Resultset &result, TestFrog_ &obj){ obj.testID( result.getUInt64(1) ); },
		[](const DORM::Resultset &result, TestFrog_ &obj){ obj.frog_flavour( result.getString(2) ); }
};


void TestFrog_::clear() {
	// explictly [re]set to "empty" values
	_testID = 0;
	_frog_flavour.clear();
}


std::unique_ptr<DORM::Object> TestFrog_::make_unique() {
	return std::make_unique<TestFrog>();
}


void TestFrog_::set_from_resultset( const DORM::Resultset &result ) {
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


std::unique_ptr<TestFrog> TestFrog_::load() {
	DORM::Object::search();
	return result();
}


std::unique_ptr<TestFrog> TestFrog_::load(uint64_t key_testID) {
	TestFrog obj;
	obj.testID( key_testID );
	return obj.load();
}


std::unique_ptr<TestFrog> TestFrog_::load(const DORM::Object &obj) {
	// XXX TO DO
}


std::unique_ptr<TestFrog> TestFrog_::result() {
	if ( resultset && resultset->next() ) {
		auto obj = std::make_unique<TestFrog>();
		obj->set_from_resultset( *resultset );
		return obj;
	}

	resultset.reset();

	return std::unique_ptr<TestFrog>();
}
