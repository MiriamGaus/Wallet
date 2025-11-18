#ifndef DATEHANDLER_H
#define DATEHANDLER_H

int isDateValid(struct tm date);
int isDateBetween(struct tm date, struct tm start, struct tm end);

#endif