#include "TableOrSubquery.hpp"
#include "Query.hpp"


namespace DORM {

	std::shared_ptr<const TableOrSubquery> Table::make_shared() const {
		return std::make_shared<const Table>(*this);
	}


	Subquery::Subquery( const Query &query, std::string alias ) {
		subquery_alias = alias;
		subquery = query.make_shared();
	}


	std::shared_ptr<const TableOrSubquery> Subquery::make_shared() const {
		return std::make_shared<const Subquery>(*this);
	}


	std::string Subquery::to_string() const {
		if (!subquery)
			throw std::runtime_error("Empty Subquery");

		return "(SELECT " + subquery->to_string() + ") AS " + subquery_alias;
	}


	void Subquery::bind(sql::PreparedStatement &pstmt, unsigned int &bind_offset) const {
		if (!subquery)
			throw std::runtime_error("Empty Subquery");

		subquery->bind(pstmt, bind_offset);
	}

}
