#ifndef LOGIN_EMPLOYEE
#define LOGIN_EMPLOYEE

#include <stdio.h>     //SEEKSET
#include <stdbool.h>   // Import for `bool` data type
#include <sys/types.h> // Import for `open`, `lseek`
#include <sys/stat.h>  // Import for `open`
#include <fcntl.h>     // Import for `open`
#include <stdlib.h>    // Import for `atoi`
#include <errno.h>     // Import for `errno`
#include <string.h>    //memset
#include <unistd.h>    //read, close

#include "../../functions/server_const.h"
#include "../../struct/struct_employee.h"

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
    int fileDescriptor = open("EMPLOYEE_FILE", O_RDONLY);

    if (fileDescriptor == -1)
    {
        perror("Error opening file");
        return false;
    }

    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_RDLCK;    // Read lock
    lock.l_whence = SEEK_SET; // Start from the beginning of the file
    lock.l_start = 0;         // Offset 0
    lock.l_len = 0;           // Lock the entire file

    // Try to acquire the lock in blocking mode
    if (fcntl(fileDescriptor, F_SETLKW, &lock) == -1)
    {
        perror("Error locking the file");
        close(fileDescriptor);
        exit(EXIT_FAILURE);
    }
    struct employee_struct employee;

    while (read(fileDescriptor, &employee, sizeof(struct employee_struct)) == sizeof(struct employee_struct))
    {
        if (strcmp(employee.login, login_id) == 0)
        {
            if (strcmp(employee.password, password) == 0)
            {
                printf("Password match\n");
                // unlocking
                lock.l_type = F_UNLCK;
                if (fcntl(fileDescriptor, F_SETLK, &lock) == -1)
                {
                    perror("Error releasing the lock");
                }
                close(fileDescriptor);
                return true;
            }
        }
    }
    // unlocking
    lock.l_type = F_UNLCK;
    if (fcntl(fileDescriptor, F_SETLK, &lock) == -1)
    {
        perror("Error releasing the lock");
    }
    close(fileDescriptor);
    return false;
}

#endif