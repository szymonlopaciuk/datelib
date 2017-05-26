/*! \file */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>

//! Modulo function, works with negative numbers
int _mod(int a, int b)
{
    if (a >= 0 || a % b == 0) return a % b;
    else return a % b + b;
}

//! Integer division, for negative numbers quotient rounded towards infinity
int _div(int a, int b)
{
    if (a >= 0 || a % b == 0) return a / b;
    else return a / b - 1;
}

//! Modulo function, works with negative numbers (long long version)
long long _modl(long long a, long long b)
{
    if (a >= 0 || a % b == 0) return a % b;
    else return a % b + b;
}

//! Integer division, for negative numbers quotient rounded towards infinity (long long version)
long long _divl(long long a, long long b)
{
    if (a >= 0 || a % b == 0) return a / b;
    else return a / b - 1;
}

//! Date/time struct
typedef struct
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int usecond;    //!< Microsecond
    int weekday;    //!< Weekday (0..6, where 0 is Monday)
    int tz_offset;  //!< Time zone offset in minutes to the east
} date_t;

//! Time difference struct
typedef struct
{
    int weeks;
    int days;
    int hours;
    int minutes;
    int seconds;
    int useconds;   //!< Microseconds
} timediff_t;

//! Beginning of the UNIX epoch
const date_t unix_epoch = {1970, 1, 1, 0, 0, 0, 0, 3, 0};
//! Month length table, in the first dimension for a normal year, in the second for a leap year
const int month_lengths[2][12] = {{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}, {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};
//! English weekday names
const char* D_WEEKDAY_NAMES[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday", "Invalid"};
//! 3-letter English weekday name abbreviations 
const char* D_WEEKDAY_ABBRV[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun", "Inv"};
//! English month names
const char* D_MONTH_NAMES[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December", "Invalid"};
//! 3-letter English month name abbreviations 
const char* D_MONTH_ABBRV[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", "Inv"};
const char* D_AMPM_CAPS[] = {"AM", "PM"};
const char* D_AMPM_SMALL[] = {"a.m.", "p.m."};
const char* D_ADBC[] = {"CE", "BCE"};
const char* D_PLUSMINUS[] = {"+", "-"};

//! \brief Current time in process' time zone
//! \returns Current time
date_t get_current_time();
//! Convert a date to a different timezone
void convert_to_timezone(date_t *date, int tz_offset);
/*! \brief Convert date_t to string
    \returns String representation of a date (returned value needs to be freed)
*/
char* d_to_s(date_t);
//! Convert POSIX time_t time to date_t
date_t time_to_date(time_t);
//! Convert date_t time to POSIX time_t
time_t date_to_time(date_t);

//! Convert UNIX struct timeval time to date_t
date_t timeval_to_date(struct timeval, struct timezone);
//! Convert date_t to UNIX struct timeval
struct timeval date_to_timeval(date_t, struct timezone *);

//! \brief Number of microseconds since 0000-01-01 00:00 given a date_t
//! \returns Number of microseconds since 0000-01-01 00:00
long long date_to_usec_since_zero(date_t date);
//! \brief Create a date_t given number of microseconds since 0000-01-01 00:00
//! \param Number of microseconds since 0000-01-01 00:00
//! \returns A date correspnding to usec
date_t usec_since_zero_to_date(long long usec, int tz_offset);

//! Number of leap years before a year
int leap_years_before(int year);

//! Number of leap years in [start_year, end_year)
int leap_years_between(int start_year, int end_year);
bool is_leap_year(int year);
//! Number of days in a year
int year_length(int year);
/*!
\brief Compare two dates
\returns

 - 1 if greater is later the smaller
 - 0 is dates are equal
 - -1 if smaller is later the greater

*/
int date_compare(date_t greater, date_t smaller);
//! Number of the day in a year (January 1 is a first day)
int day_of_year(date_t);
//! Get ISO week number from a date_t
int iso_week_number(date_t);
//! Get ISO week-numbering year from a date_t
int iso_week_numbering_year(date_t);
//! Get century from a year (correct version, unlike in UNIX time command)
int century(int year);
//! Create a date object
date_t make_date(int year, int month, int day, int hour, int minute, int second, int usecond, int tz_offset);
//! Fix a broken date (e.g. 2015-01-32 becomes 2015-02-01)
void fix_date(date_t *);
//! Get day and month of easter in a year
void easter_in_year(date_t *date);
//! Difference between two dates in microseconds
long long usec_difference(date_t sooner, date_t later);
//! Difference between two dates in timediff_t
timediff_t difference(date_t sooner, date_t later);
//! Get date_t difference after a date
date_t date_add(date_t date, timediff_t difference);

date_t get_current_time()
{
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    // Make sure that GMT is not a result of obsolete struct timezone on this system
    if (tz.tz_minuteswest == 0)
    {
        // Another failsafe method of checking the timezone
        time_t t = time(NULL);
        struct tm *local = localtime(&t);
        tz.tz_minuteswest = (int)-local->tm_gmtoff/60;
    }
    return timeval_to_date(tv, tz);
}

void convert_to_timezone(date_t *date, int tz_offset)
{
    long long usec = date_to_usec_since_zero(*date);
    *date = usec_since_zero_to_date(usec, tz_offset);
}

date_t make_date(int year, int month, int day, int hour, int minute, int second, int usecond, int tz_offset)
{
    if (hour == 24) hour = 0;
    date_t date = {year, month, day, hour, minute, second, usecond, 0, tz_offset};
    convert_to_timezone(&date, tz_offset);
    return date;
}

void fix_date(date_t *date)
{
    if (date->hour == 24) date->hour = 0;
    convert_to_timezone(date, date->tz_offset);
}

time_t date_to_time(date_t date)
{
    time_t time = date.second + 60 * (date.minute - date.tz_offset) + 3600 * date.hour;
    int days_since_epoch = date.day - unix_epoch.day;
    
    while (date.month > unix_epoch.month)
    {
        date.month--;
        days_since_epoch += month_lengths[is_leap_year(date.year)][date.month - 1];
    }
    
    days_since_epoch += (date.year - unix_epoch.year) * 365 + leap_years_between(unix_epoch.year, date.year);
    
    time += days_since_epoch * 86400;
    
    return time;
}

date_t time_to_date(time_t time)
{
    date_t date = {0};
    int days_since_epoch = _div(time, 86400);
    int time_of_day = _mod(time, 86400);
    
    date.second = time_of_day % 60;
    date.minute = (time_of_day / 60) % 60;
    date.hour = time_of_day / 3600;
    
    date.weekday = _mod((unix_epoch.weekday + days_since_epoch), 7);
    
    date.year = unix_epoch.year;
    if (days_since_epoch >= 0)
    {
        while (days_since_epoch >= year_length(date.year))
        {
            days_since_epoch -= year_length(date.year);
            date.year++;
        }
    }
    else
    {
        while (days_since_epoch <= 0)
        {
            date.year--;
            days_since_epoch += year_length(date.year);
        }
    }
    
    date.month = unix_epoch.month;
    while (days_since_epoch >= month_lengths[is_leap_year(date.year)][date.month - 1])
    {
        days_since_epoch -= month_lengths[is_leap_year(date.year)][date.month - 1];
        date.month++;
    }
    
    date.day = unix_epoch.day + days_since_epoch;
    date.tz_offset = 0;
    
    return date;
}

date_t timeval_to_date(struct timeval tv, struct timezone tz)
{
    date_t date = time_to_date(tv.tv_sec - 60 * tz.tz_minuteswest);
    date.tz_offset = -tz.tz_minuteswest;
    date.usecond = tv.tv_usec;
    return date;
}

struct timeval date_to_timeval(date_t date, struct timezone *tz)
{
    time_t time = date_to_time(date) - 60 * date.tz_offset;
    if (tz != NULL)
    {
        tz->tz_minuteswest = -date.tz_offset;
    }
    struct timeval tv;
    tv.tv_sec = time;
    tv.tv_usec = date.usecond;
    return tv;
}

int day_of_year(date_t date)
{
    long long days = date.day;
    
    while (date.month > 1)
    {
        date.month--;
        days += month_lengths[is_leap_year(date.year)][date.month - 1];
    }
    
    return days;
}

int iso_week_number(date_t date)
{
    date_t first_of_year = date;
    first_of_year.day = 1;
    first_of_year.month = 1;
    fix_date(&first_of_year);
    first_of_year.day += _mod((first_of_year.weekday - 3), 7);
    fix_date(&first_of_year);
    
    if (date_compare(first_of_year, date) == 1)
    {
        return ((is_leap_year(date.year - 1)) ? 53 : 52);
    }
    else
    {
        return ((day_of_year(date) - first_of_year.day)/7 + 1);
    }
}

int iso_week_numbering_year(date_t date)
{
    if ((date.month == 12) && (date.day - date.weekday + 1) > 27)
    {
        return date.year + 1;
    }
    
    if ((date.month == 1) && (date.weekday - date.day) > 3)
    {
        return date.year - 1;
    } 

    return date.year;
}

int century(int year)
{
    if (year < 0) return (-year) / 100 - 1;
    return (year - 1) / 100 + 1;
}

long long date_to_usec_since_zero(date_t date)
{
    long long time = ((date.hour * 60 + date.minute - date.tz_offset) * 60 + date.second) * 1000000L + date.usecond;
    long long days_since_zero = date.day - 1;
    
    while (date.month > 1)
    {
        date.month--;
        days_since_zero += month_lengths[is_leap_year(date.year)][date.month - 1];
    }
    
    days_since_zero += date.year * 365 + leap_years_before(date.year + 1);
    
    time += days_since_zero * 86400000000L;
    
    return time;
}

date_t usec_since_zero_to_date(long long usec, int tz_offset)
{
    date_t date = {0};
    
    usec += tz_offset * 60000000L;
    date.tz_offset = tz_offset;
    
    long long time_of_day = _modl(usec, 86400000000L);
    long long days_since_zero = _divl(usec, 86400000000L);
    
    date.weekday = _modl((days_since_zero + 5), 7); //0000-01-01 was Saturday
    
    date.usecond = time_of_day % 1000000;
    date.second = (time_of_day / 1000000) % 60;
    date.minute = (time_of_day / 60000000) % 60;
    date.hour = time_of_day / 3600000000;
    
    static const int CYCLE_1 = 365;
    static const int CYCLE_4 = (CYCLE_1   *  4 + 1);
    static const int CYCLE_100 = (CYCLE_4   * 25 - 1);
    static const int CYCLE_400 = (CYCLE_100 *  4 + 1);
    
    date.year += 400 * _divl(days_since_zero, CYCLE_400);
    days_since_zero = _modl(days_since_zero, CYCLE_400);
    date.year += 100 * _divl(days_since_zero, CYCLE_100);
    days_since_zero = _modl(days_since_zero, CYCLE_100);
    date.year += 4 * _divl(days_since_zero, CYCLE_4);
    days_since_zero = _modl(days_since_zero, CYCLE_4);
    date.year += 1 * _divl(days_since_zero, CYCLE_1);
    days_since_zero = _modl(days_since_zero, CYCLE_1);
    
    date.month = 1;
    while (days_since_zero >= month_lengths[is_leap_year(date.year)][date.month - 1])
    {
        days_since_zero -= month_lengths[is_leap_year(date.year)][date.month - 1];
        date.month++;
    }
    
    date.day = days_since_zero + (date.year < 0 ? 0 : 1);
    
    return date;
}

int leap_years_before(int year)
{
    year--;
    return (year / 4) - (year / 100) + (year / 400);
}

// for years in [start, end)
int leap_years_between(int start, int end)
{
    //assert(start <= end);
    return leap_years_before(end) - leap_years_before(start);
}

int date_compare(date_t g, date_t s)
{
    long long g_time = date_to_usec_since_zero(g);
    long long s_time = date_to_usec_since_zero(s);
    if (g_time > s_time) return 1;
    else if (g_time < s_time) return -1;
    else return 0;
}

bool is_leap_year(int year)
{
    if (year % 4 != 0) return false;
    else if (year % 100 != 0) return true;
    else if (year % 400 != 0) return false;
    else return true;
}

int year_length(int year)
{
    return 365 + (is_leap_year(year) ? 1 : 0);
}

void easter_in_year(date_t *date)
{
   short a = (short)_mod(date->year, 19);
   short b = date->year >> 2;
   short c = (b / 25) + 1;
   short d = (c * 3) >> 2;
   short e = (short)_mod(((a * 19) - ((c * 8 + 5) / 25) + d + 15), 30);
   e += (29578 - a - e * 32) >> 10;
   e -= _mod((_mod(date->year, 7) + b - d + e + 2), 7);
   d = e >> 5;
   date->day = e - d * 31;
   date->month = d + 3;
}

long long usec_difference(date_t sooner, date_t later)
{
    return date_to_usec_since_zero(later) - date_to_usec_since_zero(sooner);
}

timediff_t difference(date_t sooner, date_t later)
{
    long long time = usec_difference(sooner, later);
    timediff_t diff;
    diff.useconds = time % 1000000;
    time /= 1000000;
    diff.seconds = time % 60;
    time /= 60;
    diff.minutes = time % 60;
    time /= 60;
    diff.hours = time % 24;
    time /= 24;
    diff.days = time % 7;
    time /= 7;
    diff.weeks = time;
    return diff;
}

date_t date_add(date_t date, timediff_t difference)
{
    date_t new_date = date;
    new_date.day += difference.days + difference.weeks * 7;
    new_date.hour += difference.hours;
    new_date.minute += difference.minutes;
    new_date.second += difference.seconds;
    new_date.usecond += difference.useconds;
    fix_date(&new_date);
    return new_date;
}

char* d_to_s(date_t d)
{
    unsigned len = 35 + strlen(D_WEEKDAY_ABBRV[d.weekday]) + (d.year < 0 ? 1 : 0);
    char* string = malloc(sizeof(char) * len);
    snprintf(string, sizeof(char) * len, "%s, %04d-%02d-%02d %02d:%02d:%02d.%06d%c%02d:%02d", D_WEEKDAY_ABBRV[d.weekday], d.year, d.month, d.day, d.hour, d.minute, d.second, d.usecond, (d.tz_offset >= 0 ? '+' : '-'), abs(d.tz_offset) / 60, abs(d.tz_offset) % 60);
    return string;
}
