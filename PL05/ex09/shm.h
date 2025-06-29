#ifndef SHM_H
#define SHM_H

#include <semaphore.h>

typedef struct {
    int number;         // Bank account number
    double amount;      // Account balance
    sem_t semaphore;    // Semaphore for synchronizing access
} account;

#define SIZE sizeof(account)

#endif // SHM_H