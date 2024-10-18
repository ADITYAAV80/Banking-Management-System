#ifndef LOGIN_CUSTOMER
#define LOGIN_CUSTOMER

#include <stdio.h>     //SEEKSET
#include <stdbool.h>   // Import for `bool` data type
#include <sys/types.h> // Import for `open`, `lseek`
#include <sys/stat.h>  // Import for `open`
#include <fcntl.h>     // Import for `open`
#include <stdlib.h>    // Import for `atoi`
#include <errno.h>     // Import for `errno`
#include <string.h>    //memset
#include <unistd.h>    //read, close
#include <crypt.h>

#include "../../functions/server_const.h"
#include "../../struct/struct_customer.h"
#include "login_common.h"

bool login_customer(int connectionFileDescriptor, char *customer_id, char *passBuffer);

bool customer_password_checker(char *customer_id, char *passBuffer);

bool login_customer(int connectionFileDescriptor, char *customer_id, char *passBuffer)
{
    if (customer_password_checker(customer_id, passBuffer))
    {
        return true;
    }
    return false;
}

bool customer_password_checker(char *login_id, char *password)
{

    int fileDescriptor = open("CUSTOMER_FILE", O_RDONLY);

    if (fileDescriptor == -1)
    {
        perror("Error opening file");
        return false;
    }
    struct flock lock;
    memset(&lock, 0, sizeof(lock));

    struct customer_struct customer;

    while (read(fileDescriptor, &customer, sizeof(struct customer_struct)) == sizeof(struct customer_struct))
    {
        if (strcmp(customer.login, login_id) == 0)
        {

            // Get the position of the customer record
            off_t customer_record_pos = lseek(fileDescriptor, 0, SEEK_CUR) - sizeof(struct customer_struct);

            // Set up the read lock on the specific customer record
            lock.l_type = F_RDLCK;                       // Read lock
            lock.l_whence = SEEK_SET;                    // Use absolute position
            lock.l_start = customer_record_pos;          // Lock at the position of the customer record
            lock.l_len = sizeof(struct customer_struct); // Lock the size of the customer record

            // Try to acquire the lock in blocking mode
            if (fcntl(fileDescriptor, F_SETLKW, &lock) == -1)
            {
                perror("Error locking the file");
                close(fileDescriptor);
                exit(EXIT_FAILURE);
            }

            char *hashed_input_password = crypt(password, HASH);
            if (strcmp(hashed_input_password, customer.password) == 0)
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