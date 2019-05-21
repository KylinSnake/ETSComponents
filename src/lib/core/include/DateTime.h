#ifndef SNAKE_CORE_DATETIME_H
#define SNAKE_CORE_DATETIME_H

#include <time.h>

#include <chrono>
#include <string>

using Clock = std::chrono::system_clock;
using TimePoint = std::chrono::time_point<Clock>;
using Duration = TimePoint::duration;
using namespace std::chrono;

namespace snake
{
	namespace core
	{
		class DateTime
		{
		public:
			DateTime() = default;
			~DateTime() = default;
			DateTime(const DateTime&) = default;
			DateTime(DateTime&&) = default;
			DateTime& operator=(const DateTime&) = default;
			DateTime& operator=(DateTime&& t) = default;

			DateTime(const TimePoint& t) : tp_(t) {}
			DateTime(TimePoint&& t) : tp_(t) {}

			DateTime(std::int64_t nano) : tp_(nanoseconds(nano)) {}

			DateTime& operator=(const TimePoint& t) { tp_ = t; return *this;}
			DateTime& operator=(TimePoint&& t) { tp_ = t; return *this;}

			bool valid() const { return *this != null(); }
			
			bool operator==(const DateTime& t) const { return tp_ == t.tp_; }
			bool operator!=(const DateTime& t) const { return tp_ != t.tp_; }

			bool operator<(const DateTime& t) const { return tp_ < t.tp_; }
			bool operator>(const DateTime& t) const { return tp_ > t.tp_; }
			bool operator<=(const DateTime &t) const { return tp_ <= t.tp_; }
			bool operator>=(const DateTime &t) const { return tp_ >= t.tp_; }

			explicit operator TimePoint() const { return tp_;}

			explicit operator int64_t() const { return tp_.time_since_epoch().count(); }

			DateTime& operator +=(const Duration& t) { tp_ += t; return *this;}
			DateTime& operator -=(const Duration& t) { tp_ -= t; return *this;}

			DateTime operator +(const Duration& t) const { return DateTime(tp_ + t);}
			DateTime operator -(const Duration& t) const { return DateTime(tp_ - t);}
			
			Duration operator -(const DateTime& t) const { return tp_ - t.tp_;}


			//Following are calendar accessor
			std::int32_t year() const; // 1900-20XX
			std::int32_t month() const; // 01-12
			std::int32_t day() const; // 01-31
			std::int32_t hour() const; // 00-23
			std::int32_t minute() const; // 00-59
			std::int32_t second() const;// 00-59
			std::int32_t weekday() const; // Sun(0) - Sat(6)
			std::int32_t micro_second() const; // 000000 - 999999
			std::int32_t milli_second() const; // 000 - 999
			std::int32_t nano_second() const; // 000000000 - 999999999
			bool is_weekend() { auto d = weekday(); return d == 0 || d == 6; }

			DateTime date() const;
			DateTime time() const;

			static DateTime now() noexcept;
			static DateTime today() noexcept;
			static const DateTime& null() noexcept;

			static bool parse(const char* fmt, const std::string& str, DateTime& d);
			static bool parse_from_default(const std::string& str, DateTime& d);
			static bool parse_date_from_default(const std::string& str, DateTime& d);
			static bool parse_time_from_default(const std::string& str, DateTime& d);

			static size_t format(char*, size_t, const char* fmt, const DateTime& d, uint32_t digit_lt_one_sec = 0);
			static std::string format(const std::string& fmt, const DateTime& d, uint32_t digit_lt_one_sec = 0);
			static std::string format_from_default(const DateTime& d);

			std::string format(const std::string& fmt, uint32_t digit_lt_one_sec) const;
			std::string format_from_default() const;
			std::string format_fully() const;
		private:
			TimePoint tp_ {null().tp_};
		};

		inline Duration Days(int i)
		{
			return duration_cast<Duration>(hours(24 * i));
		}

		inline Duration Hours(int i)
		{
			return duration_cast<Duration>(hours(i));
		}

		inline Duration Minutes(int i)
		{
			return duration_cast<Duration>(minutes(i));
		}

		inline Duration Seconds(int i)
		{
			return duration_cast<Duration>(seconds(i));
		}

		inline Duration Milliseconds(int i)
		{
			return duration_cast<Duration>(milliseconds(i));
		}

		inline Duration Microseconds(int i)
		{
			return duration_cast<Duration>(microseconds(i));
		}

		inline Duration Nanoseconds(int i)
		{
			return duration_cast<Duration>(nanoseconds(i));
		}

		template<typename OStreamT>
		OStreamT& operator<<(OStreamT& os, const DateTime& d)
		{
			os << d.format_from_default();
			return os;
		}
	}
}

#endif
