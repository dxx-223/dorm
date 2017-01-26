#include "Object.hpp"
#include "DB.hpp"
#include "Query.hpp"
#include "Resultset.hpp"

#include "sql/sqlEq.hpp"

#include <iostream>


namespace DORM {

	void Object::save() {
		const column_index_by_name_t &column_index_by_name = get_column_index_by_name();

		for(const auto &column_info : column_index_by_name) {
			std::cout << "column " << column_info.first << " has index " << column_info.second << std::endl;
		}

		std::vector< SPC<Where> > updates;
		std::vector< SPC<Where> > inserts;

		auto column_info = get_column_info();

		// foreach non-key column:
			// if column has changed (MINIMAL_SAVE)
				// if column exists
					// if column defined
						// add column sqlEq to UPDATEs (take care with timestamps)
					// else
						// if column declared as "not null"
							// UPDATE column sqlEqDefault (to set it to DEFAULT)
						// else
							// UPDATE column sqlEqNull (to set it to NULL)

			// also do the same with INSERT if column has changed OR is declared NOT NULL with no DEFAULT (to avoid MySQL error 1364)
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


	uint64_t Object::search() {
		Query query;
		query.cols.push_back("*");
		query.tables = Tables( get_table_name() );

		search_prep(query);

		// mySQL-only
		query.cols[0] = "SQL_CALC_FOUND_ROWS " + query.cols[0];
		resultset.reset( DB::select(query) );

		Query found_rows_query;
		found_rows_query.cols.push_back("found_rows()");
		const uint64_t found_rows = DB::fetch_uint64(found_rows_query);

		return found_rows;
	}


	void Object::search_prep(Query &query) {
	}

#if 0
	std::unique_ptr<Object> Object::result() {
		if ( resultset && resultset->next() )
			return make_from_resultset(resultset);

		resultset.reset();

		return std::unique_ptr<Object>();
	}
#endif

}
