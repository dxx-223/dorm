#include "Object.hpp"
#include "DB.hpp"
#include "Query.hpp"
#include "Resultset.hpp"

#include "sql/sqlEq.hpp"
#include "sql/sqlAnd.hpp"
#include "sql/sqlIsNull.hpp"

#include <iostream>


namespace DORM {

#if 0
	Object::Object(const Object &other_obj) {
		auto other_column_info = other_obj.get_column_info();
		const auto &other_column_states = other_obj.column_states;

		const int n_other_columns = other_column_info.size();
		for(int i=0; i<n_other_columns; ++i) {
			const auto &other_info = other_column_info[i];

			if ( !other_info.is_key )
				continue;

			const auto &other_state = other_column_states[ other_info.index - 1 ];

			if ( other_state.exists && other_state.defined ) {
				// copy column from other to same-named column in us
			}
		}


		<%=$class%>::<%=$class%>(const IO::Object *other_obj): IO::Object() {
			this->_init();

			<% foreach my $col (@columns) { %>
				if ( other_obj->is_key_by_name("<%=$col->{name}%>") )
					if ( other_obj->exists_by_name("<%=$col->{name}%>") )
						if ( other_obj->defined_by_name("<%=$col->{name}%>") )
							this-><%=$col->{name}%>( other_obj->get_<%=$col->{conn_type}%>_by_name("<%=$col->{name}%>") );
						else
							this->remove_<%=$col->{name}%>();
					else
						this->remove_<%=$col->{name}%>();
				else
					this->remove_<%=$col->{name}%>();
			<% } %>
		};

	}
#endif


	void Object::save() {
		#if 0
			const column_index_by_name_t &column_index_by_name = get_column_index_by_name();

			for(const auto &column_info : column_index_by_name) {
				std::cout << "column " << column_info.first << " has index " << column_info.second << std::endl;
			}
		#endif

		std::vector< SPC<Where> > updates;
		std::vector< SPC<Where> > inserts;

		auto column_info = get_column_info();

		/*
		 *	foreach non-key column:
		 * 		if column exists and has changed (MINIMAL_SAVE)
		 * 			if column defined
		 * 				add column to UPDATEs using specific sqlEq<> (take care with timestamps?)
		 * 			else
		 * 				if column declared as "not null"
		 * 					add column to UPDATEs using sqlEq<Default> (to set it to DEFAULT)
		 * 				else
		 * 					add column to UPDATEs using sqlEq<Null> (to set it to NULL)
		 *
		 * 		also do the same with INSERT if column has changed OR is declared NOT NULL with no DEFAULT (to avoid MySQL error 1364)
		 */
		for(const auto &info : column_info) {
			if ( info.is_key )
				continue;

			std::cout << "non-key column: " << info.name << std::endl;

			const auto &state = column_states[ info.index - 1 ];

			if ( state.exists && state.changed ) {
				if ( state.defined )
					updates.push_back( column_eq(info.index) );
				else if ( info.not_null )
					updates.push_back( sqlEq<Default>(info.name).make_shared() );
				else
					updates.push_back( sqlEq<Null>(info.name).make_shared() );
			}

			if ( (info.not_null && !info.has_default) || state.changed ) {
				if ( state.defined )
					inserts.push_back( column_eq(info.index) );
				else if ( info.not_null )
					inserts.push_back( sqlEq<Default>(info.name).make_shared() );
				else
					inserts.push_back( sqlEq<Null>(info.name).make_shared() );
			}
		}

		// add all key columns to INSERTs, regardless of whether they've changed
		for(const auto &info : column_info) {
			if ( !info.is_key )
				continue;

			std::cout << "key column: " << info.name << std::endl;

			const auto &state = column_states[ info.index - 1 ];

			if ( state.exists && state.defined )
				inserts.push_back( column_eq(info.index) );
		}

		// writerow in try..catch block
		try {
			DB::writerow( get_table_name(), inserts, updates);
		} catch(const std::exception &e) {
			throw e;
		}

		// grab last_insert_id() if table has AUTO_INCREMENT column
		const int autoinc_index = get_autoinc_index();
		if (autoinc_index) {
			auto &state = column_states[ autoinc_index - 1 ];

			if ( !state.exists || !state.defined ) {
				Query query;
				query.cols.push_back("last_insert_id()");

				const uint64_t autoinc_value = DB::fetch_uint64(query);		// what happens if this throws but the previous writerow() did not?

				set_autoinc(autoinc_value);
				state.exists = true;
				state.defined = true;
			}
		}

		// reset column changed flags
		for(auto &state : column_states)
			state.changed = false;
	}


	void Object::search_prep_columns(Query &query) {
		auto column_info = get_column_info();

		std::vector< SPC<Where> > where_clauses;

		for(const auto &info : column_info) {
			const auto &state = column_states[ info.index - 1 ];

			if ( state.exists ) {
				if ( state.defined )
					where_clauses.push_back( column_eq(info.index) );
				else
					where_clauses.push_back( sqlIsNull(info.name).make_shared() );
			}
		}

		if ( !where_clauses.empty() )
			query.where = sqlAnd(where_clauses).make_shared();
	}


#if 0
	uint64_t Object::search() {
		Query query;
		query.cols.push_back("*");
		query.tables = Tables( get_table_name() );

		// convert columns
		search_prep_columns(query);

		search_prep(query);

		// mySQL-only
		query.cols[0] = "SQL_CALC_FOUND_ROWS " + query.cols[0];
		resultset.reset( DB::select(query) );

		Query found_rows_query;
		found_rows_query.cols.push_back("found_rows()");
		const uint64_t found_rows = DB::fetch_uint64(found_rows_query);

		return found_rows;
	}
#endif

	uint64_t Object::search( std::initializer_list<Object> objs ) {
		Query query;
		query.cols.push_back("*");
		query.tables = Tables( get_table_name() );

		// convert columns
		search_prep_columns(query);

		// XXX JOIN ADDITIONAL OBJECTS HERE

		search_prep(query);

		// mySQL-only
		query.cols[0] = "SQL_CALC_FOUND_ROWS " + query.cols[0];
		resultset.reset( DB::select(query) );

		Query found_rows_query;
		found_rows_query.cols.push_back("found_rows()");
		const uint64_t found_rows = DB::fetch_uint64(found_rows_query);

		return found_rows;
	}


	void Object::refresh() {
		// we only need keys to reload the object
		Query query;
		query.cols.push_back("*");
		query.tables = Tables( get_table_name() );
		query.limit = 1;

		auto column_info = get_column_info();

		std::vector< SPC<Where> > where_clauses;

		for(const auto &info : column_info) {
			if ( !info.is_key )
				continue;

			const auto &state = column_states[ info.index - 1 ];

			if ( state.exists &&  state.defined )
				where_clauses.push_back( column_eq(info.index) );
		}

		if ( !where_clauses.empty() )
			query.where = sqlAnd(where_clauses).make_shared();

		std::unique_ptr<Resultset> results( DB::select(query) );

		set_from_resultset( *results );

		// reset state flags
		for(auto &state : column_states) {
			state.changed = false;
			state.exists = true;
		}
	}


	bool Object::present() {
		Query query;
		query.cols.push_back("*");
		query.tables = Tables( get_table_name() );

		// convert columns
		search_prep_columns(query);

		search_prep(query);

		query.cols.assign( {"true"} );
		query.limit = 1;

		std::unique_ptr<Resultset> results( DB::select(query) );

		if ( results && results->next() )
			return results->getBoolean(1);

		return false;
	}


	uint64_t Object::count() {
		Query query;
		query.cols.push_back("*");
		query.tables = Tables( get_table_name() );

		// convert columns
		search_prep_columns(query);

		search_prep(query);

		query.limit = 1;

		/* we do a full search then use SQL_CALC_FOUND_ROWS ? */
		/* why not use count(*) ? */

		// mySQL-only
		query.cols[0] = "SQL_CALC_FOUND_ROWS " + query.cols[0];
		std::unique_ptr<Resultset> results( DB::select(query) );

		Query found_rows_query;
		found_rows_query.cols.push_back("found_rows()");
		const uint64_t found_rows = DB::fetch_uint64(found_rows_query);

		return found_rows;
	}


	bool Object::lock_record( LOCK_MODE lock_mode ) {
		// SELECT true FROM <table> WHERE <defined keys> FOR UPDATE
		Query query;
		query.cols.push_back("SQL_CALC_FOUND_ROWS true");
		query.tables = Tables( get_table_name() );
		query.limit = 1;

		if (lock_mode == EXCLUSIVE)
			query.for_update = true;
		else
			query.lock_in_share_mode = true;

		auto column_info = get_column_info();

		std::vector< SPC<Where> > where_clauses;

		for(const auto &info : column_info) {
			if ( !info.is_key )
				continue;

			const auto &state = column_states[ info.index - 1 ];

			if ( state.exists &&  state.defined )
				where_clauses.push_back( column_eq(info.index) );
		}

		if ( !where_clauses.empty() )
			query.where = sqlAnd(where_clauses).make_shared();

		std::unique_ptr<Resultset> results( DB::select(query) );

		Query found_rows_query;
		found_rows_query.cols.push_back("found_rows()");
		const uint64_t found_rows = DB::fetch_uint64(found_rows_query);

		return found_rows == 1;
	}


	uint64_t Object::lock_records( LOCK_MODE lock_mode ) {
		// SELECT true FROM <tables> WHERE <...> FOR UPDATE
		Query query;
		query.cols.push_back("*");
		query.tables = Tables( get_table_name() );

		// convert columns
		search_prep_columns(query);

		search_prep(query);

		// rewrite slightly
		query.cols.assign( {"SQL_CALC_FOUND_ROWS true"} );

		if (lock_mode == EXCLUSIVE)
			query.for_update = true;
		else
			query.lock_in_share_mode = true;

		std::unique_ptr<Resultset> results( DB::select(query) );

		Query found_rows_query;
		found_rows_query.cols.push_back("found_rows()");
		const uint64_t found_rows = DB::fetch_uint64(found_rows_query);

		return found_rows;
	}

}
