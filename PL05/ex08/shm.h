#ifndef SHM_H
#define SHM_H

#define NUMBER_OF_TICKETS 10 //para não estar em loop infinito, mas na realidade seria
#define SIZE sizeof(ticket_t)

typedef struct
{
    int number;
} ticket_t;

#endif // SHM_H