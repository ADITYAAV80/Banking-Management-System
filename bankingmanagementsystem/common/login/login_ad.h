#ifndef LOGIN_ADMIN
#define LOGIN_ADMIN

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
#include "../../struct/struct_admin.h"
#include "login_common.h"

bool login_admin(int connectionFileDescriptor, char *admin_id, char *passBuffer);

bool admin_password_checker(char *admin_id, char *passBuffer);

bool login_admin(int connectionFileDescriptor, char *admin_id, char *passBuffer)
{
    if (admin_password_checker(admin_id, passBuffer))
    {
        return true;
    }
    return false;
}

bool admin_password_checker(char *login_id, char *password)
{
    if (strcmp(login_id, "admin") == 0)
    {
        if (strcmp(password, ADMIN_PASSWORD___) == 0)
        {
            printf("Superuser authenticated\n");
            add_user(login_id); // Add user to logged-in users
            print_logged_in_users();
            return true;
        }
        else
        {
            printf("Superuser password mismatch\n");
            return false; // Password didn't match for superuser
        }
    }

    int fileDescriptor = open("ADMIN_FILE", O_RDONLY);

    if (fileDescriptor == -1)
    {
        perror("Error opening file");
        return false;
    }

    struct admin_struct admin;

    while (read(fileDescriptor, &admin, sizeof(struct admin_struct)) == sizeof(struct admin_struct))
    {
        if (strcmp(admin.login, login_id) == 0)
        {
            struct flock lock;
            memset(&lock, 0, sizeof(lock));
            lock.l_type = F_RDLCK;                    // Read lock
            lock.l_whence = SEEK_CUR;                 // Start from the beginning of the file
            lock.l_start = 0;                         // Offset 0
            lock.l_len = sizeof(struct admin_struct); // Lock the entire file

            // Try to acquire the lock in blocking mode
            if (fcntl(fileDescriptor, F_SETLKW, &lock) == -1)
            {
                perror("Error locking the file");
                close(fileDescriptor);
                exit(EXIT_FAILURE);
            }

            if (strcmp(admin.password, password) == 0)
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
        }
    }
    close(fileDescriptor);
    return false;
}

#endif
