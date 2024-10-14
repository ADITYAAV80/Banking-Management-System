#ifndef LOGIN_ADMIN
#define LOGIN_ADMIN

#include <stdio.h>     // Import for `printf` & `perror`
#include <unistd.h>    // Import for `read`, `write & `lseek`
#include <string.h>    // Import for string functions
#include <stdbool.h>   // Import for `bool` data type
#include <sys/types.h> // Import for `open`, `lseek`
#include <sys/stat.h>  // Import for `open`
#include <fcntl.h>     // Import for `open`
#include <stdlib.h>    // Import for `atoi`
#include <errno.h>     // Import for `errno`

#include "../../functions/server_const.h"

bool login_admin(int connectionFileDescriptor);

bool login_admin(int connectionFileDescriptor)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];
    char tempBuffer[1000];

    bzero(readBuffer, sizeof(readBuffer));
    bzero(writeBuffer, sizeof(writeBuffer));

    strcpy(writeBuffer, ADMIN_LOGIN_WELCOME);
    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error in writing\n");
        return false;
    }

    readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error in reading\n");
        return false;
    }

    char admin_id[20];
    strcpy(admin_id, readBuffer);

    bzero(readBuffer, sizeof(readBuffer));
    bzero(writeBuffer, sizeof(writeBuffer));

    strcpy(writeBuffer, ADMIN_PASSWORD_DISPLAY);
    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error in writing\n");
        return false;
    }

    readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error in reading\n");
        return false;
    }

    printf("%s", admin_id);
    printf("%s\n", readBuffer);

    if ((strcmp(admin_id, "admin") == 0) && strcmp(readBuffer, ADMIN_PASSWORD) == 0)
    {
        printf("Inside authentication\n");
        return true;
    }
    else
    {
        return false;
    }
}

#endif