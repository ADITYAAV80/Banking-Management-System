bool change_password_employee(int connectionFileDescriptor, char *employee_id)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];
    int employeeFileDescriptor = open("EMPLOYEE_FILE", O_RDWR, 0777);
    if (employeeFileDescriptor == -1)
    {
        perror("Error while opening file");
        return false;
    }

    struct employee_struct employee1;
    while (read(employeeFileDescriptor, &employee1, sizeof(struct employee_struct)) == sizeof(struct employee_struct))
    {
        if (strcmp(employee1.login, employee_id) == 0)
        {
            lseek(employeeFileDescriptor, -1 * sizeof(struct employee_struct), SEEK_CUR);

            // to update password
            bzero(writeBuffer, sizeof(writeBuffer));
            bzero(readBuffer, sizeof(readBuffer));

            strcpy(writeBuffer, S_EMPLOYEE_CHANGE_PASSWORD);

            writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));

            strcpy(employee1.password, readBuffer);

            struct flock lock;
            memset(&lock, 0, sizeof(lock));
            lock.l_type = F_WRLCK;    // Write lock
            lock.l_whence = SEEK_CUR; // Start from the current position of the file
            lock.l_start = 0;         // Offset 0
            lock.l_len = sizeof(struct employee_struct);

            // Try to acquire the lock in blocking mode
            if (fcntl(employeeFileDescriptor, F_SETLKW, &lock) == -1)
            {
                perror("Error locking the file");
                close(employeeFileDescriptor);
                exit(EXIT_FAILURE);
            }

            write(employeeFileDescriptor, &employee1, sizeof(struct employee_struct));

            lock.l_type = F_UNLCK; // to unlock file
            if (fcntl(employeeFileDescriptor, F_SETLK, &lock) == -1)
            {
                perror("Error releasing the lock");
            }
            close(employeeFileDescriptor);
            strcpy(writeBuffer, UPDATED);
            writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            readBytes = read(connectionFileDescriptor, &readBuffer, strlen(readBuffer));
            break;
        }
    }
    return true;
}

#endif