#include "FileIO.h"
#include <stdio.h>
#include <stdlib.h>
#include "DateHandler.h"
#include "WalletHandler.h"

/**
 * Loads wallet data from a file into the Wallet struct.
 */
int loadWalletFromFile(FILE *file, Wallet *wallet)
{
    // Allocating memory for the entries
    fscanf(file, "%d", &wallet->categoriesSize);
    wallet->categories = (Category **)malloc(sizeof(Category *) * wallet->categoriesSize);
    if (!wallet->categories)
        return 0;
    Category *category;
    for (int i = 0; i < wallet->categoriesSize; i++)
    {
        category = (Category *)malloc(sizeof(Category));
        if (!category)
        {
            freeWallet(wallet);
            return 0;
        }
        fscanf(file, "%49s", category->name);
        wallet->categories[i] = category;
    }
    // Allocating memory for the entries
    fscanf(file, "%d", &wallet->size);
    wallet->entries = (Entry **)malloc(sizeof(Entry *) * wallet->size);
    if (!wallet->entries)
        return 0;
    Entry *entry;
    for (int i = 0; i < wallet->size; ++i)
    {
        entry = (Entry *)malloc(sizeof(Entry));
        if (!entry)
        {
            freeWallet(wallet);
            return 0;
        }
        int year, month, day;
        fscanf(file, "%d.%d.%d", &day, &month, &year);
        entry->date = normalize(year, month, day);
        fscanf(file, "%s", entry->categoryId);
        fscanf(file, "%lf", &entry->amount);
        fscanf(file, "%s", entry->currency);
        if (entry->amount >= 0)
        {
            entry->type = Income;
        }
        else
        {
            entry->type = Expense;
        }
        // Check if input entry is correct
        if (!isDateValid(entry->date) || !isCategory(wallet, entry->categoryId) || !isCurrency(entry->currency))
        {
            printf("Error: Entry %d contains invalid data. File can not be read.\n", i);
            freeWallet(wallet);
            return 0;
        }
        addAmountToCategory(transformIntoCurrency(entry->amount, entry->currency, "EUR"), entry->categoryId, wallet);
        wallet->entries[i] = entry;
    }
    return 1;
}

/**
 * Saves the Wallet data into a file.
 */
void saveWalletToFile(FILE *file, Wallet *wallet)
{
    // Writing the number of categories into the file and writing the categories aswell
    fprintf(file, "%d\n", wallet->categoriesSize);
    for (int i = 0; i < wallet->categoriesSize; i++)
    {
        fprintf(file, "%s\n", wallet->categories[i]->name);
    }
    // Writing the number of entries into the file and writing the entries aswell
    fprintf(file, "%d\n", wallet->size);
    Entry *entry;
    for (int i = 0; i < wallet->size; i++)
    {
        entry = wallet->entries[i];
        fprintf(file, "%02d.%02d.%04d\n", entry->date.tm_mday, entry->date.tm_mon + 1, entry->date.tm_year + 1900);
        fprintf(file, "%s\n", entry->categoryId);
        fprintf(file, "%.2f\n", entry->amount);
        fprintf(file, "%s\n", entry->currency);
    }
}

/**
 * Opens a wallet database file and loads its data into Wallet struct.
 */
int openFile(char *path, Wallet *wallet)
{
    FILE *file = fopen(path, "r");
    if (file == NULL)
        return 0;
    int status = loadWalletFromFile(file, wallet);
    fclose(file);
    return status;
}

/**
 * Saves the Wallet data to a file at the specified path.
 */
int saveDb(char *path, Wallet *wallet)
{
    FILE *file = fopen(path, "w");
    if (file == NULL)
    {
        printf("Error: The wallet can not be saved. Please try again.\n");
        return 0;
    }
    saveWalletToFile(file, wallet);
    fclose(file);
    printf("The wallet is saved successfully.\n");
    return 1;
}
