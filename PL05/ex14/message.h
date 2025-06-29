#ifndef MESSAGE_H
#define MESSAGE_H

#define MESSAGE 128
#define LOOP 10

typedef struct {
    char message[MESSAGE];
    int readerCount;
    int writerCount;
} string_t;

#endif
