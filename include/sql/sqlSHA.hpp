#ifndef DORM__INCLUDE__SQL__SQLSHA_HPP
#define DORM__INCLUDE__SQL__SQLSHA_HPP

#include "Where.hpp"

namespace DORM {

	class sqlSHA: public Where {
		private:
			std::string					sha_col;
			std::string					sha_salt;
			std::string					sha_salt_col;
			std::string					sha_plaintext;

		public:
			sqlSHA(std::string col, std::string salt, std::string salt_col, std::string plaintext): \
				sha_col(col), sha_salt(salt), sha_salt_col(salt_col), sha_plaintext(plaintext) {};

			virtual std::string toString( std::string prefix = "" ) const {
				std::string output = prefix.empty() ? prefix : prefix + " ";
				output += sha_col + " = SHA2( CONCAT( ?, " + sha_salt_col + ", ? ), 256 )";
				return output;
			}

			virtual void bind(sql::PreparedStatement &pstmt, unsigned int &bind_offset) const {
				pstmt.setString(bind_offset++, sha_salt);
				pstmt.setString(bind_offset++, sha_plaintext);
			}
	};

}

#endif
