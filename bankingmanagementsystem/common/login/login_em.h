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

            // Get the position of the employee record
            off_t employee_record_pos = lseek(fileDescriptor, 0, SEEK_CUR) - sizeof(struct employee_struct);

            // Set up the read lock on the specific employee record
            lock.l_type = F_RDLCK;                       // Read lock
            lock.l_whence = SEEK_SET;                    // Use absolute position
            lock.l_start = employee_record_pos;          // Lock at the position of the employee record
            lock.l_len = sizeof(struct employee_struct); // Lock the size of the employee record

            // HASHING
            // char *hashed_input_password = crypt(password, HASH);
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