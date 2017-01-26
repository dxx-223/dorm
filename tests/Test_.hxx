// GENERATED
#include "Object.hpp"
#include "DB.hpp"
#include "sql/sqlEq.hpp"


class Test_: public DORM::Object {
	private:
		static const std::vector< std::function< SPC<DORM::Where>(const Test_ &obj) > > column_eq_function;

	protected:
		static const column_index_by_name_t column_index_by_name;
		static const std::vector<Info> column_info;


		virtual const column_index_by_name_t &get_column_index_by_name() const {
			return column_index_by_name;
		}


		virtual const std::vector<Info> &get_column_info() const {
			return column_info;
		}


		virtual SPC<DORM::Where> column_eq(int index) const {
			return (column_eq_function[index - 1])(*this);
		}

	private:
		uint64_t _testID;
		std::string _name;
		unsigned int _age;

	public:
		Test_() { column_states.resize(3); };

		virtual const std::string get_table_name() const { return "Tests"; };
		virtual const int get_autoinc_index() const { return 1; };
		virtual void set_autoinc( const uint64_t &value ) { _testID = value; };

		uint64_t testID() const { return _testID; };
		std::string name() const { return _name; };
		unsigned int age() const { return _age; };

		void testID( const uint64_t &value) { _name = value; auto &state = column_states[0]; state.exists = true; state.defined = true; state.changed = true; };
		void name( const std::string &value) { _name = value; auto &state = column_states[1]; state.exists = true; state.defined = true; state.changed = true; };
		void age( const unsigned int &value) { _age = value; auto &state = column_states[2]; state.exists = true; state.defined = true; state.changed = true; };
};


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
