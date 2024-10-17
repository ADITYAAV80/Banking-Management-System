bool change_password_manager(int connectionFileDescriptor, char *manager_id)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];
    int managerFileDescriptor = open("MANAGER_FILE", O_RDWR, 0777);
    if (managerFileDescriptor == -1)
    {
        perror("Error while opening file");
        return false;
    }

    struct manager_struct manager1;
    while (read(managerFileDescriptor, &manager1, sizeof(struct manager_struct)) == sizeof(struct manager_struct))
    {
        if (strcmp(manager1.login, manager_id) == 0)
        {
            lseek(managerFileDescriptor, -1 * sizeof(struct manager_struct), SEEK_CUR);

            // to update password
            bzero(writeBuffer, sizeof(writeBuffer));
            bzero(readBuffer, sizeof(readBuffer));

            strcpy(writeBuffer, S_MANAGER_CHANGE_PASSWORD);

            writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));

            strcpy(manager1.password, readBuffer);

            struct flock lock;
            memset(&lock, 0, sizeof(lock));
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

            write(managerFileDescriptor, &manager1, sizeof(struct manager_struct));

            lock.l_type = F_UNLCK; // to unlock file
            if (fcntl(managerFileDescriptor, F_SETLK, &lock) == -1)
            {
                perror("Error releasing the lock");
            }
            close(managerFileDescriptor);
            strcpy(writeBuffer, UPDATED);
            writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            readBytes = read(connectionFileDescriptor, &readBuffer, strlen(readBuffer));
            break;
        }
    }
    return true;
}

#endif