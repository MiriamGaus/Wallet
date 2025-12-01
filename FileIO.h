#ifndef FILEIO_H
#define FILEIO_H

#include <stdio.h>
#include "WalletHandler.h"

/**
 * Reads wallet data from a file and populates the Wallet struct.
 * @param file Pointer to the already opened input file.
 * @param wallet Pointer to the Wallet to populate.
 * @return 1 if successful, 0 on failure.
 */
int loadWalletFromFile(FILE *file, Wallet *wallet);

/**
 * Writes the Wallet data into a file.
 * @param file Pointer to the already opened output file.
 * @param wallet Pointer to the Wallet to write.
 */
void saveWalletToFile(FILE *file, Wallet *wallet);

/**
 * Opens a wallet database file, loads its data into Wallet struct.
 * @param path Path to the database file.
 * @param wallet Pointer to the Wallet struct to load into.
 * @return 1 if file loaded successfully, 0 otherwise.
 */
int openFile(char *path, Wallet *wallet);

/**
 * Saves the Wallet data to a file at the specified path.
 * @param path Path to save file.
 * @param wallet Pointer to the Wallet struct.
 * @return 1 if successful save, 0 otherwise.
 */
int saveDb(char *path, Wallet *wallet);
#endif