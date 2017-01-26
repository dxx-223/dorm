#ifndef DORM__INCLUDE__TIMESTAMP_HPP
#define DORM__INCLUDE__TIMESTAMP_HPP

#include <ctime>
// #include <sys/time.h>


namespace DORM {

	class Timestamp {
		public:
			struct timeval	tv;

			Timestamp(): tv{0, 0} {};
			Timestamp(const time_t &t): tv{t, 0} {};
			Timestamp(const struct timeval &t): tv{t} {};
	};

}

#endif
