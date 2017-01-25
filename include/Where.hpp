#ifndef DORM__INCLUDE__WHERE__HPP
#define DORM__INCLUDE__WHERE__HPP

#include <cppconn/prepared_statement.h>
#include <string>


namespace DORM {

	class Where {
		public:
			virtual std::shared_ptr<const Where> make_shared() const =0;

			virtual std::string to_string() const =0;
			virtual void bind(sql::PreparedStatement &pstmt, unsigned int &bind_offset) const =0;
	};

}

#endif
