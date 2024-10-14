#ifndef ADMIN_PORTAL
#define ADMIN_PORTAL

#include "../common/login/login_ad.h"
#include "../struct/struct_employee.h"
#include "../functions/server_const.h"

bool admin_portal(int connectionFileDescriptor);
bool add_employee(int connectionFileDescriptor);
/*
bool admin_portal(int connectionFileDescriptor)
{
    if (login_admin(connectionFileDescriptor))
    {
        // Login successful for admin
        ssize_t writeBytes, readBytes;            // Number of bytes read from / written to the client
        char readBuffer[1000], writeBuffer[1000]; // A buffer used for reading & writing to the client
        bzero(writeBuffer, sizeof(writeBuffer));
        while (1)
        {

            bzero(readBuffer, sizeof(readBuffer));
            bzero(writeBuffer, sizeof(writeBuffer));
            strcpy(writeBuffer, ADMIN_MENU);

            writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            if (writeBytes == -1)
            {
                perror("Error while writing ADMIN_MENU to client!");
                return false;
            }
            bzero(writeBuffer, sizeof(writeBuffer));

            readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer));
            if (readBytes == -1)
            {
                perror("Error while reading client's choice for ADMIN_MENU");
                return false;
            }
            printf("Choice:%s", readBuffer);
            int choice = atoi(readBuffer);
            switch (choice)
            {
            case 1:
                // add_employee(connectionFileDescriptor);
                break;
            default:
                printf("Invalid choice. Exiting...\n");
                return false;
            }
        }
    }
    else
    {
        return false; // admin login failed
    }
    return true;
}
*/
bool admin_portal(int connectionFileDescriptor)
{
    if (login_admin(connectionFileDescriptor))
    {
        ssize_t writeBytes, readBytes;
        char readBuffer[1000], writeBuffer[1000];
        int choice;
        while (1)
        {
            // Send the Admin menu to the client
            bzero(writeBuffer, sizeof(writeBuffer));
            strcpy(writeBuffer, ADMIN_MENU);
            writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            if (writeBytes == -1)
            {
                perror("Error while writing ADMIN_MENU to client!");
                return false;
            }

            // Read the user's choice from the client
            bzero(readBuffer, sizeof(readBuffer));
            readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer));
            if (readBytes == -1)
            {
                perror("Error while reading client's choice for ADMIN_MENU");
                return false;
            }

            // Convert the user's input to an integer choice
            choice = atoi(readBuffer);
            printf("Choice received: %d\n", choice);

            // Process the admin's choice
            switch (choice)
            {
            case 1:
                add_employee(connectionFileDescriptor);
                break;
            case 9:
                printf("Logout and exit\n");
                return true; // Exit from the admin portal
            default:
                printf("Invalid choice. Exiting...\n");
                return false;
            }
        }
    }
    else
    {
        return false; // Admin login failed
    }
}

bool add_employee(int connectionFileDescriptor)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct employee_struct new_employee, prev_employee;
    // for Employee's emp no given in FCFS format

    int EmployeeFileDescriptor = open("employee.txt", O_RDONLY);
    if (EmployeeFileDescriptor == -1 && errno == ENOENT)
    {
        //  employee.txt was never created
        new_employee.emp_no = 1;
    }

    else if (EmployeeFileDescriptor == -1)
    {
        perror("Error while opening employee file");
        return false;
    }

    else
    {
        int offset = lseek(EmployeeFileDescriptor, -sizeof(struct employee_struct), SEEK_END);
        if (offset == -1)
        {
            perror("Error seeking to last employee record!");
            return false;
        }

        struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct employee_struct), getpid()};
        int lockingStatus = fcntl(EmployeeFileDescriptor, F_SETLKW, &lock);
        if (lockingStatus == -1)
        {
            perror("Error obtaining read lock on EMPLOYEE record!");
            return false;
        }

        readBytes = read(EmployeeFileDescriptor, &prev_employee, sizeof(struct employee_struct));
        if (readBytes == -1)
        {
            perror("Error while reading EMPLOYEE record from file");
            return false;
        }

        lock.l_type = F_UNLCK;
        fcntl(EmployeeFileDescriptor, F_SETLK, &lock);

        close(EmployeeFileDescriptor);

        new_employee.emp_no = prev_employee.emp_no + 1;
    }
    close(EmployeeFileDescriptor);

    // for employee name
    writeBytes = write(connectionFileDescriptor, EMPLOYEE_NAME, strlen(EMPLOYEE_NAME));
    if (writeBytes == -1)
    {
        perror("Error writing EMPLOYEE_NAME message to client!");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading EMPLOYEE name");
        return false;
    }

    strcpy(new_employee.name, readBuffer);

    // for EMPLOYEEs age
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(connectionFileDescriptor, EMPLOYEE_AGE, strlen(EMPLOYEE_AGE));
    if (writeBytes == -1)
    {
        perror("Error writing EMPLOYEE_AGE message to client!");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    strcpy(new_employee.age, readBuffer);

    // for EMPLOYEEs address
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(connectionFileDescriptor, EMPLOYEE_ADDRESS, strlen(EMPLOYEE_ADDRESS));
    if (writeBytes == -1)
    {
        perror("Error writing EMPLOYEE_ADDRESS message to client!");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    strcpy(new_employee.address, readBuffer);

    // for EMPLOYEEs email
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(connectionFileDescriptor, EMPLOYEE_EMAIL, strlen(EMPLOYEE_EMAIL));
    if (writeBytes == -1)
    {
        perror("Error writing EMPLOYEE_EMAIL message to client!");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    /*
    const char *src = "@iiitb.ac.in";
    strcat(readBuffer, src);
    */
    strcpy(new_employee.email, readBuffer);

    // for EMPLOYEEs login
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(connectionFileDescriptor, EMPLOYEE_LOGIN, strlen(EMPLOYEE_LOGIN));
    if (writeBytes == -1)
    {
        perror("Error writing EMPLOYEE_LOGIN message to client!");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    strcpy(new_employee.login, readBuffer);

    // for EMPLOYEEs password
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(connectionFileDescriptor, EMPLOYEE_PASSWORD, strlen(EMPLOYEE_PASSWORD));
    if (writeBytes == -1)
    {
        perror("Error writing EMPLOYEE_PASSWORD message to client!");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    strcpy(new_employee.password, readBuffer);

    // make EMPLOYEE active from the beginning
    new_employee.active = true;

    EmployeeFileDescriptor = open("EMPLOYEE_FILE", O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);
    if (EmployeeFileDescriptor == -1)
    {
        perror("Error while creating / opening EMPLOYEE file!");
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
    // writing the EMPLOYEEs data into the file
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(EmployeeFileDescriptor, &new_employee, sizeof(struct employee_struct));
    if (writeBytes == -1)
    {
        perror("Error while writing EMPLOYEE record to file!");
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
    writeBytes = write(connectionFileDescriptor, EMPLOYEE_ADDED, strlen(EMPLOYEE_ADDED));
    if (writeBytes == -1)
    {
        perror("Error writing message");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    return true;
}
#endif