#ifndef EMPLOYEE_STRUCT
#define EMPLOYEE_STRUCT
#include <stdbool.h>

struct employee_struct
{
    int emp_no;
    bool active;
    char name[30];
    char age[10];
    char address[50];
    char email[20];
    char login[20];
    char password[120];
};

#endif