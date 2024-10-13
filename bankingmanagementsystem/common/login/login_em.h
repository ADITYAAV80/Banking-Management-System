#ifndef LOGIN_EMPLOYEE
#define LOGIN_EMPLOYEE

#include <stdio.h>     //SEEKSET
#include <stdbool.h>   // Import for `bool` data type
#include <sys/types.h> // Import for `open`, `lseek`
#include <sys/stat.h>  // Import for `open`
#include <fcntl.h>     // Import for `open`
#include <stdlib.h>    // Import for `atoi`
#include <errno.h>     // Import for `errno`
#include "../../functions/server_const.h"
#include "../password_check.h"

bool login_employee(int connectionFileDescriptor, char *student_id, char *passBuffer);
bool employee_password_checker(char *student_id, char *passBuffer);

bool login_employee(int connectionFileDescriptor, char *student_id, char *passBuffer)
{
    if (employee_password_checker(student_id, passBuffer))
    {
        return true;
    }
    return false;
}

bool employee_password_checker(char *login_id, char *password)
{
    int fileDescriptor = open("employee.txt", O_RDONLY);

    if (fileDescriptor == -1)
    {
        perror("Error opening file");
        return false;
    }

    printf("%d", fileDescriptor);
}

#endif