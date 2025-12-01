#include "InputHandler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DateHandler.h"
#include "FileIO.h"
#include "WalletHandler.h"

// ----------------------------------------- Utils --------------------------------------------

/**
 * Prints a horizontal line of dashes for UI separation.
 */
void printLine()
{
    for (int i = 0; i < 40; i++)
    {
        printf("-");
    }
    printf("\n");
}

// --------------------------------loaded-database menu ----------------------------------------

/**
 * Adds a new entry to the Wallet by prompting user input.
 */
void enterEntry(Wallet *wallet)
{
    // Reading the new entry from user input
    Entry *entry = (Entry *)malloc(sizeof(Entry));
    if (!entry)
    {
        printf("Error: Not able to add the new entry to the wallet. Try again.\n");
        return;
    }
    char type;
    // Get the date
    printf("To add a new income or expense, follow the following steps.\n");
    printf("1. Enter the date in the following format: YYYY MM DD \n");
    int year, month, day;
    scanf("%d %d %d", &year, &month, &day);
    entry->date = normalize(year, month, day);
    // Get the category
    printf("2. Enter the category: \n");
    scanf("%49s", entry->categoryId);
    // Get the amount
    printf("3. Enter the amount of the new entry: \n");
    scanf("%lf", &entry->amount);
    if (entry->amount > 0)
    {
        entry->type = Income;
    }
    else if (entry->amount < 0)
    {
        entry->type = Expense;
    }
    // Get the currency
    printf("4. Enter the currency (3 letters max):\n");
    scanf("%3s", entry->currency);
    // Storing the new entry in the wallet
    addEntry(wallet, entry);
}

/**
 * Adds a new category to the Wallet by prompting user input.
 */
void enterCategory(Wallet *wallet)
{
    Category *category = (Category *)malloc(sizeof(Category));
    if (!category)
        return;
    printf("Enter the name of the new category: \n");
    scanf("%49s", category->name);
    category->total = 0;
    addCategory(wallet, category);
}

/**
 * Evaluates the wallet's totals and highest expense category in a chosen currency.
 */
void evaluateTotalDb(Wallet *wallet)
{
    // About the category with the highest expenses
    char currency[4];
    Category *expCategory;
    // About the total evaluation
    double totalExpenses = 0;
    double totalIncomes = 0;
    if (wallet->categoriesSize == 0)
    {
        printf("Error: Database can not be evaluated. There are no categories.\n");
        return;
    }
    printf("Enter the currency to evaluate the wallet.\n");
    scanf("%3s", currency);
    if (!isCurrency(currency))
    {
        printf("Error: %s is not a supported currency", currency);
        return;
    }
    printLine();
    // All totals are stored in EUR.
    expCategory = wallet->categories[0];
    for (int i = 0; i < wallet->categoriesSize; i++)
    {
        if (wallet->categories[i]->total < expCategory->total)
        {
            expCategory = wallet->categories[i];
        }
    }
    printf("Category with the highest expense:\n%s\n", expCategory->name);
    printf("Amount: %28.2f %s\n", transformIntoCurrency(expCategory->total, "EUR", currency), currency);
    printLine();
    for (int i = 0; i < wallet->size; i++)
    {
        if (wallet->entries[i]->type == Income)
        {
            totalIncomes += transformIntoCurrency(wallet->entries[i]->amount, wallet->entries[i]->currency, currency);
        }
        else
        {
            totalExpenses += transformIntoCurrency(wallet->entries[i]->amount, wallet->entries[i]->currency, currency);
        }
    }
    double balance = totalExpenses + totalIncomes;
    printf("Total income: %22.2f %s\n", totalIncomes, currency);
    printf("Total expense: %21.2f %s\n", totalExpenses, currency);
    printLine();
    printf("Current balance: %19.2f %s\n", balance, currency);
}

/**
 * Displays income, expense, largest expense, and balance for each category during a date range.
 */
void displayTimePeriod(Wallet *wallet)
{
    // Get the time period
    printf("Enter the start date (format: YYYY MM DD)\n");
    int startYear, startMonth, startDay;
    scanf("%d %d %d", &startYear, &startMonth, &startDay);
    struct tm start = normalize(startYear, startMonth, startDay);
    printf("Enter the end date (format: YYYY MM DD)\n");
    int endYear, endMonth, endDay;
    scanf("%d %d %d", &endYear, &endMonth, &endDay);
    struct tm end = normalize(endYear, endMonth, endDay);
    if (!isDateValid(start) || !isDateValid(end))
        return;
    // Get the currency in which the statistics should be displayed
    char currency[4];
    printf("Enter the currency to display the statistics: \n");
    scanf("%s", currency);
    if (!isCurrency(currency))
        return;
    // Evaluate each category
    double expense, income, largestExpense;
    int count;
    for (int i = 0; i < wallet->categoriesSize; i++)
    {
        expense = 0;
        income = 0;
        largestExpense = 0;
        count = 0;
        for (int j = 0; j < wallet->size; j++)
        {
            Entry entry = *wallet->entries[j];
            // Check whether the entry is in the right time interval and whether it belongs to the right catgory
            if (isDateBetween(entry.date, start, end) && (strcmp(wallet->categories[i]->name, entry.categoryId) == 0))
            {
                if (entry.type == Income)
                {
                    income += transformIntoCurrency(entry.amount, entry.currency, currency);
                }
                else
                {
                    expense += transformIntoCurrency(entry.amount, entry.currency, currency);
                }
                if (transformIntoCurrency(entry.amount, entry.currency, currency) < largestExpense)
                {
                    largestExpense = transformIntoCurrency(entry.amount, entry.currency, currency);
                }
                count++;
            }
        }
        printLine();
        printf("%s\n", wallet->categories[i]->name);
        if (count == 0)
        {
            printf("No entry in this category.\n");
        }
        else
        {
            printf("Income: %28.2f %s\n", income, currency);
            printf("Expense: %27.2f %s\n", expense, currency);
            if (largestExpense == 0)
            {
                printf("Largest Expense:              ---.-- ---\n");
            }
            else
            {
                printf("Largest Expense: %19.2f %s\n", largestExpense, currency);
            }
            printf("Balance: %27.2f %s\n", income + expense, currency);
        }
    }
}

/**
 * Main loop to handle user menu input and perform wallet operations.
 */
void handleInput(char *path, Wallet *wallet)
{
    int choice;
    int exit = 0;
    while (1)
    {
        printLine();
        printf("Enter  \n1 to add a new entry. \n2 to create a new category. \n3 to evaluate wallet in total. \n4 to display statistics of each category. \n5 to save the wallet. \n9 to close the wallet. \n");
        scanf("%d", &choice);
        switch (choice)
        {
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
            if (exit)
                freeWallet(wallet);
            break;
        default:
            printf("There is no action for the given input. Choose another action!\n");
            break;
        }
        if (exit)
            break;
    }
}