#include <stdio.h>
#include <string.h>
#include <time.h>

typedef enum entry_type {
    Income, Expense
} entry_type;

typedef struct Entry {
    time_t date;
    entry_type type;
    char category[50];
    int amount;
    char currency[4];
} Entry;

int openFile(char path[]) {
    // TODO Handle the file
    return 1;
}

void addEntry(char arg[]) {
    Entry newEntry;
    printf("To enter a new Entry, follow this format: \nyear:month:day type category amount currency\n");
}

void saveDb(char arg[]) {
    // TODO close file
}

void createCategory(char arg[]) {
    char newCategory[50];
    printf("Enter the name of the new category: \n");
    scanf("%s", newCategory);
    // TODO add category and check whether the Category already exist
}

void evaluateTotalDb(char arg[]) {

}

void displayTimePeriod(char arg[]) {

}

void handleInput(char arg[]) {
    int choice;
    int exit = 0;
    while(1) {
        printf("Enter  \n1 to add a new entry. \n2 to create a new catergory. \n3 to evaluate the database in total. \n4 to display all entries in a given time period. \n5 to save the database. \n9 to close the database. \n");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                addEntry(arg);
                break;
            case 2:
                createCategory(arg);
                break;
            case 3:
                evaluateTotalDb(arg);
                break;
            case 4:
                displayTimePeriod(arg);
                break;
            case 5:
                saveDb(arg);
                break;
            case 9: 
                saveDb(arg);
                exit = 1;
            default:
                break;
        }
        if (exit) break;
    }
}

int main(void) {
    char path[50];
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
        if (openFile(path)) {
            printf("The database is loaded.\n");
            handleInput(path);
        } else {
            printf("An error occured during opening the database. Please try again.\n");
        }
    }
    return 0;
}