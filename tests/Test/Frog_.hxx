// GENERATED - DO NOT EDIT!

#ifndef TEST__FROG_HXX
#define TEST__FROG_HXX

#include "DB.hpp"
#include "Object.hpp"
#include "SearchMod.hpp"
#include "Query.hpp"

#include "sql/sqlEq.hpp"

class TestFrog;


class TestFrog_: public DORM::Object {
	private:
		static const std::vector< std::function< SPC<DORM::Where>(const TestFrog_ &obj) > > column_eq_function;
		static const column_index_by_name_t column_index_by_name;
		static const std::vector<Info> column_info;
		static const std::vector< std::function< void(const DORM::Resultset &result, TestFrog_ &obj) > > column_resultset_function;

		uint64_t _testID;
		std::string _frog_flavour;

		virtual const column_index_by_name_t &get_column_index_by_name() const { return column_index_by_name; };
		virtual const std::vector<Info> &get_column_info() const { return column_info; };
		virtual SPC<DORM::Where> column_eq(int index) const { return (column_eq_function[index - 1])(*this); };

		virtual const std::string get_table_name() const { return "TestFrogs"; };

		virtual const int get_autoinc_index() const { return 0; };
		virtual void set_autoinc( const uint64_t &value ) { _testID = value; };

		std::unique_ptr<Object> make_unique();
		void set_from_resultset( const DORM::Resultset &result );

	public:
		virtual void clear();

		TestFrog_() { column_states.resize(2); clear(); };

		inline uint64_t testID() const { return _testID; };
		inline std::string frog_flavour() const { return _frog_flavour; };

		inline void testID( const uint64_t &value) { _testID = value; defined_column_state(1); };
		inline void frog_flavour( const std::string &value) { _frog_flavour = value; defined_column_state(2); };

		inline void undef_testID() { undefined_column_state(1); };
		inline void undef_frog_flavour() { undefined_column_state(2); };

		inline void delete_testID() { deleted_column_state(1); };
		inline void delete_frog_flavour() { deleted_column_state(2); };

		virtual std::unique_ptr<TestFrog> load();
		static std::unique_ptr<TestFrog> load(uint64_t key_testID);
		static std::unique_ptr<TestFrog> load(const Object &obj);

		virtual std::unique_ptr<TestFrog> result();
};

#endif
