#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DateHandler.h"
#include "WalletHandler.h"

/**
 * Prints the details of a wallet entry in a formatted manner.
 */
void printEntry(Entry entry)
{
    const char *typeStr;
    switch (entry.type)
    {
    case Income:
        typeStr = "Income";
        break;
    case Expense:
        typeStr = "Expense";
        break;
    default:
        typeStr = "Unknown";
        break;
    }
    printf("%02d.%02d.%04d %s %s %.2f %s\n", entry.date.tm_mday, entry.date.tm_mon, entry.date.tm_year,
           typeStr, entry.categoryId, entry.amount, entry.currency);
}

/**
 * Checks if a given category exists in the wallet.
 */
int isCategory(Wallet *wallet, char *category)
{
    for (int i = 0; i < wallet->categoriesSize; i++)
    {
        if (strcmp(wallet->categories[i]->name, category) == 0)
        {
            return 1;
        }
    }
    return 0;
}

/**
 * Checks if a given currency is supported.
 */
int isCurrency(char *currency)
{
    if (!strcmp(currency, "EUR") || !strcmp(currency, "HUF") || !strcmp(currency, "USD"))
        return 1;
    printf("Error: %s is not a supported currency.\n", currency);
    return 0;
}

/**
 * Transforms an amount from one currency to another using predefined exchange rates.
 */
double transformIntoCurrency(double amount, char *oldCurr, char *newCurr)
{
    if (strcmp(oldCurr, newCurr) == 0)
    {
        return amount;
    }
    char key[9];
    sprintf(key, "%s->%s", oldCurr, newCurr);
    for (int i = 0; i < 6; i++)
    {
        if (strcmp(key, exchangeRates[i].pair) == 0)
        {
            return amount * exchangeRates[i].rate;
        }
    }
    return 0;
}

/**
 * Adds a specified amount to a category's total in the wallet.
 */
void addAmountToCategory(double amount, char *categoryId, Wallet *wallet)
{
    for (int i = 0; i < wallet->categoriesSize; i++)
    {
        if (strcmp(wallet->categories[i]->name, categoryId) == 0)
        {
            wallet->categories[i]->total += amount;
            return;
        }
    }
}

/**
 * Adds a new entry to the wallet after validating it.
 */
void addEntry(Wallet *wallet, Entry *entry)
{
    if (!isDateValid(entry->date) || !isCategory(wallet, entry->categoryId) || !isCurrency(entry->currency) || (entry->amount == 0))
    {
        printf("Error: Not able to add the new entry to the wallet. Try again.\n");
        free(entry);
        return;
    }
    Entry **newEntries = realloc(wallet->entries, (wallet->size + 1) * sizeof(Entry *));
    if (newEntries == NULL)
    {
        printf("Error: Not able to add the new entry to the wallet. Try again.\n");
        return;
    }
    wallet->entries = newEntries;
    wallet->entries[wallet->size] = entry;
    wallet->size++;
    addAmountToCategory(transformIntoCurrency(entry->amount, entry->currency, "EUR"), entry->categoryId, wallet);
    printf("The new entry successfully added to the wallet: \n");
    printEntry(*entry);
}

/**
 * Adds a new category to the wallet after checking for duplicates.
 */
void addCategory(Wallet *wallet, Category *category)
{
    if (isCategory(wallet, category->name))
    {
        printf("This category already exits.\n");
        free(category);
        return;
    }
    Category **categories;
    categories = (Category **)realloc(wallet->categories, (wallet->categoriesSize + 1) * sizeof(Category *));
    if (!categories)
        return;
    wallet->categories = categories;
    wallet->categories[wallet->categoriesSize] = category;
    wallet->categoriesSize++;
    printf("The new category %s is successfully added.\n", category->name);
}

/**
 * Frees all dynamically allocated memory associated with the wallet.
 */
void freeWallet(Wallet *wallet)
{
    if (!wallet)
        return;
    for (int i = 0; i < wallet->categoriesSize; i++)
    {
        free(wallet->categories[i]);
        wallet->categories[i] = NULL;
    }
    free(wallet->categories);
    wallet->categories = NULL;
    for (int i = 0; i < wallet->size; i++)
    {
        free(wallet->entries[i]);
        wallet->entries[i] = NULL;
    }
    free(wallet->entries);
    wallet->entries = NULL;
    free(wallet);
    wallet = NULL;
}