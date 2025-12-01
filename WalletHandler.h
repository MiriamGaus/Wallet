#ifndef WALLETHANDLER_H
#define WALLETHANDLER_H

#include <time.h>

/**
 * Enum representing the type of an entry: Income or Expense.
 */
typedef enum entry_type
{
    Income, /**< Entry represents income */
    Expense /**< Entry represents expense */
} entry_type;

/**
 * Structure representing a category for entries.
 * Contains a name and a total amount tracked for that category.
 */
typedef struct Category
{
    char name[50]; /**< Name of the category */
    double total;  /**< Total amount belonging to this category */
} Category;

/**
 * Structure representing a single financial entry in the wallet.
 * It contains a date, type (income/expense), category ID, amount, and currency code.
 */
typedef struct Entry
{
    struct tm date;      /**< Date of the entry */
    entry_type type;     /**< Type of entry: Income or Expense */
    char categoryId[50]; /**< Identifier of the category for this entry */
    double amount;       /**< Monetary amount of this entry */
    char currency[4];    /**< Currency code of the entry amount (e.g., "EUR") */
} Entry;

/**
 * Structure representing the wallet.
 * It contains a dynamic list of entries, and a dynamic list of categories.
 */
typedef struct Wallet
{
    Entry **entries;       /**< Array of pointers to entries */
    int size;              /**< Current number of entries */
    Category **categories; /**< Array of pointers to categories */
    int categoriesSize;    /**< Current number of categories */
} Wallet;

/**
 * Static array of exchange rates to convert between supported currency pairs.
 * Each element contains a currency pair string and the corresponding exchange rate.
 */
static struct
{
    char pair[9]; /**< Currency pair string in format "XXX->YYY" */
    double rate;  /**< Exchange rate from first to second currency */
} exchangeRates[] = {
    {"EUR->HUF", 385.0},
    {"HUF->EUR", 0.0026},
    {"EUR->USD", 1.15},
    {"USD->EUR", 0.87},
    {"HUF->USD", 330},
    {"USD->HUF", 1 / 330}};

/**
 * Prints the details of a given Entry.
 * @param entry The Entry to print.
 */
void printEntry(Entry entry);

/**
 * Checks if a category with the given name exists in the wallet.
 * @param wallet Pointer to the Wallet structure.
 * @param category Name of the category to check.
 * @return 1 if the category exists, 0 otherwise.
 */
int isCategory(Wallet *wallet, char *category);

/**
 * Checks if the given currency code is valid/supported.
 * @param currency String representing the currency code.
 * @return 1 if currency is valid, 0 otherwise.
 */
int isCurrency(char *currency);

/**
 * Converts the given amount from oldCurr currency to newCurr currency using the exchange rates.
 * @param amount The amount to convert.
 * @param oldCurr The currency code of the original amount.
 * @param newCurr The target currency code.
 * @return The converted amount in newCurr currency.
 */
double transformIntoCurrency(double amount, char *oldCurr, char *newCurr);

/**
 * Adds the specified amount to the total of the category identified by categoryId in the wallet.
 * @param amount The amount to add.
 * @param categoryId The identifier/name of the category.
 * @param wallet Pointer to the Wallet structure.
 */
void addAmountToCategory(double amount, char *categoryId, Wallet *wallet);

/**
 * Adds an Entry to the Wallet.
 * @param wallet Pointer to the Wallet structure.
 * @param entry Pointer to the Entry to add.
 */
void addEntry(Wallet *wallet, Entry *entry);

/**
 * Adds a Category to the Wallet.
 * @param wallet Pointer to the Wallet structure.
 * @param category Pointer to the Category to add.
 */
void addCategory(Wallet *wallet, Category *category);

/**
 * Frees all dynamically allocated memory inside the Wallet structure,
 * including entries and categories arrays.
 * @param wallet Pointer to the Wallet structure to free.
 */
void freeWallet(Wallet *wallet);

#endif