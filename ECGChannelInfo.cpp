#include "ECGChannelInfo.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/format.hpp"

std::string ECGChannelInfo::sampleToTime( int sample )
{
	auto pow = [](int num, int exp) -> int {
		int ret = 1;
		for(int i=0; i<exp; ++i)
			ret *= num;
		return ret;
	};
	long long milis = ((long long) sample * 1000)/frequecy;
	boost::posix_time::time_duration td = boost::posix_time::milliseconds(milis);
	const int FRACTIONAL_DIGITS = 2;
	int div = pow(10, td.num_fractional_digits()-FRACTIONAL_DIGITS);
	return (boost::format("%02d:%02d:%02d.%02d") % td.hours() % td.minutes() % td.seconds() % (td.fractional_seconds()/div)).str();
}
