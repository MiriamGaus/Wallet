#ifndef DATEHANDLER_H
#define DATEHANDLER_H

#include <time.h>

/**
 * Checks if a given date is valid according to the time library.
 *
 * The date fields tm_year and tm_mon should be in normal human-readable
 * form (e.g., tm_year as full year, tm_mon as 1-12).
 * Internally, the function adjusts them for mktime usage.
 *
 * @param date A struct tm representing the date to validate.
 * @return 1 if the date is valid, 0 otherwise.
 */
int isDateValid(struct tm date);

/**
 * Checks if a date lies inclusively between a start and end date.
 *
 * The date fields tm_year and tm_mon should be in normal human-readable
 * form.
 * Internally, the function normalizes the dates for comparison.
 *
 * @param date The date to check.
 * @param start The start of the date range.
 * @param end The end of the date range.
 * @return 1 if date is in the range [start, end], 0 otherwise.
 */
int isDateBetween(struct tm date, struct tm start, struct tm end);

/**
 * Normalizes a date given in year, month, day format into a struct tm.
 *
 * @param year The year of the date.
 * @param month The month of the date (1-12).
 * @param day The day of the date.
 * @return A struct tm representing the normalized date.
 */
struct tm normalize(int year, int month, int day);

#endif