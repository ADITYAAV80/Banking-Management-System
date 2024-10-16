#ifndef LOAN_STRUCT
#define LOAN_STRUCT
#include <stdbool.h>

struct loan_struct
{
    int ln_no;
    char customer_id[20];
    char employee_id[20];
    int amount;
    bool approved;
};

#endif