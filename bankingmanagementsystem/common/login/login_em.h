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

bool login_employee(int connectionFileDescriptor, char *employee_id, char *passBuffer);
bool employee_password_checker(char *employee_id, char *passBuffer);

bool login_employee(int connectionFileDescriptor, char *employee_id, char *passBuffer)
{
    if (employee_password_checker(employee_id, passBuffer))
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

    struct employee_struct employee;

    while (read(fileDescriptor, &employee, sizeof(struct employee_struct)) == sizeof(struct employee_struct))
    {
        if (strcmp(employee.login, login_id) == 0)
        {

            lock.l_type = F_RDLCK;                       // Read lock
            lock.l_whence = SEEK_CUR;                    // Start from the beginning of the file
            lock.l_start = 0;                            // Offset 0
            lock.l_len = sizeof(struct employee_struct); // Lock the entire file

            if (strcmp(employee.password, password) == 0)
            {
                printf("Password match\n");
                add_user(login_id);
                print_logged_in_users();
                // unlocking
                lock.l_type = F_UNLCK;
                if (fcntl(fileDescriptor, F_SETLK, &lock) == -1)
                {
                    perror("Error releasing the lock");
                }
                close(fileDescriptor);
                return true;
            }
            else
            {
                lock.l_type = F_UNLCK;
                if (fcntl(fileDescriptor, F_SETLK, &lock) == -1)
                {
                    perror("Error releasing the lock");
                }
            }
        }
    }
    close(fileDescriptor);
    return false;
}

#endif