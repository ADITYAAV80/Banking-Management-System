#ifndef TRANSACTION_STRUCT
#define TRANSACTION_STRUCT

struct transaction_struct
{

    int t_id;
    char type[2];
    int amount;
    char source_account[20];
    char destination_account[20];
    int balance_after;
};

#endif