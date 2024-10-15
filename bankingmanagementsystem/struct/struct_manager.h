#ifndef MANAGER_STRUCT
#define MANAGER_STRUCT
#include <stdbool.h>

struct manager_struct
{
    int mng_no;
    bool active;
    char name[30];
    char age[10];
    char address[50];
    char email[20];
    char login[20];
    char password[10];
};

#endif