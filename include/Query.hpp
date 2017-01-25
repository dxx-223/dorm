#ifndef DORM__INCLUDE__QUERY__HPP
#define DORM__INCLUDE__QUERY__HPP

#include "Tables.hpp"
#include "Where.hpp"

#include <vector>
#include <string>


namespace DORM {

	class Query {
		public:
			std::vector<std::string>		cols;
			Tables							tables;
			std::shared_ptr<const Where>	where;
			std::shared_ptr<const Where>	having;
			std::string						order_by;
			std::string						group_by;
			uint64_t						limit;
			uint64_t						offset;
			bool							lock_in_share_mode;
			bool							for_update;

			Query(): limit(0), offset(0), lock_in_share_mode(false), for_update(false) {};

			std::shared_ptr<const Query> make_shared() const;

			std::string to_string() const;
			void bind(sql::PreparedStatement &pstmt, unsigned int &bind_offset) const;
	};

}

#endif
