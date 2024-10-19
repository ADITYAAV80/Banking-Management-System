#ifndef LOGIN_MANAGER
#define LOGIN_MANAGER

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
#include "../../struct/struct_manager.h"

bool login_manager(int connectionFileDescriptor, char *manager_id, char *passBuffer);
bool manager_password_checker(char *manager_id, char *passBuffer);

bool login_manager(int connectionFileDescriptor, char *manager_id, char *passBuffer)
{
    if (manager_password_checker(manager_id, passBuffer))
    {
        return true;
    }
    return false;
}

bool manager_password_checker(char *login_id, char *password)
{
    int fileDescriptor = open("MANAGER_FILE", O_RDONLY);

    if (fileDescriptor == -1)
    {
        perror("Error opening file");
        return false;
    }

    struct flock lock;
    memset(&lock, 0, sizeof(lock));

    struct manager_struct manager;

    while (read(fileDescriptor, &manager, sizeof(struct manager_struct)) == sizeof(struct manager_struct))
    {
        if (strcmp(manager.login, login_id) == 0)
        {
            // Get the position of the employee record
            off_t manager_record_pos = lseek(fileDescriptor, 0, SEEK_CUR) - sizeof(struct manager_struct);

            // Set up the read lock on the specific employee record
            lock.l_type = F_RDLCK;                      // Read lock
            lock.l_whence = SEEK_SET;                   // Use absolute position
            lock.l_start = manager_record_pos;          // Lock at the position of the employee record
            lock.l_len = sizeof(struct manager_struct); // Lock the size of the employee record

            // Try to acquire the lock in blocking mode
            if (fcntl(fileDescriptor, F_SETLKW, &lock) == -1)
            {
                perror("Error locking the file");
                close(fileDescriptor);
                exit(EXIT_FAILURE);
            }

            // HASHING
            // char *hashed_input_password = crypt(password, HASH);
            if (strcmp(manager.password, password) == 0)
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