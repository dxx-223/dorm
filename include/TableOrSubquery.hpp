#ifndef DORM__INCLUDE__TABLEORSUBQUERY__HPP
#define DORM__INCLUDE__TABLEORSUBQUERY__HPP

#include <cppconn/prepared_statement.h>
#include <string>


namespace DORM {

	class Query;


	class TableOrSubquery {
		public:
			// virtual const TableOrSubquery *clone() const { throw std::runtime_error("Can't clone abstract TableOrSubquery class"); };
			virtual const TableOrSubquery *clone() const =0;

			virtual std::string to_string() const =0;
			virtual void bind(sql::PreparedStatement &pstmt, unsigned int &bind_offset) const =0;
	};
	

	class Table: public TableOrSubquery {
		private:
			std::string						table_name;
	
		public:
			Table(std::string table): table_name(table) {};

			virtual const Table *clone() const { return new Table(*this); };

			virtual std::string to_string() const { return table_name; };
			virtual void bind(sql::PreparedStatement &pstmt, unsigned int &bind_offset) const {};
	};
	

	class Subquery: public TableOrSubquery {
		private:
			std::shared_ptr<const Query>	subquery;
			std::string						subquery_alias;
			
		public:
			// NOTE: We're using a shared_ptr to a copy of the passed Query to remove header includes
			Subquery( const Query *query, std::string alias );

			virtual const Subquery *clone() const { return new Subquery(*this); };

			virtual std::string to_string() const;
			virtual void bind(sql::PreparedStatement &pstmt, unsigned int &bind_offset) const;
	};
}

#endif
