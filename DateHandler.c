#include <stdio.h>
#include <time.h>
#include "DateHandler.h"

int isDateValid(struct tm date) {
    date.tm_year -= 1900;
    date.tm_mon -= 1;
    struct tm copy = date;         
    time_t t = mktime(&copy);

    if (t == -1) return 0;
    if(date.tm_year == copy.tm_year && date.tm_mon == copy.tm_mon && date.tm_mday == copy.tm_mday) {
        return 1;
    }
    printf("Error: Incorrect date %02d.%02d.%04d\n", date.tm_mday, date.tm_mon, date.tm_year);
    return 0;
}

int isDateBetween(struct tm date, struct tm start, struct tm end) {
    // Normalize all the dates because thats how they are represented in time.h
    date.tm_year -= 1900;
    date.tm_mon -= 1;
    start.tm_year -= 1900;
    start.tm_mon -= 1;
    end.tm_year -= 1900;
    end.tm_mon -= 1;
    return(mktime(&start) <= mktime(&date)) && (mktime(&date) <= mktime(&end));
}