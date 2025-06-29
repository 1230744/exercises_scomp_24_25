#ifndef SHM_H
#define SHM_H

#define MAXLENGH 256

typedef struct
{
    int access;
    int integer;
    char message[MAXLENGH];
} message;

#endif