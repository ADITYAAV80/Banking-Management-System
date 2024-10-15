#ifndef EMPLOYEE_PORTAL
#define EMPLOYEE_PORTAL

#include <unistd.h>
#include "../functions/server_const.h"
#include "../common/login/login_em.h"
#include "../struct/struct_customer.h"
#include "../struct/struct_employee.h"

bool add_customer_employee(int connectionFileDescriptor);
bool modify_customer_employee(int connectionFileDescriptor);
bool process_loan_applications(int connectionFileDescriptor);
bool approve_reject_loans(int connectionFileDescriptor);
bool view_assigned_loan_applications(int connectionFileDescriptor);
bool view_customer_transactions(int connectionFileDescriptor);
bool change_password_employee(int connectionFileDescriptor, char *employee_id);

bool employee_portal(int connectionFileDescriptor)
{
    // creating read and write buffer
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];
    char tempBuffer[1000];

    strcpy(writeBuffer, S_EMPLOYEE_LOGIN_WELCOME);
    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error in writing\n");
        return false;
    }

    readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error in writing\n");
        return false;
    }

    char employee_id[20];
    strcpy(employee_id, readBuffer);

    bzero(readBuffer, sizeof(readBuffer));

    bzero(writeBuffer, sizeof(writeBuffer));
    strcpy(writeBuffer, S_EMPLOYEE_PASSWORD);

    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error in writing\n");
        return false;
    }
    readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error in writing\n");
        return false;
    }
    char pass_buffer[20];
    strcpy(pass_buffer, readBuffer);

    // Debug
    // printf("%s", employee_id);
    // printf("%s", pass_buffer);

    bzero(writeBuffer, sizeof(writeBuffer));
    if (login_employee(connectionFileDescriptor, employee_id, pass_buffer))
    {
        bzero(readBuffer, sizeof(readBuffer));
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, S_EMPLOYEE_LOGIN_SUCCESS);
        while (1)
        {
            strcat(writeBuffer, "\n");
            strcat(writeBuffer, S_EMPLOYEE_MENU);
            writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            if (writeBytes == -1)
            {
                perror("Error while writing to client!");
                return false;
            }
            bzero(writeBuffer, sizeof(writeBuffer));

            readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer));
            if (readBytes == -1)
            {
                perror("Error while reading client's choice for ADMIN_MENU");
                return false;
            }
            // all the employee options
            int choice = atoi(readBuffer);
            printf("Choice received: %d\n", choice);

            switch (choice)
            {
            case 1:
                add_customer_employee(connectionFileDescriptor);
                break;
            case 2:
                modify_customer_employee(connectionFileDescriptor);
                break;
            case 3:
                view_assigned_loan_applications(connectionFileDescriptor);
                break;
            case 4:
                approve_reject_loans(connectionFileDescriptor);
                break;
            case 5:
                view_customer_transactions(connectionFileDescriptor);
                break;
            case 6:
                change_password_employee(connectionFileDescriptor,employee_id);
                break;
            case 7:
                return true;
            default:
                return true;
            }
        }
    }
    else
    {
        return false; // employee login failed
    }
    return true;
}

bool add_customer_employee(int connectionFileDescriptor){
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct customer_struct new_customer, prev_customer;
    // for Employee's emp no given in FCFS format

    int EmployeeFileDescriptor = open("CUSTOMER_FILE", O_RDONLY);
    if (EmployeeFileDescriptor == -1 && errno == ENOENT)
    {
        //  customer.txt was never created
        new_customer.cus_no = 1;
    }

    else if (EmployeeFileDescriptor == -1)
    {
        perror("Error while opening customer file");
        return false;
    }

    else
    {
        int offset = lseek(EmployeeFileDescriptor, -sizeof(struct customer_struct), SEEK_END);
        if (offset == -1)
        {
            perror("Error seeking to last customer record!");
            return false;
        }

        struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct customer_struct), getpid()};
        int lockingStatus = fcntl(EmployeeFileDescriptor, F_SETLKW, &lock);
        if (lockingStatus == -1)
        {
            perror("Error obtaining read lock on CUSTOMER record!");
            return false;
        }

        readBytes = read(EmployeeFileDescriptor, &prev_customer, sizeof(struct customer_struct));
        if (readBytes == -1)
        {
            perror("Error while reading CUSTOMER record from file");
            return false;
        }

        lock.l_type = F_UNLCK;
        fcntl(EmployeeFileDescriptor, F_SETLK, &lock);

        close(EmployeeFileDescriptor);

        new_customer.cus_no = prev_customer.cus_no + 1;
    }
    close(EmployeeFileDescriptor);

    // for customer name
    writeBytes = write(connectionFileDescriptor, CUSTOMER_NAME, strlen(CUSTOMER_NAME));
    if (writeBytes == -1)
    {
        perror("Error writing CUSTOMER_NAME message to client!");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading CUSTOMER name");
        return false;
    }

    strcpy(new_customer.name, readBuffer);

    // for CUSTOMERs age
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(connectionFileDescriptor, CUSTOMER_AGE, strlen(CUSTOMER_AGE));
    if (writeBytes == -1)
    {
        perror("Error writing CUSTOMER_AGE message to client!");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    strcpy(new_customer.age, readBuffer);

    // for CUSTOMERs address
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(connectionFileDescriptor, CUSTOMER_ADDRESS, strlen(CUSTOMER_ADDRESS));
    if (writeBytes == -1)
    {
        perror("Error writing CUSTOMER_ADDRESS message to client!");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    strcpy(new_customer.address, readBuffer);

    // for CUSTOMERs email
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(connectionFileDescriptor, CUSTOMER_EMAIL, strlen(CUSTOMER_EMAIL));
    if (writeBytes == -1)
    {
        perror("Error writing CUSTOMER_EMAIL message to client!");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    strcpy(new_customer.email, readBuffer);

    // for CUSTOMERs login
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(connectionFileDescriptor, CUSTOMER_LOGIN, strlen(CUSTOMER_LOGIN));
    if (writeBytes == -1)
    {
        perror("Error writing CUSTOMER_LOGIN message to client!");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    strcpy(new_customer.login, readBuffer);

    // for CUSTOMERs password
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(connectionFileDescriptor, CUSTOMER_PASSWORD, strlen(CUSTOMER_PASSWORD));
    if (writeBytes == -1)
    {
        perror("Error writing CUSTOMER_PASSWORD message to client!");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    strcpy(new_customer.password, readBuffer);

    // make CUSTOMER active from the beginning
    new_customer.active = true;
    new_customer.balance = 0;

    EmployeeFileDescriptor = open("CUSTOMER_FILE", O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);
    if (EmployeeFileDescriptor == -1)
    {
        perror("Error while creating / opening CUSTOMER file!");
        return false;
    }
    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK;    // Write lock
    lock.l_whence = SEEK_SET; // Start from the beginning of the file
    lock.l_start = 0;         // Offset 0
    lock.l_len = 0;           // Lock the entire file

    // Try to acquire the lock in blocking mode
    if (fcntl(EmployeeFileDescriptor, F_SETLKW, &lock) == -1)
    {
        perror("Error locking the file");
        close(EmployeeFileDescriptor);
        exit(EXIT_FAILURE);
    }
    // writing the CUSTOMERs data into the file
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(EmployeeFileDescriptor, &new_customer, sizeof(struct customer_struct));
    if (writeBytes == -1)
    {
        perror("Error while writing CUSTOMER record to file!");
        return false;
    }
    // releasing the lock
    lock.l_type = F_UNLCK;
    if (fcntl(EmployeeFileDescriptor, F_SETLK, &lock) == -1)
    {
        perror("Error releasing the lock");
    }
    close(EmployeeFileDescriptor);

    // writing a message for add confirmation
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(connectionFileDescriptor, CUSTOMER_ADDED, strlen(CUSTOMER_ADDED));
    if (writeBytes == -1)
    {
        perror("Error writing message");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    printf("%s", readBuffer);
    return true;
}
bool modify_customer_employee(int connectionFileDescriptor){
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];
    // opening the file in Read Write Mode
    int customerFileDescriptor = open("CUSTOMER_FILE", O_RDWR, 0777);
    if (customerFileDescriptor == -1)
    {
        perror("Error while opening customer file");
        return false;
    }

    // write lock
    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK;    // Write lock
    lock.l_whence = SEEK_SET; // Start from the beginning of the file
    lock.l_start = 0;         // Offset 0
    lock.l_len = 0;           // Lock the entire file

    // Try to acquire the lock in blocking mode
    if (fcntl(customerFileDescriptor, F_SETLKW, &lock) == -1)
    {
        perror("Error locking the file");
        close(customerFileDescriptor);
        exit(EXIT_FAILURE);
    }
    struct customer_struct customer1, customer;
    char EmployeeList[1000];
    EmployeeList[0] = '\0';
    // giving all the customer id to be modified as choice
    while (read(customerFileDescriptor, &customer1, sizeof(struct customer_struct)) == sizeof(struct customer_struct))
    {
        if (customer1.active == 1)
        {
            char tempBuffer[1000]; // Temporary buffer to construct the string
            sprintf(tempBuffer, "Login ID : %s", customer1.login);
            strcat(EmployeeList, tempBuffer);
            sprintf(tempBuffer, "\tName : %s ", customer1.name);
            strcat(EmployeeList, tempBuffer);
            strcat(EmployeeList, "\n");
        }
    }
    lseek(customerFileDescriptor, 0, SEEK_SET);

    strcpy(writeBuffer, EmployeeList);
    strcat(writeBuffer, CUSTOMER_UPDATE);

    bzero(EmployeeList, sizeof(EmployeeList));
    EmployeeList[0] = '\0';

    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error while writing to file!");
        return false;
    }
    readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer));
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
            readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer));
            if (strcmp(readBuffer, "y") == 0)
            {
                bzero(writeBuffer, sizeof(writeBuffer));
                bzero(readBuffer, sizeof(readBuffer));

                strcpy(writeBuffer, UPDATE);
                writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
                readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer));
                strcpy(customer1.name, readBuffer);
            }

            // to update address
            bzero(writeBuffer, sizeof(writeBuffer));
            bzero(readBuffer, sizeof(readBuffer));
            strcpy(writeBuffer, CUSTOMER_ADDRESS);
            strcat(writeBuffer, CHOICE);
            writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer));
            if (strcmp(readBuffer, "y") == 0)
            {
                bzero(writeBuffer, sizeof(writeBuffer));
                bzero(readBuffer, sizeof(readBuffer));

                strcpy(writeBuffer, UPDATE);
                writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
                readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer));
                strcpy(customer1.address, readBuffer);
            }

            // to update email
            bzero(writeBuffer, sizeof(writeBuffer));
            bzero(readBuffer, sizeof(readBuffer));
            strcpy(writeBuffer, CUSTOMER_EMAIL);
            strcat(writeBuffer, CHOICE);
            writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer));
            if (strcmp(readBuffer, "y") == 0)
            {
                bzero(writeBuffer, sizeof(writeBuffer));
                bzero(readBuffer, sizeof(readBuffer));

                strcpy(writeBuffer, UPDATE);
                writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
                readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer));
                strcpy(customer1.email, readBuffer);
            }

            bzero(writeBuffer, sizeof(writeBuffer));
            bzero(readBuffer, sizeof(readBuffer));

            write(customerFileDescriptor, &customer1, sizeof(struct customer_struct));
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
    // if ID not found
    //  unlocking
    lock.l_type = F_UNLCK;
    if (fcntl(customerFileDescriptor, F_SETLK, &lock) == -1)
    {
        perror("Error releasing the lock");
    }
    close(customerFileDescriptor);
    strcpy(writeBuffer, NOT_UPDATED);
    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    readBytes = read(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    return true;
}
bool process_loan_applications(int connectionFileDescriptor){}

bool approve_reject_loans(int connectionFileDescriptor){}

bool view_assigned_loan_applications(int connectionFileDescriptor){}

bool view_customer_transactions(int connectionFileDescriptor){}

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

    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK;    // Write lock
    lock.l_whence = SEEK_SET; // Start from the beginning of the file
    lock.l_start = 0;         // Offset 0
    lock.l_len = 0;           // Lock the entire file

    // Try to acquire the lock in blocking mode
    if (fcntl(employeeFileDescriptor, F_SETLKW, &lock) == -1)
    {
        perror("Error locking the file");
        close(employeeFileDescriptor);
        exit(EXIT_FAILURE);
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
            readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer));

            strcpy(employee1.password, readBuffer);

            write(employeeFileDescriptor, &employee1, sizeof(struct employee_struct));

            lock.l_type = F_UNLCK; // to unlock file
            if (fcntl(employeeFileDescriptor, F_SETLK, &lock) == -1)
            {
                perror("Error releasing the lock");
            }
            close(employeeFileDescriptor);
            strcpy(writeBuffer, UPDATED);
            writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            readBytes = read(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            break;
        }
    }
    return true;
}


#endif