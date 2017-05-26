/*! \file */

void dnprintf(date_t d, char* const buffer, size_t len, const char* format);
int place_n_in_s(char* buffer, size_t len, long long num, char opt, short padd);
int place_s_in_s(char* buffer, size_t len, char* str, char opt, short padd);
int convert_to_roman(unsigned int val, char *buf, size_t len);

//! \cond foo
#define PUT(NUM, OPT, PADD) j += place_n_in_s(buffer + j, len - j - 1, NUM, OPT, PADD)
#define PUTS(STR, OPT, PADD) j += place_s_in_s(buffer + j, len - j - 1, STR, OPT, PADD)
//! \endcond

#define F_ISO_8601_T "%0Y-%0m-%0dT%0H:%0M:%0S.%0u%t%0Z:%0z"         //!< Example: 2015-06-11T21:53:12.543294091+02:00
#define F_ISO_8601_SPACE "%0Y-%0m-%0d %0H:%0M:%0S.%0u %t%0Z:%0z"    //!< Example: 2015-06-11 21:53:12.543294091 +02:00
#define F_ISO_8601_WDATE "%0Y-W%0W-%w"                              //!< Example: 2015-W23-4
#define F_TIME "%0H:%0M:%0S"                                        //!< Example: 22:00:00
#define F_DATE "%0Y-%0m-%0d"                                        //!< Example: 2015-06-11
#define F_ISO_8601_NOUSEC "%0Y-%0m-%0d %0H:%0M:%0S %t%0Z:%0z"       //!< Example: 2015-06-11 21:53:12 +02:00
#define F_RFC_2822 "%b, %d %a %Y %H:%0M:%0S %t%0Z%0z"               //!< Example: Sat, 13 Mar 2010 11:29:05 -0800
#define F_US_SHORT "%m/%d/%y %I:%0M %p"                             //!< Example: 6/11/15 9:55 p.m.
#define F_US_LONG "%b %d %a %Y, %I:%0M %p"                          //!< Example: Thu 11 Jun 2015, 9:59 p.m.
#define F_US_LONGER  "%B, %A %d, %Y, %I:%0M %p"                     //!< Example: Thursday, June 6, 2015, 9:57 p.m.


//! Helper function to put a number inside of string buffer
/*!
    \param buffer string buffer
    \param len size of the buffer
    \param num number to put
    \param opt optional formatting character (see dnprintf)
    \param padd minimum number of characters to be printed; if the value to be printed is shorter than this number, the result is padded
*/
int place_n_in_s(char* buffer, size_t len, long long num, char opt, short padd)
{
    if (num < 0) padd++;
    if (opt == 0 || opt == '^') return snprintf(buffer, len, "%lld", num);
    char* f = strdup("% *lld");
    f[1] = opt;
    int ret = snprintf(buffer, len, f, padd, num);
    free(f);
    return ret;
}

//! Helper function to put a string inside of string buffer
/*!
    \param buffer string buffer
    \param len size of the buffer
    \param str string to put
    \param opt optional formatting character (see dnprintf)
    \param padd minimum number of characters to be printed; if the value to be printed is shorter than this number, the result is padded
*/
int place_s_in_s(char* buffer, size_t len, char* str, char opt, short padd)
{
    if (opt == 0 || opt == '^') return snprintf(buffer, len, "%s", str);
    char* f = strdup("% *s");
    f[1] = opt;
    int ret = snprintf(buffer, len, f, padd, str);
    free(f);
    return ret;
}

//! Convert a number to roman numeral string
/*!
    \param val number to convert
    \param buf buffer in which the string is to be created
    \param len size of the buffer
*/
int convert_to_roman(unsigned int val, char *buf, size_t len)
{
    char *init = buf;
    char *huns[] = {"", "C", "CC", "CCC", "CD", "D", "DC", "DCC", "DCCC", "CM"};
    char *tens[] = {"", "X", "XX", "XXX", "XL", "L", "LX", "LXX", "LXXX", "XC"};
    char *ones[] = {"", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX"};
    int size[] = {0, 1, 2, 3, 2, 1, 2, 3, 4, 2};

    while (val >= 1000) {
        if (len-- < 1) return 0;
        *buf++ = 'M';
        val -= 1000;
    }

    if (len < size[val/100]) return 0;
    len -= size[val/100];
    strcpy(buf, huns[val/100]);
    buf += size[val/100];
    val = val % 100;

    if (len < size[val/10]) return 0;
    len -= size[val/10];
    strcpy(buf, tens[val/10]);
    buf += size[val/10];
    val = val % 10;

    if (len < size[val]) return 0;
    len -= size[val];
    strcpy(buf, ones[val]);
    buf += size[val];

    if (len < 1) return len;
    *buf = 0;
    return buf - init;
}

//! Create a date string with format
/*!
    \param d date to create a string from
    \param buffer buffer in which the string is to be created
    \param len size of the buffer
    \param format format string
    \brief
Interpreted sequences of `format' are:
    
- %% – a literal %
- \%H – hour (00..23)
- \%I – hour (01..12)
- \%M – minute
- \%S – second
- \%s – seconds since the beginning of UNIX epoch (1970-01-01T00:00:00.0Z)
- \%u – microseconds
- \%Y – full year (year 0 is 1 BC, year can be negative)
- \%y – last two digits of %Y
- \%F – ISO week-numbering year 
- \%J – full year (no year 0, no negative years)
- \%j – last two digits of %J
- \%m – month (1..12)
- \%d – day of the month                
- \%a – abbreviated name of the month (3 characters)
- \%A – full name of the month
- \%r – month as a Roman numeral (I..XII)
- \%R – year as a Roman numeral
- \%b – abbreviated name of the weekday (3 characters)
- \%B – full name of the weekday
- \%w – weekday (1..7, where 1 is Monday)
- \%v – weekday (0..6, where 0 is Sunday)
- \%c – century (arabic)
- \%C – century (roman)
- \%L – CE/BCE
- \%l – like %L but with plus/minus sign instead
- \%W – ISO week number
- \%p – am/pm
- \%P – AM/PM
- \%t – timezone sign (`+' for east, `-' for west)
- \%Z – timezone hours
- \%z – timezone minutes
- \%X – timezone minutes only (%Z * 60 + %z)

Optional flags may follow `\%':
    
- `+' – force display of sign
- `0' – pad with zeros
- ` ' – (a space) pad with spaces
`-' – left justify within a given field
*/
void dnprintf(date_t d, char* const buffer, size_t len, const char* format)
{
    size_t f_len = strlen(format);
    int j = 0;
    for (int i = 0; i < f_len; i++)
    {
        char c = format[i];
        if (c != '%')
        {
            buffer[j++] = c;
        }
        else
        {
            char opt = 0;
            c = format[++i];
            if (c == 0) break;
            else if (c == '+' || c == '0' || c == ' ')
            {
                opt = c;
                c = format[++i];
                if (c == 0) break;
            }
            switch (c)
            {
                case '%': buffer[j++] = '%'; break;
                case 'H': PUT(d.hour, opt, 2*(opt != 0)); break;
                case 'I': PUT((d.hour % 12 == 0 ? 12 : d.hour % 12), opt, 2*(opt != 0)); break;
                case 'M': PUT(d.minute, opt, 2*(opt != 0)); break;
                case 'S': PUT(d.second, opt, 2*(opt != 0)); break;
                case 's': PUT(date_to_usec_since_zero(d)/1000000L, opt, 12*(opt != 0)); break;
                case 'u': PUT(d.usecond, opt, 6*(opt != 0)); break;
                case 'Y': PUT(d.year, opt, 4*(opt != 0)); break;
                case 'y': PUT(d.year % 100, opt, 2*(opt != 0)); break;
                case 'F': PUT(iso_week_numbering_year(d), opt, 4*(opt != 0)); break;
                case 'J': PUT((d.year <= 0 ? - d.year + 1 : d.year), opt, 4*(opt != 0)); break;
                case 'j': PUT((d.year <= 0 ? - d.year + 1 : d.year) % 100, opt, 2*(opt != 0)); break;
                case 'm': PUT(d.month, opt, 2*(opt != 0)); break;
                case 'd': PUT(d.day, opt, 2*(opt != 0)); break;
                
                case 'a': PUTS((char*)D_MONTH_ABBRV[d.month - 1], opt, 3*(opt != 0)); break;
                case 'A': PUTS((char*)D_MONTH_NAMES[d.month - 1], opt, 9*(opt != 0)); break;
                case 'r': j += convert_to_roman(d.month, buffer + j, len - j - 1); break;
                case 'R': j += convert_to_roman((d.year <= 0 ? - d.year + 1 : d.year), buffer + j, len - j - 1); break;
                case 'b': PUTS((char*)D_WEEKDAY_ABBRV[d.weekday], opt, 3*(opt != 0)); break;
                case 'B': PUTS((char*)D_WEEKDAY_NAMES[d.weekday], opt, 3*(opt != 0)); break;
                case 'w': PUT(d.weekday + 1, opt, (opt != 0)); break;
                case 'v': PUT((d.weekday + 1) % 7, opt, (opt != 0)); break;
                case 'c': PUT(abs(century(d.year)), opt, 2*(opt != 0)); break;
                case 'C': j += convert_to_roman(abs(century(d.year)), buffer + j, len - j); break;
                case 'L': PUTS((char*)D_ADBC[d.year <= 0], opt, 2*(opt != 0)); break;
                case 'l': PUTS((char*)D_PLUSMINUS[d.year <= 0], opt, (opt != 0)); break;
                case 'W': PUT(iso_week_number(d), opt, 2*(opt != 0)); break;
                case 'p': PUTS((char*)D_AMPM_SMALL[d.hour/12], opt, 3*(opt != 0)); break;
                case 'P': PUTS((char*)D_AMPM_CAPS[d.hour/12], opt, 3*(opt != 0)); break;
                
                case 't': PUTS((char*)D_PLUSMINUS[d.tz_offset < 0], opt, (opt != 0)); break;
                case 'Z': PUT(abs(d.tz_offset) / 60, opt, 2*(opt != 0)); break;
                case 'z': PUT(abs(d.tz_offset) % 60, opt, 2*(opt != 0)); break;
                case 'X': PUT(abs(d.tz_offset), opt, 2*(opt != 0)); break;
            }
        }
    }
    buffer[j] = 0;
}
