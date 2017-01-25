#ifndef DORM__INCLUDE__SQL__SQLAND_HPP
#define DORM__INCLUDE__SQL__SQLAND_HPP

#include "Where.hpp"

#include <vector>


namespace DORM {

	class sqlAnd: public Where {
		private:
			std::vector< std::shared_ptr<const Where> > clauses;

		public:
			sqlAnd(const Where &left, const Where &right) {
				clauses.push_back( std::shared_ptr<const Where>( left.clone() ) );
				clauses.push_back( std::shared_ptr<const Where>( right.clone() ) );
			}

			sqlAnd(int count, ...) {
				va_list args;
				va_start(args, count);

				for(int i=0; i<count; i++) {
					const Where *where = va_arg(args, const Where *);
					clauses.push_back( std::shared_ptr<const Where>( where->clone() ) );
				}

				va_end(args);
			}

			virtual std::string to_string() const {
				std::string output = "( ";

				for(int i=0; i<clauses.size(); i++) {
					if (i > 0)
						output += " AND ";

					output += clauses[i]->to_string();
				}

				output += " )";

				return output;
			}

			virtual void bind(sql::PreparedStatement &pstmt, unsigned int &bind_offset) const {
				for(int i=0; i<clauses.size(); i++)
					clauses[i]->bind(pstmt, bind_offset);
			}

			virtual const sqlAnd *clone() const { return new sqlAnd(*this); };

	};

}

#endif
