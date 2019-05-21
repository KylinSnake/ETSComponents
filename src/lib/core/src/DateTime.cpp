#include <cmath>
#include <DateTime.h>

namespace snake
{
	namespace core
	{
		inline bool localtime(const TimePoint& tp, struct tm& ret)
		{
			auto t = Clock::to_time_t(tp);
			return localtime_r(&t, &ret) != NULL;
		}

		DateTime DateTime::now() noexcept
		{
			return DateTime(Clock::now());
		}

		const DateTime& DateTime::null() noexcept
		{
			static const DateTime d{TimePoint::min()};
			return d;
		}

		std::int32_t DateTime::year() const
		{
			struct tm t;
			if(localtime(tp_, t))
			{
				return 1900 + t.tm_year;
			}
			return -1;
		}

		std::int32_t DateTime::month() const
		{
			struct tm t;
			if(localtime(tp_, t))
			{
				return 1 + t.tm_mon;
			}
			return -1;
		}

		std::int32_t DateTime::day() const
		{
			struct tm t;
			if(localtime(tp_, t))
			{
				return t.tm_mday;
			}
			return -1;
		}

		std::int32_t DateTime::hour() const
		{
			struct tm t;
			if(localtime(tp_, t))
			{
				return t.tm_hour;
			}
			return -1;
		}

		std::int32_t DateTime::minute() const
		{
			struct tm t;
			if(localtime(tp_, t))
			{
				return t.tm_min;
			}
			return -1;
		}

		std::int32_t DateTime::second() const
		{
			struct tm t;
			if(localtime(tp_, t))
			{
				return t.tm_sec;
			}
			return -1;
		}

		std::int32_t DateTime::weekday() const
		{
			struct tm t;
			if(localtime(tp_, t))
			{
				return t.tm_wday;
			}
			return -1;
		}

		std::int32_t DateTime::micro_second() const
		{
			return duration_cast<microseconds>(tp_.time_since_epoch()).count() % 1000000;
		}

		std::int32_t DateTime::milli_second() const
		{
			return duration_cast<milliseconds>(tp_.time_since_epoch()).count() % 1000;
		}

		std::int32_t DateTime::nano_second() const
		{
			return duration_cast<nanoseconds>(tp_.time_since_epoch()).count() % 1000000000;
		}

		DateTime DateTime::time() const
		{
			return DateTime(TimePoint(Duration(tp_.time_since_epoch().count() % Days(1).count())));
		}

		DateTime DateTime::date() const
		{
			return *this - time().tp_.time_since_epoch();
		}

		DateTime DateTime::today() noexcept
		{
			return now().date();
		}

		bool DateTime::parse(const char* fmt, const std::string& s, DateTime& t)
		{
			struct tm tm_t{};
			if(NULL != strptime(s.c_str(), fmt, &tm_t))
			{
				auto p = mktime(&tm_t);
				if(p != -1)
				{
					t  = Clock::from_time_t(p);
					return true;
				}
			}
			return false;
		}

		bool DateTime::parse_from_default(const std::string& str, DateTime& t)
		{
			return parse("%Y%m%dT%H%M%S", str, t);
		}

		bool DateTime::parse_date_from_default(const std::string& str, DateTime& t)
		{
			return parse_from_default(str + "T000000", t);
		}

		bool DateTime::parse_time_from_default(const std::string& str, DateTime& t)
		{
			return parse("%Y%m%dT%H:%M:%S", "18991231T" + str, t);
		}

		size_t DateTime::format(char* s, size_t max, const char* fmt, const DateTime& d, uint32_t digit_lt_one_sec)
		{
			struct tm tm_t;
			if(localtime(d.tp_, tm_t))
			{
				auto ret = strftime(s, max, fmt, &tm_t);
				if (digit_lt_one_sec > 0 && ret + digit_lt_one_sec + 1 < max)
				{
					if(digit_lt_one_sec == 3)
					{
						return ret + std::sprintf(s + ret, ".%03d", d.milli_second());
					}
					else if (digit_lt_one_sec == 6)
					{
						return ret + std::sprintf(s + ret, ".%06d", d.micro_second());
					}
					else if (digit_lt_one_sec == 9)
					{
						return ret + std::sprintf(s + ret, ".%09d", d.nano_second());
					}
					else if (digit_lt_one_sec < 9)
					{
						auto i = d.nano_second() / Nanoseconds(int(std::pow(10, 9 - digit_lt_one_sec))).count();
						std::string f = ".%0" + std::to_string(digit_lt_one_sec) + "d";
						return ret + std::sprintf(s + ret, f.c_str(), i);
					}
				}
				return ret;
			}
			return 0;
		}

		std::string DateTime::format(const std::string& fmt, const DateTime& d, uint32_t digit_lt_one_sec)
		{
			char buffer[64] {0};
			if (format(buffer, 64, fmt.c_str(), d, digit_lt_one_sec) > 0)
			{
				return std::string{buffer};
			}
			return std::string{};
		}

		std::string DateTime::format(const std::string& fmt, uint32_t digit_lt_one_sec) const
		{
			return format(fmt, *this, digit_lt_one_sec);
		}

		std::string DateTime::format_from_default() const
		{
			return format_from_default(*this);
		}

		std::string DateTime::format_from_default(const DateTime& d)
		{
			return format("%Y%m%d-%H:%M:%S", d, 0);
		}

		std::string DateTime::format_fully() const
		{
			return format("%Y%m%d-%H:%M:%S", *this, 9);
		}
	}
}
