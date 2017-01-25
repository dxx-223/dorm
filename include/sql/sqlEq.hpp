#ifndef DORM__INCLUDE__SQL__SQLEQ_HPP
#define DORM__INCLUDE__SQL__SQLEQ_HPP

#include "Where.hpp"

namespace DORM {

	class Column;


	template<class CXXTYPE>
	class sqlEq;

	template<>
	class sqlEq<Column>: public Where {
		private:
			std::string		col;
			std::string		column;

		public:
			sqlEq(std::string init_col, std::string init_column): col(init_col), column(init_column) {};

			virtual std::string to_string() const {
				return col + " = " + column;
			};

			virtual void bind(sql::PreparedStatement &pstmt, unsigned int &bind_offset) const {};

			virtual std::shared_ptr<const Where> make_shared() const { return std::make_shared< const sqlEq<Column> >(*this); };
	};


	template<class CXXTYPE>
	class sqlEq: public Where {
		private:
			std::string		col;
			CXXTYPE			value;

		public:
			sqlEq(std::string init_col, CXXTYPE init_value): col(init_col), value(init_value) {};

			virtual std::string to_string() const {
				return col + " = ?";
			};

			virtual void bind(sql::PreparedStatement &pstmt, unsigned int &bind_offset) const {
				DORM::DB::bind<CXXTYPE>(pstmt, bind_offset, value);
			};

			virtual std::shared_ptr<const Where> make_shared() const { return std::make_shared< const sqlEq<CXXTYPE> >(*this); };
	};

}

#endif
