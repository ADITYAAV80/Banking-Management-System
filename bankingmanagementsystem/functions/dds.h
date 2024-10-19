bool activate_managers_admin(int connectionFileDescriptor, char *admin_id)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];
    // opening the file in Read Write Mode
    int managerFileDescriptor = open("MANAGER_FILE", O_RDWR, 0777);
    if (managerFileDescriptor == -1)
    {
        perror("Error while opening manager file");
        return false;
    }

    struct flock lock;
    memset(&lock, 0, sizeof(lock));

    struct manager_struct manager1;
    char managerlist[1000];
    managerlist[0] = '\0';

    // getting all the managers ID to choose from
    while (read(managerFileDescriptor, &manager1, sizeof(struct manager_struct)) == sizeof(struct manager_struct))
    {
        if (manager1.active == 0)
        {
            lock.l_type = F_RDLCK;
            lock.l_whence = SEEK_CUR;
            lock.l_start = -sizeof(struct manager_struct);
            lock.l_len = sizeof(struct manager_struct);

            // Try to acquire the lock in blocking mode
            if (fcntl(managerFileDescriptor, F_SETLKW, &lock) == -1)
            {
                perror("Error locking the file");
                close(managerFileDescriptor);
                exit(EXIT_FAILURE);
            }

            char tempBuffer[1000]; // Temporary buffer to construct the string
            sprintf(tempBuffer, "Login: %s ", manager1.login);
            strcat(managerlist, tempBuffer);
            sprintf(tempBuffer, "Name: %s ", manager1.name);
            strcat(managerlist, tempBuffer);
            strcat(managerlist, "\n");

            // Unlock the current manager record after reading
            lock.l_type = F_UNLCK; // Unlock
            if (fcntl(managerFileDescriptor, F_SETLK, &lock) == -1)
            {
                perror("Error unlocking the manager record");
            }
        }
    }
    lseek(managerFileDescriptor, 0, SEEK_SET);

    strcpy(writeBuffer, managerlist);
    strcat(writeBuffer, MANAGER_ACTIVE);

    bzero(managerlist, sizeof(managerlist));
    managerlist[0] = '\0';

    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));

    if (writeBytes == -1)
    {
        perror("Error while writing to file!");
        return false;
    }
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    char login[20];

    strcpy(login, readBuffer);

    bzero(writeBuffer, sizeof(writeBuffer));
    bzero(readBuffer, sizeof(readBuffer));

    // searching the login id and activating them
    while (read(managerFileDescriptor, &manager1, sizeof(struct manager_struct)) == sizeof(struct manager_struct))
    {
        if (strcmp(manager1.login, login) == 0)
        {
            manager1.active = true;
            lseek(managerFileDescriptor, -1 * sizeof(struct manager_struct), SEEK_CUR);
            write(managerFileDescriptor, &manager1, sizeof(struct manager_struct));
            strcpy(writeBuffer, MANAGER_ACTIVATED);
            writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));

            lock.l_type = F_WRLCK;    // Write lock
            lock.l_whence = SEEK_CUR; // Start from the current position of the file
            lock.l_start = 0;         // Offset 0
            lock.l_len = sizeof(struct manager_struct);

            // Try to acquire the lock in blocking mode
            if (fcntl(managerFileDescriptor, F_SETLKW, &lock) == -1)
            {
                perror("Error locking the file");
                close(managerFileDescriptor);
                exit(EXIT_FAILURE);
            }

            readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
            // unlocking
            lock.l_type = F_UNLCK;
            if (fcntl(managerFileDescriptor, F_SETLK, &lock) == -1)
            {
                perror("Error releasing the lock");
            }
            close(managerFileDescriptor);
            return true;
        }
    }
    // if id not found in blocked list
    strcpy(writeBuffer, MANAGER_NOT_ACTIVATED);

    close(managerFileDescriptor);
    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    return false;
}
