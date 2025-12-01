#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DateHandler.h"
#include "WalletHandler.h"
#include "FileIO.h"
#include "InputHandler.h"

// ------------------------------------------ main ---------------------------------------------

/**
 * Program entry point. Allows loading wallet, running commands, and exiting.
 * @return Program exit code.
 */
int main(void)
{
    char path[100];
    Wallet *wallet = (Wallet *)malloc(sizeof(Wallet));
    if(!wallet) return 1;
    printf("Welcome to your WALLET :)\n");
    while (1)
    {
        printLine();
        printf("To use the WALLET enter the path to an existing database.\n                   OR\nTo exit the program enter \"exit\".\n");
        scanf("%s", path);
        // To exit the program
        if (strcmp(path, "exit") == 0)
        {
            break;
        }
        // Open the file
        if (openFile(path, wallet))
        {
            printf("The database is loaded.\n");
            handleInput(path, wallet);
        }
        else
        {
            printf("Error: During opening the database. Please try again.\n");
        }
    }
    return 0;
}