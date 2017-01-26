#include "Object.hpp"
#include "DB.hpp"
#include "Query.hpp"

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
			DORM::DB::writerow( get_table_name(), inserts, updates);
		} catch(const std::exception &e) {
			throw e;
		}

		// grab last_insert_id() if table has AUTO_INCREMENT column
		const int autoinc_index = get_autoinc_index();
		if (autoinc_index) {
			auto &state = column_states[ autoinc_index - 1 ];

			if ( !state.exists || !state.defined ) {
				DORM::Query query;
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


#if 0
		std::vector<IDB::Where *>	inserts;
		std::vector<IDB::Where *>	updates;

		<% foreach my $col (@columns) { %>
			/* keys dealt with later on */
			<% next if grep { $_ eq $col->{name} } @keys %>

			/* add <%=$col->{name}%> to SQL */

			#ifdef MINIMAL_SAVE
				/* add to UPDATEs if column has changed */
				if (_<%=$col->{name}%>_changed) {
			#endif
					if (_<%=$col->{name}%>_exists) {
						if (_<%=$col->{name}%>_defined) {
							<% if ($col->{conn_type} eq 'Timestamp' || $col->{conn_type} eq 'F_Timestamp') { %>
								updates.push_back( new IDB::sqlEq<%=$timestamp_t%>( "<%=$col->{name}%>", IDB::Engine::from_unixtime(_<%=$col->{name}%>) ) );
							<% } else { %>
								updates.push_back( new IDB::sqlEq<%=$col->{conn_type}%>( "<%=$col->{name}%>", _<%=$col->{name}%> ) );
							<% } %>
						} else {
							<% if ( $col->{not_null} ) { %>
							// column is "not null" and value is undefined so there had better be a default otherwise SQL exception thrown
								updates.push_back( new IDB::sqlEqDefault( "<%=$col->{name}%>" ) );
							<% } else { %>
								updates.push_back( new IDB::sqlEqNull( "<%=$col->{name}%>" ) );
							<% } %>
						}
					}
			#ifdef MINIMAL_SAVE
				}
			#endif

			#ifdef MINIMAL_SAVE
				/* add to INSERTs if column has changed OR is a not-null column with no default (to avoid MySQL error 1364) */
				<% if ($col->{not_null} && !$col->{has_default}) { %>
					if (1) {	/* <%=$col->{name}%> is a "NOT NULL" column with no DEFAULT */
				<% } else { %>
					if ( _<%=$col->{name}%>_changed ) {
				<% } %>
			#endif
					if (_<%=$col->{name}%>_exists) {
						if (_<%=$col->{name}%>_defined) {
							<% if ($col->{conn_type} eq 'Timestamp' || $col->{conn_type} eq 'F_Timestamp') { %>
								inserts.push_back( new IDB::sqlEq<%=$timestamp_t%>( "<%=$col->{name}%>", IDB::Engine::from_unixtime(_<%=$col->{name}%>) ) );
							<% } else { %>
								inserts.push_back( new IDB::sqlEq<%=$col->{conn_type}%>( "<%=$col->{name}%>", _<%=$col->{name}%> ) );
							<% } %>
						} else {
							<% if ( $col->{not_null} ) { %>
								// column is "not null" and value is undefined so there had better be a default otherwise SQL exception thrown
								inserts.push_back( new IDB::sqlEqDefault( "<%=$col->{name}%>" ) );
							<% } else { %>
								inserts.push_back( new IDB::sqlEqNull( "<%=$col->{name}%>" ) );
							<% } %>
						}
					}
			#ifdef MINIMAL_SAVE
				}
			#endif
		<% } %>

		/* keys: <%=join(' ', @keys)%> */
		/* this only need to be in the INSERT section */
		<% foreach my $col (@columns) { %>
			<% next unless grep { $_ eq $col->{name} } @keys %>

			if (_<%=$col->{name}%>_exists) {
				if (_<%=$col->{name}%>_defined) {
					/* add <%=$col->{name}%> to updates */
					<% if ($col->{conn_type} eq 'Timestamp' || $col->{conn_type} eq 'F_Timestamp') { %>
						inserts.push_back( new IDB::sqlEq<%=$timestamp_t%>( "<%=$col->{name}%>", IDB::Engine::from_unixtime(_<%=$col->{name}%>) ) );
					<% } else { %>
						inserts.push_back( new IDB::sqlEq<%=$col->{conn_type}%>( "<%=$col->{name}%>", _<%=$col->{name}%> ) );
					<% } %>
				}
			}
		<% } %>

		/* do save */
		try {
			idbe()->writerow("<%=$table%>", inserts, updates);
		} catch (const std::exception &e) {
			/* clean inserts & updates */
			for(auto insert : inserts)
				delete insert;
			for(auto update : updates)
				delete update;

			throw(e);
		}

		/* clean inserts & updates */
		for(auto insert : inserts)
			delete insert;
		for(auto update : updates)
			delete update;

		/* reset changed-ness */
		<% foreach my $col (@columns) { %>
			_<%=$col->{name}%>_changed = false;
		<% } %>

		/* autoinc support */
		<% if ($autoinc) { %>
			if (!_<%=$autoinc%>_exists || !_<%=$autoinc%>_defined) {
				this-><%=$autoinc%>( idbe()->fetchInt("last_insert_id()", IDB_NO_TABLES, IDB_NO_WHERE, IDB_NO_OPTIONS) );
			}
		<% } %>
#endif

	}

}
