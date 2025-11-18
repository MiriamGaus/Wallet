#ifndef WALLETHANDLER_H
#define WALLETHANDLER_H

typedef enum entry_type {
    Income, Expense
} entry_type;

typedef struct Category {
    char name[50];
    double total;
} Category;

typedef struct Entry {
    struct tm date;
    entry_type type;
    char categoryId[50];
    double amount;
    char currency[4];
} Entry;

typedef struct Wallet {
    Entry **entries;
    int size;
    Category **categories;
    int categoriesSize;
} Wallet;

static struct {
        char pair[9];
        double rate;
    } exchangeRates[] = {
        {"EUR->HUF", 385.0},
        {"HUF->EUR", 0.0026},
        {"EUR->USD", 1.15},
        {"USD->EUR", 0.87},
        {"HUF->USD", 330},
        {"USD->HUF", 1/330}
};

void printEntry(Entry entry);
int isCategory(Wallet *wallet, char *category);
int isCurrency(char *currency);
double transformIntoCurrency(double amount, char *oldCurr, char *newCurr);
void addAmountToCategory(double amount, char *categoryId, Wallet *wallet);
void addEntry(Wallet *wallet, Entry *entry);
void addCategory(Wallet *wallet, Category *category);
void freeWallet(Wallet *wallet);

#endif