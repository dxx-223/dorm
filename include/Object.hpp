#ifndef DORM__INCLUDE__OBJECT_HPP
#define DORM__INCLUDE__OBJECT_HPP

#include "SPC.hpp"
#include "Resultset.hpp"

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <initializer_list>


namespace DORM {

	class Where;
	class Query;

	class Object {
		protected:
			enum CPPCONN_TYPE { Int, UInt, Int64, UInt64, String, Double };

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

			std::vector<State>			column_states;

			std::unique_ptr<Resultset>	resultset;

			virtual const column_index_by_name_t &get_column_index_by_name() const =0;
			virtual const std::vector<Info> &get_column_info() const =0;

			virtual SPC<Where> column_eq(int index) const =0;

			virtual const std::string get_table_name() const =0;
			virtual const int get_autoinc_index() const { return 0; };
			virtual void set_autoinc(const uint64_t &value) =0;

			virtual void search_prep_columns(Query &query) const;
			// virtual void search_prep_join(Query &query, ...?);

			virtual std::unique_ptr<Object> make_unique() =0;	// actually returns std::unique_ptr<derived Object>
			virtual void set_from_resultset(const Resultset &result) =0;

			inline void defined_column_state(unsigned int index) { auto &state = column_states[index - 1]; state.exists = true; state.defined = true; state.changed = true; };
			inline void undefined_column_state(unsigned int index) { auto &state = column_states[index - 1]; state.exists = true; state.defined = false; state.changed = true; };
			inline void deleted_column_state(unsigned int index) { auto &state = column_states[index - 1]; state.exists = false; state.defined = false; state.changed = true; };

			void copy_columns( const Object &other_obj );

		public:
			enum LOCK_MODE { SHARE, EXCLUSIVE };

			Object() {};
			Object(const Object &) =delete;		// really likely to be a bad idea
			Object(Object &&) =delete;			// might be safe to allow move-constructor

			virtual void clear() =0;

			// virtual unique_ptr<derived Object> load() <-- used to be present()
			// static unique_ptr<derived Object> load( { key values } )
			// static unique_ptr<derived Object> load( const Where & )		<-- never used?
			// static unique_ptr<derived Object> load( const Object & )	<-- need all our key columns from other Object

			virtual void save();

			virtual uint64_t search( std::initializer_list< std::reference_wrapper<const Object> > objs );
			virtual uint64_t search() { std::initializer_list< std::reference_wrapper<const Object> > objs = {}; return search(objs); };

			virtual void search_prep( Query &query ) const {};

			// virtual unique_ptr<derived Object> result() <-- generated but not listed here due to specific return type

			virtual void refresh();
			virtual bool present();
			virtual uint64_t count();

			virtual bool lock_record( LOCK_MODE mode = EXCLUSIVE );
			virtual uint64_t lock_records( LOCK_MODE mode = EXCLUSIVE );
	};

}

#endif
