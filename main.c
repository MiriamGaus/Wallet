#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "DateHandler.h"
#include "WalletHandler.h"

// ----------------------------------------- Utils --------------------------------------------

void printLine() {
    for (int i = 0; i < 40; i++) {
            printf("-");
        }
    printf("\n");
}

// ------------------------------- File Handling ----------------------------------------------

int transformIntoWallet(FILE *file, Wallet *wallet) {
    // Allocating memory for the entries
    fscanf(file, "%d", &wallet->categoriesSize);
    wallet->categories = (Category**) malloc(sizeof(Category*) * wallet->categoriesSize);
    if(!wallet->categories) return 0;
    Category *category;
    for(int i = 0; i < wallet->categoriesSize; i++) {
        category = (Category*) malloc(sizeof(Category));
        if(!category) {
            freeWallet(wallet);
            return 0;
        }
        fscanf(file, "%49s", category->name);
        wallet->categories[i] = category;
    }
    // Allocating memory for the entries
    fscanf(file, "%d", &wallet->size);
    wallet->entries = (Entry**) malloc(sizeof(Entry*) * wallet->size);
    if(!wallet->entries) return 0;
    Entry *entry;
    for (int i = 0; i < wallet->size; ++i) { 
        entry = (Entry *) malloc(sizeof(Entry));
        if(!entry) {
            freeWallet(wallet);
            return 0;
        }
        struct tm date = {0};
        entry->date = date;
        fscanf(file, "%d.%d.%d", &entry->date.tm_mday, &entry->date.tm_mon, &entry->date.tm_year);
        entry->date.tm_isdst = -1;
        fscanf(file, "%s", entry->categoryId);
        fscanf(file, "%lf", &entry->amount);
        fscanf(file, "%s", entry->currency);
        if(entry->amount >= 0) {
            entry->type = Income;
        } else {
            entry->type = Expense;
        }
        // Check if input entry is correct
        if(!isDateValid(entry->date) || !isCategory(wallet, entry->categoryId) || !isCurrency(entry->currency)) {
            printf("Error: Entry %d contains invalid data. File can not be read.\n", i);
            freeWallet(wallet);
            return 0;
        }
        addAmountToCategory(transformIntoCurrency(entry->amount, entry->currency, "EUR"), entry->categoryId, wallet);
        wallet->entries[i] = entry;
    }
    return 1;
}

void transformIntoFile(FILE *file, Wallet *wallet) {
    // Writing the number of categories into the file and writing the categories aswell
    fprintf(file, "%d\n", wallet->categoriesSize);
    for(int i = 0; i < wallet->categoriesSize; i++) {
        fprintf(file, "%s\n", wallet->categories[i]->name);
    }
    // Writing the number of entries into the file and writing the entries aswell
    fprintf(file, "%d\n", wallet->size);
    Entry *entry;
    for(int i = 0; i < wallet->size; i++) {
        entry = wallet->entries[i];
        fprintf(file, "%02d.%02d.%04d\n", entry->date.tm_mday, entry->date.tm_mon, entry->date.tm_year);
        fprintf(file, "%s\n", entry->categoryId);
        fprintf(file, "%.2f\n", entry->amount);
        fprintf(file, "%s\n", entry->currency);
    }
}

// --------------------------------------- main menu -------------------------------------------

int openFile(char *path, Wallet *wallet) {
    FILE *file = fopen(path, "r");
    if (file == NULL) return 0;
    int status = transformIntoWallet(file, wallet);
    fclose(file);
    return status;
}

// --------------------------------loaded-database menu ----------------------------------------

void enterEntry(Wallet *wallet) {
    // Reading the new entry from user input
    Entry *entry = (Entry*) malloc(sizeof(Entry));
    if(!entry) {
        printf("Error: Not able to add the new entry to the wallet. Try again.\n");
        return;
    }
    char type;
    // Get the date
    printf("To add a new income or expense, follow the following steps.\n");
    printf("1. Enter the date in the following format: YYYY MM DD \n");
    struct tm date = {0};
    entry->date = date;
    scanf("%d %d %d", &entry->date.tm_year, &entry->date.tm_mon, &entry->date.tm_mday);
    // Get the category
    printf("2. Enter the category: \n");
    scanf("%49s", entry->categoryId);
    // Get the amount
    printf("3. Enter the amount of the new entry: \n");
    scanf("%lf", &entry->amount);
    if(entry->amount >= 0) {
        entry->type = Income;
    } else {
        entry->type = Expense;
    }
    // Get the currency
    printf("4. Enter the currency (3 letters max):\n");
    scanf("%3s", entry->currency);
    // Storing the new entry in the wallet
    addEntry(wallet, entry);
}

void enterCategory(Wallet *wallet) {
    Category *category = (Category*) malloc(sizeof(Category));
    if(!category) return;
    printf("Enter the name of the new category: \n");
    scanf("%49s", category->name);
    category->total = 0;
    addCategory(wallet, category);
}

void evaluateTotalDb(Wallet *wallet) {
    // About the category with the highest expenses
    char currency[4];
    Category *expCategory;
    // About the total evaluation
    double totalExenses = 0;
    double totalIncomes = 0;
    if(wallet->categoriesSize == 0) {
        printf("Error: Database can not be evaluated. There are no categories.\n");
        return;
    }
    printf("Enter the currency to evaluate the wallet.\n");
    scanf("%3s", currency);
    if(!isCurrency(currency)) {
        printf("Error: %s is not a supported currency", currency);
        return;
    }
    printLine();
    // All totals are stored in EUR.
    expCategory = wallet->categories[0];
    for(int i = 0; i < wallet->categoriesSize; i++) {
        if(wallet->categories[i]->total < expCategory->total) {
            expCategory = wallet->categories[i];
        }
    }
    printf("Category with the highest expense:\n%s\n", expCategory->name);
    printf("Amount: %28.2f %s\n", transformIntoCurrency(expCategory->total, "EUR", currency), currency);
    printLine();
    for(int i = 0; i < wallet->size; i++) {
        if(wallet->entries[i]->type == Income) {
            totalIncomes += transformIntoCurrency(wallet->entries[i]->amount, wallet->entries[i]->currency, currency);
        } else {
            totalExenses += transformIntoCurrency(wallet->entries[i]->amount, wallet->entries[i]->currency, currency);
        }
    }
    double balance = totalExenses + totalIncomes;
    printf("Total income: %22.2f %s\n", totalIncomes, currency);
    printf("Total expense: %21.2f %s\n", totalExenses, currency);
    printLine();
    printf("Current balance: %19.2f %s\n", balance, currency);
}

void displayTimePeriod(Wallet *wallet) {
    printf("Enter the start date (format: YYYY MM DD)\n");
    struct tm start = {0};
    scanf("%d %d %d", &start.tm_year, &start.tm_mon, &start.tm_mday);
    printf("Enter the end date (format: YYYY MM DD)\n");
    struct tm end = {0};
    scanf("%d %d %d", &end.tm_year, &end.tm_mon, &end.tm_mday);
    if(!isDateValid(start) || !isDateValid(end)) return;
    char currency[4];
    printf("Enter the currency to display the statistics: \n");
    scanf("%s", currency);
    if(!isCurrency(currency)) return;
    double expense = 0;
    double income = 0;
    double largestExpense = __INT16_MAX__;
    int count;
    for(int i = 0; i < wallet->categoriesSize; i++) {
        expense = 0;
        income = 0;
        largestExpense = __INT16_MAX__;
        count = 0;
        for(int j = 0; j < wallet->size; j++) {
            Entry entry = *wallet->entries[i];
            // Check whether the entry is in the right time interval and whether it belongs to the right catgory
            if(isDateBetween(entry.date, start, end) && strcmp(wallet->categories[i]->name, entry.categoryId)) {
                printEntry(entry);
                if(entry.type = Income) {
                    income += transformIntoCurrency(entry.amount, entry.currency, currency);
                } else {
                    expense += transformIntoCurrency(entry.amount, entry.currency, currency);
                }
                if(transformIntoCurrency(entry.amount, entry.currency, currency) < largestExpense) {
                    largestExpense = transformIntoCurrency(entry.amount, entry.currency, currency) < largestExpense;
                }
                count++;
            }
        }
        printLine();
        printf("%s\n", wallet->categories[i]->name);
        if(count == 0) {
            printf("No entry in this category.\n");
        } else {
            printf("Income: %28.2f %s\n", income, currency);
            printf("Expense: %27.2f %s\n", income, currency);
            printf("Largest Expense: %19.2f %s\n", income, currency);
            printf("Balance: %27.2f %s\n", income + expense, currency);
        }
    }
}

int saveDb(char *path, Wallet *wallet) {
    FILE *file = fopen(path, "w");
    if (file == NULL) {
        printf("Error: The wallet can not be saved. Please try again.\n");
        return 0;
    }
    transformIntoFile(file, wallet);
    fclose(file);
    return 1;
}

void handleInput(char *path, Wallet *wallet) {
    int choice;
    int exit = 0;
    while(1) {
        printLine();
        printf("Enter  \n1 to add a new entry. \n2 to create a new catergory. \n3 to evaluate the database in total. \n4 to display all entries in a given time period. \n5 to save the database. \n9 to close the database. \n");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                enterEntry(wallet);
                break;
            case 2:
                enterCategory(wallet);
                break;
            case 3:
                evaluateTotalDb(wallet);
                break;
            case 4:
                displayTimePeriod(wallet);
                break;
            case 5:
                saveDb(path, wallet);
                break;
            case 9: 
                exit = saveDb(path, wallet);
                // Only free the memory of the wallet, if the saving is successful
                if(exit) freeWallet(wallet);
                break;
            default:
                printf("There is no action for the given input. Choose another action!\n");
                break;
        }
        if (exit) break;
    }
}

// ------------------------------------------ main ---------------------------------------------

int main(void) {
    char path[50];
    Wallet *wallet = (Wallet*) malloc(sizeof(Wallet));
    printf("Welcome to your WALLET :)\n");
    while(1) {
        printLine();
        printf("To use the WALLET enter the path to an existing database.\n                   OR\nTo exit the program enter \"exit\".\n");
        scanf("%s", path);
        // To exit the program
        if(strcmp(path, "exit") == 0) {
            break;
        } 
        // Open the file
        if (openFile(path, wallet)) {
            printf("The database is loaded.\n");
            handleInput(path, wallet);
        } else {
            printf("Error: During opening the database. Please try again.\n");
        }
    }
    return 0;
}