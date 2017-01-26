#ifndef DORM__INCLUDE__OBJECT_HPP
#define DORM__INCLUDE__OBJECT_HPP

#include "SPC.hpp"
#include "Resultset.hpp"

#include <string>
#include <map>
#include <vector>
#include <functional>


namespace DORM {

	class Where;
	class Query;

	class Object {
		protected:
			typedef enum { Int, UInt, Int64, UInt64, String, Double } CPPCONN_TYPE;
			typedef std::map<std::string,int> column_index_by_name_t;

			class Info {
				public:
					std::string		name;
					int				index;
					CPPCONN_TYPE	cppconn_type;
					bool			is_key;
					bool			not_null;
					bool			has_default;
			};

			class State {
				public:
					bool			changed;
					bool			exists;
					bool			defined;

					State(): changed(false), exists(false), defined(false) {};
			};

			std::vector<State>		column_states;

			std::unique_ptr<Resultset> resultset;

			virtual const column_index_by_name_t &get_column_index_by_name() const =0;
			virtual const std::vector<Info> &get_column_info() const =0;

			virtual SPC<Where> column_eq(int index) const =0;

			virtual const std::string get_table_name() const =0;
			virtual const int get_autoinc_index() const { return 0; };
			virtual void set_autoinc(const uint64_t &value) =0;

		public:
			Object() {};
			Object(const Object &) =delete;
			Object(Object &&) =delete;

			virtual void save();

			virtual uint64_t search();
			virtual void search_prep( Query &query );
	};

}

#endif
