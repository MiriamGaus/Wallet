#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include "WalletHandler.h"
#include "DateHandler.h"

/**
 * Prints a horizontal line of dashes for UI separation.
 */
void printLine();

/**
 * Prompts user to enter a new entry and adds it to the Wallet.
 * @param wallet Pointer to the Wallet struct.
 */
void enterEntry(Wallet *wallet);

/**
 * Prompts user to enter a new category and adds it to the Wallet.
 * @param wallet Pointer to the Wallet struct.
 */
void enterCategory(Wallet *wallet);

/**
 * Evaluates the wallet's totals and highest expense category in a chosen currency.
 * @param wallet Pointer to the Wallet struct.
 */
void evaluateTotalDb(Wallet *wallet);

/**
 * Displays income, expense, largest expense, and balance for each category during a date range.
 * @param wallet Pointer to the Wallet struct.
 */
void displayTimePeriod(Wallet *wallet);

/**
 * Main loop to handle user menu input and perform wallet operations.
 * @param path Path to current wallet database file.
 * @param wallet Pointer to the Wallet struct.
 */
void handleInput(char *path, Wallet *wallet);

#endif