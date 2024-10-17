
bool modify_customer_admin(int connectionFileDescriptor)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];
    // opening the file in Read Write Mode
    int customerFileDescriptor = open("CUSTOMER_FILE", O_RDWR, 0777);
    if (customerFileDescriptor == -1)
    {
        perror("Error while opening customer file");
        return false;
    }

    struct flock lock;
    memset(&lock, 0, sizeof(lock));

    struct customer_struct customer1, customer;
    char customerlist[1000];
    customerlist[0] = '\0';

    // giving all the customer id to be modified as choice
    while (read(customerFileDescriptor, &customer1, sizeof(struct customer_struct)) == sizeof(struct customer_struct))
    {
        if (customer1.active == 1)
        {
            lock.l_type = F_RDLCK;
            lock.l_whence = SEEK_CUR;
            lock.l_start = -sizeof(struct customer_struct);
            lock.l_len = sizeof(struct customer_struct);

            // Try to acquire the lock in blocking mode
            if (fcntl(customerFileDescriptor, F_SETLKW, &lock) == -1)
            {
                perror("Error locking the file");
                close(customerFileDescriptor);
                exit(EXIT_FAILURE);
            }

            char tempBuffer[1000]; // Temporary buffer to construct the string
            sprintf(tempBuffer, "Login ID : %s", customer1.login);
            strcat(customerlist, tempBuffer);
            sprintf(tempBuffer, "\tName : %s ", customer1.name);
            strcat(customerlist, tempBuffer);
            strcat(customerlist, "\n");

            lock.l_type = F_UNLCK;
            if (fcntl(customerFileDescriptor, F_SETLK, &lock) == -1)
            {
                perror("Error releasing the lock");
            }

            // Unlock the current customer record after reading
            lock.l_type = F_UNLCK; // Unlock
            if (fcntl(customerFileDescriptor, F_SETLK, &lock) == -1)
            {
                perror("Error unlocking the customer record");
            }
        }
    }
    lseek(customerFileDescriptor, 0, SEEK_SET);

    strcpy(writeBuffer, customerlist);
    strcat(writeBuffer, CUSTOMER_UPDATE);

    bzero(customerlist, sizeof(customerlist));
    customerlist[0] = '\0';

    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error while writing to file!");
        return false;
    }
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    char login[20];

    strcpy(login, readBuffer);
    // searching the requested ID and modifying it.
    while (read(customerFileDescriptor, &customer1, sizeof(struct customer_struct)) == sizeof(struct customer_struct))
    {
        if (strcmp(customer1.login, login) == 0)
        {

            lseek(customerFileDescriptor, -1 * sizeof(struct customer_struct), SEEK_CUR);
            // to update name
            bzero(writeBuffer, sizeof(writeBuffer));
            bzero(readBuffer, sizeof(readBuffer));

            strcpy(writeBuffer, CUSTOMER_NAME);
            strcat(writeBuffer, CHOICE);
            writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
            if (strcmp(readBuffer, "y") == 0)
            {
                bzero(writeBuffer, sizeof(writeBuffer));
                bzero(readBuffer, sizeof(readBuffer));

                strcpy(writeBuffer, UPDATE);
                writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
                readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
                strcpy(customer1.name, readBuffer);
            }

            // to update address
            bzero(writeBuffer, sizeof(writeBuffer));
            bzero(readBuffer, sizeof(readBuffer));
            strcpy(writeBuffer, CUSTOMER_ADDRESS);
            strcat(writeBuffer, CHOICE);
            writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
            if (strcmp(readBuffer, "y") == 0)
            {
                bzero(writeBuffer, sizeof(writeBuffer));
                bzero(readBuffer, sizeof(readBuffer));

                strcpy(writeBuffer, UPDATE);
                writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
                readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
                strcpy(customer1.address, readBuffer);
            }

            // to update email
            bzero(writeBuffer, sizeof(writeBuffer));
            bzero(readBuffer, sizeof(readBuffer));
            strcpy(writeBuffer, CUSTOMER_EMAIL);
            strcat(writeBuffer, CHOICE);
            writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
            if (strcmp(readBuffer, "y") == 0)
            {
                bzero(writeBuffer, sizeof(writeBuffer));
                bzero(readBuffer, sizeof(readBuffer));

                strcpy(writeBuffer, UPDATE);
                writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
                readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
                strcpy(customer1.email, readBuffer);
            }

            bzero(writeBuffer, sizeof(writeBuffer));
            bzero(readBuffer, sizeof(readBuffer));

            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_CUR;
            lock.l_start = 0;
            lock.l_len = sizeof(struct customer_struct);

            // Try to acquire the lock in blocking mode
            if (fcntl(customerFileDescriptor, F_SETLKW, &lock) == -1)
            {
                perror("Error locking the file");
                close(customerFileDescriptor);
                exit(EXIT_FAILURE);
            }
            writeBytes = write(customerFileDescriptor, &customer1, sizeof(struct customer_struct));
            // unlocking
            lock.l_type = F_UNLCK;
            if (fcntl(customerFileDescriptor, F_SETLK, &lock) == -1)
            {
                perror("Error releasing the lock");
            }
            close(customerFileDescriptor);
            strcpy(writeBuffer, UPDATED);
            writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            readBytes = read(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            return true;
        }
    }
    close(customerFileDescriptor);
    strcpy(writeBuffer, NOT_UPDATED);
    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, strlen(readBuffer));
    return true;
}
