// GENERATED - DO NOT EDIT!

#ifndef TEST___HXX
#define TEST___HXX

#include "DB.hpp"
#include "Object.hpp"
#include "SearchMod.hpp"
#include "Query.hpp"

#include "sql/sqlEq.hpp"

class Test;


class Test_: public DORM::Object {
	private:
		static const std::vector< std::function< SPC<DORM::Where>(const Test_ &obj) > > column_eq_function;
		static const column_index_by_name_t column_index_by_name;
		static const std::vector<Info> column_info;

		uint64_t _testID;
		std::string _name;
		unsigned int _age;

		virtual const column_index_by_name_t &get_column_index_by_name() const { return column_index_by_name; };
		virtual const std::vector<Info> &get_column_info() const { return column_info; };
		virtual SPC<DORM::Where> column_eq(int index) const { return (column_eq_function[index - 1])(*this); };

		virtual const std::string get_table_name() const { return "Tests"; };

		virtual const int get_autoinc_index() const { return 1; };
		virtual void set_autoinc( const uint64_t &value ) { _testID = value; };

		std::unique_ptr<Test> make_from_resultset( std::unique_ptr<DORM::Resultset> &resultset );

	public:
		Test_() { column_states.resize(3); };

		uint64_t testID() const { return _testID; };
		std::string name() const { return _name; };
		unsigned int age() const { return _age; };

		void testID( const uint64_t &value) { _name = value; auto &state = column_states[0]; state.exists = true; state.defined = true; state.changed = true; };
		void name( const std::string &value) { _name = value; auto &state = column_states[1]; state.exists = true; state.defined = true; state.changed = true; };
		void age( const unsigned int &value) { _age = value; auto &state = column_states[2]; state.exists = true; state.defined = true; state.changed = true; };

		void undef_testID() { auto &state = column_states[0]; state.exists = true; state.defined = false; state.changed = true; };
		void undef_name() { auto &state = column_states[1]; state.exists = true; state.defined = false; state.changed = true; };
		void undef_age() { auto &state = column_states[2]; state.exists = true; state.defined = false; state.changed = true; };

		void delete_testID() { auto &state = column_states[0]; state.exists = false; state.defined = false; state.changed = true; };
		void delete_name() { auto &state = column_states[1]; state.exists = false; state.defined = false; state.changed = true; };
		void delete_age() { auto &state = column_states[2]; state.exists = false; state.defined = false; state.changed = true; };

		std::unique_ptr<Test> result();
};

#endif
