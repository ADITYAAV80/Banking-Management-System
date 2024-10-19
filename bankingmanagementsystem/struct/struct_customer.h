#ifndef CUSTOMER_STRUCT
#define CUSTOMER_STRUCT
#include <stdbool.h>

struct customer_struct
{
    int cus_no;
    bool active;
    char name[30];
    char age[10];
    char address[50];
    char email[20];
    char login[20];
    char password[120];
    int balance;
};

#endif