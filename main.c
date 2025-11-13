#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum entry_type {
    Income, Expense
} entry_type;

typedef struct Entry {
    struct tm date;
    entry_type type;
    char category[50];
    double amount;
    char currency[4];
} Entry;

typedef struct Wallet {
    Entry **entries;
    int size;
    char **categories;
    int categoriesSize;
} Wallet;

// ----------------------------------------- Utils --------------------------------------------
void printEntry(Entry entry) {
    const char *typeStr;
    switch (entry.type) {
        case Income: typeStr = "Income"; break;
        case Expense: typeStr = "Expense"; break;
        default: typeStr = "Unknown"; break;
    }
    printf("%02d.%02d.%04d %s %s %.2f %s\n", entry.date.tm_mday, entry.date.tm_mon, entry.date.tm_year,
        typeStr, entry.category, entry.amount, entry.currency);
}


/*
 * Free the dynamically allocated memory of the wallet
 * @param Wallet *wallet
 */
void freeWallet(Wallet *wallet) {
    if(!wallet) return;
    for(int i = 0; i < wallet->categoriesSize; i++) {
        free(wallet->categories[i]);
        wallet->categories[i] = NULL;
    }
    free(wallet->categories);
    wallet->categories = NULL;
    for(int i = 0; i < wallet->size; i++) {
        free(wallet->entries[i]);
        wallet->entries[i] = NULL;
    }
    free(wallet->entries);
    wallet->entries = NULL;
    free(wallet);
    wallet = NULL;
}

int isCategory(Wallet *wallet, char *category) {
    for(int i = 0; i < wallet->categoriesSize; i++) {
        if(strcmp(wallet->categories[i], category) == 0) {
            return 1;
        }
    }
    return 0;
}

int isCurrency(char *currency) {
    if(!strcmp(currency, "EUR") || !strcmp(currency, "HUF") || !strcmp(currency, "USD")) return 1;
    printf("Error: %s is not a supported currency.\n", currency);
    return 0;
}

int isDateValid(struct tm date) {
    struct tm copy = date;         
    time_t t = mktime(&copy);       
    if (t == -1) return 0;
    if(date.tm_year == copy.tm_year && date.tm_mon == copy.tm_mon && date.tm_mday == copy.tm_mday) {
        return 1;
    }
    printf("Incorrect date %02d.%02d.%04d\n", date.tm_mday, date.tm_mon, date.tm_year);
    return 0;
}

int transformIntoWallet(FILE *file, Wallet *wallet) {
    // Allocating memory for the entries
    fscanf(file, "%d", &wallet->categoriesSize);
    wallet->categories = (char**) malloc(sizeof(char*) * wallet->categoriesSize);
    if(!wallet->categories) return 0;
    char *category;
    for(int i = 0; i < wallet->categoriesSize; i++) {
        category = (char*) malloc(sizeof(char) * 50);
        if(!category) {
            freeWallet(wallet);
            return 0;
        }
        fscanf(file, "%49s", category);
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
        char entrytype[10];
        struct tm date = {0};
        entry->date = date;
        fscanf(file, "%d.%d.%d", &entry->date.tm_mday, &entry->date.tm_mon, &entry->date.tm_year);
        entry->date.tm_isdst = -1;
        fscanf(file, "%s", entry->category);
        fscanf(file, "%lf", &entry->amount);
        fscanf(file, "%s", entry->currency);
        if(entry->amount >= 0) {
            entry->type = Income;
        } else {
            entry->type = Expense;
        }
        // Check if input entry is correct
        if(!isDateValid(entry->date) || !isCategory(wallet, entry->category) || !isCurrency(entry->currency)) {
            printf("Entry %d contains invalid data. File can not be read.\n", i);
            freeWallet(wallet);
            return 0;
        }
        wallet->entries[i] = entry;
    }
    return 1;
}

void transformIntoFile(FILE *file, Wallet *wallet) {
    // Writing the number of categories into the file and writing the categories aswell
    fprintf(file, "%d\n", wallet->categoriesSize);
    for(int i = 0; i < wallet->categoriesSize; i++) {
        fprintf(file, "%s\n", wallet->categories[i]);
    }
    // Writing the number of entries into the file and writing the entries aswell
    fprintf(file, "%d\n", wallet->size);
    Entry *entry;
    for(int i = 0; i < wallet->size; i++) {
        entry = wallet->entries[i];
        fprintf(file, "%d.%d.%d\n", entry->date.tm_mday, entry->date.tm_mon, entry->date.tm_year);
        fprintf(file, "%s\n", entry->category);
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

void addEntry(Wallet *wallet) {
    // Reading the new entry from user input
    Entry *entry = (Entry*) malloc(sizeof(Entry));
    if(!entry) {
        printf("There was an error while adding the new entry to the wallet. Try again.\n");
        return;
    }
    char type;
    // Get the date
    printf("To add a new income or expense, follow the following steps.\n");
    printf("1. Enter the date in the following format: YYYY MM DD \n");
    struct tm date = {0};
    entry->date = date;
    scanf("%d %d %d", &entry->date.tm_year, &entry->date.tm_mon, &entry->date.tm_mday);
    if(!isDateValid(entry->date)) {
        printf("There was an error while adding the new entry to the wallet. Try again.\n");
        return;
    }
    // Get the category
    printf("2. Enter the category: \n");
    scanf("%49s", entry->category);
    if(!isCategory(wallet, entry->category)) {
        printf("There was an error while adding the new entry to the wallet. Try again.\n");
        return;
    }
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
    if(!isCurrency(entry->currency)) {
        printf("There was an error while adding the new entry to the wallet. Try again.\n");
        return;
    }
    // Storing the new entry in the wallet
    // TODO does not work correctly in storing 
    Entry **newEntries = realloc(wallet->entries, (wallet->size + 1) * sizeof(Entry *));
    if (newEntries == NULL) {
        printf("There was an error while adding the new entry to the wallet. Try again.\n");
        return;
    }
    wallet->entries = newEntries;
    wallet->entries[wallet->size] = entry;
    wallet->size++;
    printf("The new entry successfully added to the wallet: \n");
    printEntry(*entry);
}

int saveDb(char *path, Wallet *wallet) {
    // TODO Error messages if not successful
    FILE *file = fopen(path, "w");
    if (file == NULL) return 0;
    transformIntoFile(file, wallet);
    fclose(file);
    return 1;
}

void createCategory(Wallet *wallet) {
    char *category = (char*) malloc(sizeof(char) * 50);
    if(!category) return;
    printf("Enter the name of the new category: \n");
    scanf("%49s", category);
    if(isCategory(wallet, category)) {
        printf("This category already exits.\n");
        free(category);
        return;
    }
    char **categories;
    categories = (char**) realloc(wallet->categories, (wallet->categoriesSize + 1) * sizeof(char*));
    if(!categories) return;
    wallet->categories[wallet->categoriesSize] = category;
    wallet->categoriesSize++;
    printf("The new category is successfully added.\n");
}

void evaluateTotalDb(Wallet *wallet) {

}

void displayTimePeriod(Wallet *wallet){

}

void handleInput(char *path, Wallet *wallet) {
    int choice;
    int exit = 0;
    while(1) {
        for (int i = 0; i < 40; i++) {
            printf("-");
        }
        printf("\n");
        printf("Enter  \n1 to add a new entry. \n2 to create a new catergory. \n3 to evaluate the database in total. \n4 to display all entries in a given time period. \n5 to save the database. \n9 to close the database. \n");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                addEntry(wallet);
                break;
            case 2:
                createCategory(wallet);
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
        for (int i = 0; i < 40; i++) {
            printf("-");
        }
        printf("\n");
        printf("To use the WALLET enter the path to an existing database.\n              OR\nTo exit the program enter \"exit\".\n");
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
            printf("An error occured during opening the database. Please try again.\n");
        }
    }
    return 0;
}