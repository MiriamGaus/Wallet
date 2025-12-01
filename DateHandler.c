#include <stdio.h>
#include "DateHandler.h"

/**
 * Validate if a given date is correct.
 * Adjusts the fields to suit mktime normalization and compares results.
 * Prints an error message on invalid date.
 */
int isDateValid(struct tm date)
{
    struct tm copy = date;
    time_t t = mktime(&copy);
    if (t == -1)
        return 0;
    if (date.tm_year == copy.tm_year && date.tm_mon == copy.tm_mon && date.tm_mday == copy.tm_mday)
    {
        return 1;
    }

    printf("Error: The date %02d.%02d.%04d is not valid.\n", date.tm_mday, date.tm_mon + 1, date.tm_year + 1900);
    return 0;
}

/**
 * Check if a date lies between start and end dates inclusively.
 * All dates are normalized using mktime before comparison.
 */
int isDateBetween(struct tm date, struct tm start, struct tm end)
{
    return (mktime(&start) <= mktime(&date)) && (mktime(&date) <= mktime(&end));
}

/**
 * Normalizes the date that it can be internally stored as a struct tm.
 */
struct tm normalize(int year, int month, int day)
{
    struct tm date = {0};
    date.tm_year = year - 1900;
    date.tm_mon = month - 1;
    date.tm_mday = day;
    return date;
};
