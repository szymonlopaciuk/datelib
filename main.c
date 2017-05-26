#include "datecal.h"
#include "dateformat.h"
#include <time.h>

#define TEST(DATE, STR) dnprintf(DATE, buffer, 100, STR); \
                        printf(" - %s\n", buffer)

int main(int argc, char** argv)
{
    date_t now = get_current_time();
    char* string = d_to_s(now);
    
    date_t caesars_assasitation = make_date(-43, 3, 15, 21, 0, 0, 0, 120);
    date_t pearl_harbor = make_date(1941, 12, 7, 7, 48, 0, 0, -630);
    
    char buffer[100] = {0};
    
    printf("\n================ Now ===============\n");
    
    TEST(now, F_ISO_8601_SPACE);
    TEST(now, F_ISO_8601_T);
    TEST(now, F_RFC_2822);
    TEST(now, F_ISO_8601_WDATE);
    TEST(now, "%b %a %d, %Y, %I:%0M %p");
    TEST(now, "%d %r %Y, %H:%0M");
    TEST(now, "%m/%d/%y %I:%0M %P");
    TEST(now, "%d.%0m.%Y %H:%0M");
    easter_in_year(&now);
    TEST(now, "Easter that year: %d.%0m.%J %L");
    
    timediff_t append = {1, 1, 1, 1, 1, 1};
    now = date_add(now, append);
    printf("\n   Date in 1 week, 1 day, 1 hour, 1 min and one sec from this Easter:\n");
    TEST(now, F_ISO_8601_SPACE);
    
    printf("\n== Assassination of Julius Caesar ==\n");
    
    TEST(caesars_assasitation, F_ISO_8601_SPACE);
    TEST(caesars_assasitation, "%A %d, %R %L (%B)");
    TEST(caesars_assasitation, "%b, %d %A %J %L");
    date_t easter = caesars_assasitation;
    easter_in_year(&easter);
    timediff_t easter_diff = difference(caesars_assasitation, easter);
    TEST(easter, "Easter that year: %d.%0m.%J %L");
    printf("   (difference of %d weeks and %d days)\n", easter_diff.weeks, easter_diff.days);
    
    printf("\n====== Attack on Pearl Harbor ======\n");
    
    TEST(pearl_harbor, F_ISO_8601_T);
    TEST(pearl_harbor, "at %I:%0M %p, on %A %d, %Y (Hawaii, UTC%t%0Z:%0z)");
    convert_to_timezone(&pearl_harbor, 540);
    TEST(pearl_harbor, "at %I:%0M %p, on %A %d, %Y (Japan, UTC%t%0Z:%0z)");
    convert_to_timezone(&pearl_harbor, -300);
    TEST(pearl_harbor, "at %I:%0M %p, on %A %d, %Y (Washingon D.C., UTC%t%0Z:%0z)");
    easter_in_year(&pearl_harbor);
    TEST(pearl_harbor, "Easter that year: %d.%0m.%J %L");
    
    timediff_t diff = difference(pearl_harbor, now);
    printf("\nAttack on Pearl Harbor happened %d weeks, %d days, %d hrs and %d mins ago.\n",
        diff.weeks, diff.days, diff.hours, diff.minutes);
    
    return 0;
}
