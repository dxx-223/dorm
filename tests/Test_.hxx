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
		static const std::vector< std::function< void(const DORM::Resultset &result, Test_ &obj) > > column_resultset_function;

		uint64_t _testID;
		std::string _name;
		unsigned int _age;

		virtual const column_index_by_name_t &get_column_index_by_name() const { return column_index_by_name; };
		virtual const std::vector<Info> &get_column_info() const { return column_info; };
		virtual SPC<DORM::Where> column_eq(int index) const { return (column_eq_function[index - 1])(*this); };

		virtual const std::string get_table_name() const { return "Tests"; };

		virtual const int get_autoinc_index() const { return 1; };
		virtual void set_autoinc( const uint64_t &value ) { _testID = value; };

		std::unique_ptr<Object> make_unique();
		void set_from_resultset( const DORM::Resultset &result );

	public:
		virtual void clear();

		Test_() { column_states.resize(3); clear(); };

		inline uint64_t testID() const { return _testID; };
		inline std::string name() const { return _name; };
		inline unsigned int age() const { return _age; };

		inline void testID( const uint64_t &value) { _testID = value; defined_column_state(1); };
		inline void name( const std::string &value) { _name = value; defined_column_state(2); };
		inline void age( const unsigned int &value) { _age = value; defined_column_state(3); };

		inline void undef_testID() { undefined_column_state(1); };
		inline void undef_name() { undefined_column_state(2); };
		inline void undef_age() { undefined_column_state(3); };

		inline void delete_testID() { deleted_column_state(1); };
		inline void delete_name() { deleted_column_state(2); };
		inline void delete_age() { deleted_column_state(3); };

		virtual std::unique_ptr<Test> load();
		static std::unique_ptr<Test> load(uint64_t key_testID);
		static std::unique_ptr<Test> load(const Object &obj);

		virtual std::unique_ptr<Test> result();
};

#endif
