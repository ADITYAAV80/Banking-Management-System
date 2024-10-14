#ifndef ADMIN_PORTAL
#define ADMIN_PORTAL

#include "../common/login/login_ad.h"

#include "../struct/struct_employee.h"
#include "../struct/struct_manager.h"
#include "../struct/struct_customer.h"

#include "../functions/server_const.h"

bool admin_portal(int connectionFileDescriptor);

bool add_employee(int connectionFileDescriptor);
bool modify_employee(int connectionFileDescriptor);
bool view_employee_details(int connectionFileDescriptor);

bool add_manager(int connectionFileDescriptor);
bool modify_manager(int connectionFileDescriptor);
bool view_manager_details(int connectionFileDescriptor);

bool add_customer_admin(int connectionFileDescriptor);
bool modify_customer_admin(int connectionFileDescriptor);
bool view_customer_details(int connectionFileDescriptor);

bool change_password_admin(int connectionFileDescriptor);

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
            case 2:
                modify_employee(connectionFileDescriptor);
                break;
            case 3:
                add_manager(connectionFileDescriptor);
                break;
            case 4:
                modify_manager(connectionFileDescriptor);
                break;
            case 5:
                add_customer_admin(connectionFileDescriptor);
                break;
            case 6:
                modify_customer_admin(connectionFileDescriptor);
                break;
            case 7:
                change_password_admin(connectionFileDescriptor);
                break;
            case 8:
                view_employee_details(connectionFileDescriptor);
                break;
            case 9:
                view_manager_details(connectionFileDescriptor);
                break;
            case 10:
                view_customer_details(connectionFileDescriptor);
                break;
            case 11:
                return true; // Exit from the admin portal
            default:
                return true;
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

    int EmployeeFileDescriptor = open("EMPLOYEE_FILE", O_RDONLY);
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
    printf("%s", readBuffer);
    return true;
}

bool view_employee_details(int connectionFileDescriptor)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    // the STUDENT_FILE has the details of the student
    int employeeFileDescriptor = open("EMPLOYEE_FILE", O_RDONLY, 0777);
    if (employeeFileDescriptor == -1)
    {
        perror("Error while opening student file");
        return false;
    }
    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_RDLCK;    // Read lock
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

    struct employee_struct employee1, employee;
    char EmployeeList[1000];
    EmployeeList[0] = '\0';
    // getting the login id of all employees to choose from
    while (read(employeeFileDescriptor, &employee1, sizeof(struct employee_struct)) == sizeof(struct employee_struct))
    {
        char tempBuffer[1000]; // Temporary buffer to construct the string
        sprintf(tempBuffer, "Login: %s ", employee1.login);
        strcat(EmployeeList, tempBuffer);
        sprintf(tempBuffer, "Name: %s ", employee1.name);
        strcat(EmployeeList, tempBuffer);
        strcat(EmployeeList, "\n");
    }
    lseek(employeeFileDescriptor, 0, SEEK_SET);

    strcpy(writeBuffer, EmployeeList);
    strcat(writeBuffer, EMPLOYEE_DETAILS);

    // writing the login id of all the employee avaliable blocked or unblocked
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

    bzero(writeBuffer, sizeof(writeBuffer));
    bzero(readBuffer, sizeof(readBuffer));
    // getting details of employee whose login id is provided by user
    while (read(employeeFileDescriptor, &employee, sizeof(struct employee_struct)) == sizeof(struct employee_struct))
    {
        if (strcmp(employee.login, login) == 0)
        {
            char myStr[100];
            sprintf(myStr, "Emp No: %d\n", employee.emp_no);
            strcat(EmployeeList, myStr);

            sprintf(myStr, "Name: %s\n", employee.name);
            strcat(EmployeeList, myStr);

            sprintf(myStr, "Email: %s\n", employee.email);
            strcat(EmployeeList, myStr);

            sprintf(myStr, "Address: %s\n", employee.address);
            strcat(EmployeeList, myStr);

            sprintf(myStr, "Age: %s\n", employee.age);
            strcat(EmployeeList, myStr);

            sprintf(myStr, "Login ID: %s\n", employee.login);
            strcat(EmployeeList, myStr);

            strcpy(writeBuffer, EmployeeList);
            strcat(writeBuffer, "Press any character followed by  Enter key to exit\n");
            writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            if (writeBytes == -1)
            {
                perror("Error while writing to file!");
                return false;
            }

            // unlocking
            lock.l_type = F_UNLCK;
            if (fcntl(employeeFileDescriptor, F_SETLK, &lock) == -1)
            {
                perror("Error releasing the lock");
            }

            close(employeeFileDescriptor);
            readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer));
            return true;
        }
    }
    strcpy(writeBuffer, "Wrong Option!\n Press any character followed by  Enter key to exit\n");
    // unlocking
    lock.l_type = F_UNLCK;
    if (fcntl(employeeFileDescriptor, F_SETLK, &lock) == -1)
    {
        perror("Error releasing the lock");
    }
    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer));
    close(employeeFileDescriptor);
    bzero(writeBuffer, sizeof(writeBuffer));
    bzero(readBuffer, sizeof(readBuffer));
    return false;
}

bool modify_employee(int connectionFileDescriptor)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];
    // opening the file in Read Write Mode
    int employeeFileDescriptor = open("EMPLOYEE_FILE", O_RDWR, 0777);
    if (employeeFileDescriptor == -1)
    {
        perror("Error while opening employee file");
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
    if (fcntl(employeeFileDescriptor, F_SETLKW, &lock) == -1)
    {
        perror("Error locking the file");
        close(employeeFileDescriptor);
        exit(EXIT_FAILURE);
    }
    struct employee_struct employee1, employee;
    char EmployeeList[1000];
    EmployeeList[0] = '\0';
    // giving all the employee id to be modified as choice
    while (read(employeeFileDescriptor, &employee1, sizeof(struct employee_struct)) == sizeof(struct employee_struct))
    {
        if (employee1.active == 1)
        {
            char tempBuffer[1000]; // Temporary buffer to construct the string
            sprintf(tempBuffer, "Login ID : %s", employee1.login);
            strcat(EmployeeList, tempBuffer);
            sprintf(tempBuffer, "\tName : %s ", employee1.name);
            strcat(EmployeeList, tempBuffer);
            strcat(EmployeeList, "\n");
        }
    }
    lseek(employeeFileDescriptor, 0, SEEK_SET);

    strcpy(writeBuffer, EmployeeList);
    strcat(writeBuffer, EMPLOYEE_UPDATE);

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
    while (read(employeeFileDescriptor, &employee1, sizeof(struct employee_struct)) == sizeof(struct employee_struct))
    {
        if (strcmp(employee1.login, login) == 0)
        {
            lseek(employeeFileDescriptor, -1 * sizeof(struct employee_struct), SEEK_CUR);
            // to update name
            bzero(writeBuffer, sizeof(writeBuffer));
            bzero(readBuffer, sizeof(readBuffer));

            strcpy(writeBuffer, EMPLOYEE_NAME);
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
                strcpy(employee1.name, readBuffer);
            }

            // to update address
            bzero(writeBuffer, sizeof(writeBuffer));
            bzero(readBuffer, sizeof(readBuffer));
            strcpy(writeBuffer, EMPLOYEE_ADDRESS);
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
                strcpy(employee1.address, readBuffer);
            }

            // to update email
            bzero(writeBuffer, sizeof(writeBuffer));
            bzero(readBuffer, sizeof(readBuffer));
            strcpy(writeBuffer, EMPLOYEE_EMAIL);
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
                strcpy(employee1.email, readBuffer);
            }

            bzero(writeBuffer, sizeof(writeBuffer));
            bzero(readBuffer, sizeof(readBuffer));

            write(employeeFileDescriptor, &employee1, sizeof(struct employee_struct));
            // unlocking
            lock.l_type = F_UNLCK;
            if (fcntl(employeeFileDescriptor, F_SETLK, &lock) == -1)
            {
                perror("Error releasing the lock");
            }
            close(employeeFileDescriptor);
            strcpy(writeBuffer, UPDATED);
            writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            readBytes = read(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            return true;
        }
    }
    // if ID not found
    //  unlocking
    lock.l_type = F_UNLCK;
    if (fcntl(employeeFileDescriptor, F_SETLK, &lock) == -1)
    {
        perror("Error releasing the lock");
    }
    close(employeeFileDescriptor);
    strcpy(writeBuffer, NOT_UPDATED);
    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    readBytes = read(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    return true;
}

bool add_manager(int connectionFileDescriptor)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct manager_struct new_manager, prev_manager;
    // for manager's emp no given in FCFS format

    int managerFileDescriptor = open("MANAGER_FILE", O_RDONLY);
    if (managerFileDescriptor == -1 && errno == ENOENT)
    {
        //  manager.txt was never created
        new_manager.mng_no = 1;
    }

    else if (managerFileDescriptor == -1)
    {
        perror("Error while opening manager file");
        return false;
    }

    else
    {
        int offset = lseek(managerFileDescriptor, -sizeof(struct manager_struct), SEEK_END);
        if (offset == -1)
        {
            perror("Error seeking to last manager record!");
            return false;
        }

        struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct manager_struct), getpid()};
        int lockingStatus = fcntl(managerFileDescriptor, F_SETLKW, &lock);
        if (lockingStatus == -1)
        {
            perror("Error obtaining read lock on manager record!");
            return false;
        }

        readBytes = read(managerFileDescriptor, &prev_manager, sizeof(struct manager_struct));
        if (readBytes == -1)
        {
            perror("Error while reading manager record from file");
            return false;
        }

        lock.l_type = F_UNLCK;
        fcntl(managerFileDescriptor, F_SETLK, &lock);

        close(managerFileDescriptor);

        new_manager.mng_no = prev_manager.mng_no + 1;
    }
    close(managerFileDescriptor);

    // for manager name
    writeBytes = write(connectionFileDescriptor, MANAGER_NAME, strlen(MANAGER_NAME));
    if (writeBytes == -1)
    {
        perror("Error writing MANAGER_NAME message to client!");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading manager name");
        return false;
    }

    strcpy(new_manager.name, readBuffer);

    // for managers age
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(connectionFileDescriptor, MANAGER_AGE, strlen(MANAGER_AGE));
    if (writeBytes == -1)
    {
        perror("Error writing MANAGER_AGE message to client!");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    strcpy(new_manager.age, readBuffer);

    // for managers address
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(connectionFileDescriptor, MANAGER_ADDRESS, strlen(MANAGER_ADDRESS));
    if (writeBytes == -1)
    {
        perror("Error writing MANAGER_ADDRESS message to client!");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    strcpy(new_manager.address, readBuffer);

    // for managers email
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(connectionFileDescriptor, MANAGER_EMAIL, strlen(MANAGER_EMAIL));
    if (writeBytes == -1)
    {
        perror("Error writing MANAGER_EMAIL message to client!");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    strcpy(new_manager.email, readBuffer);

    // for managers login
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(connectionFileDescriptor, MANAGER_LOGIN, strlen(MANAGER_LOGIN));
    if (writeBytes == -1)
    {
        perror("Error writing MANAGER_LOGIN message to client!");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    strcpy(new_manager.login, readBuffer);

    // for managers password
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(connectionFileDescriptor, MANAGER_PASSWORD, strlen(MANAGER_PASSWORD));
    if (writeBytes == -1)
    {
        perror("Error writing MANAGER_PASSWORD message to client!");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    strcpy(new_manager.password, readBuffer);

    // make manager active from the beginning
    new_manager.active = true;

    managerFileDescriptor = open("MANAGER_FILE", O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);
    if (managerFileDescriptor == -1)
    {
        perror("Error while creating / opening manager file!");
        return false;
    }
    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK;    // Write lock
    lock.l_whence = SEEK_SET; // Start from the beginning of the file
    lock.l_start = 0;         // Offset 0
    lock.l_len = 0;           // Lock the entire file

    // Try to acquire the lock in blocking mode
    if (fcntl(managerFileDescriptor, F_SETLKW, &lock) == -1)
    {
        perror("Error locking the file");
        close(managerFileDescriptor);
        exit(EXIT_FAILURE);
    }
    // writing the managers data into the file
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(managerFileDescriptor, &new_manager, sizeof(struct manager_struct));
    if (writeBytes == -1)
    {
        perror("Error while writing manager record to file!");
        return false;
    }
    // releasing the lock
    lock.l_type = F_UNLCK;
    if (fcntl(managerFileDescriptor, F_SETLK, &lock) == -1)
    {
        perror("Error releasing the lock");
    }
    close(managerFileDescriptor);

    // writing a message for add confirmation
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(connectionFileDescriptor, MANAGER_ADDED, strlen(MANAGER_ADDED));
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

bool modify_manager(int connectionFileDescriptor)
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

    // write lock
    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK;    // Write lock
    lock.l_whence = SEEK_SET; // Start from the beginning of the file
    lock.l_start = 0;         // Offset 0
    lock.l_len = 0;           // Lock the entire file

    // Try to acquire the lock in blocking mode
    if (fcntl(managerFileDescriptor, F_SETLKW, &lock) == -1)
    {
        perror("Error locking the file");
        close(managerFileDescriptor);
        exit(EXIT_FAILURE);
    }
    struct manager_struct manager1, manager;
    char EmployeeList[1000];
    EmployeeList[0] = '\0';
    // giving all the manager id to be modified as choice
    while (read(managerFileDescriptor, &manager1, sizeof(struct manager_struct)) == sizeof(struct manager_struct))
    {
        if (manager1.active == 1)
        {
            char tempBuffer[1000]; // Temporary buffer to construct the string
            sprintf(tempBuffer, "Login ID : %s", manager1.login);
            strcat(EmployeeList, tempBuffer);
            sprintf(tempBuffer, "\tName : %s ", manager1.name);
            strcat(EmployeeList, tempBuffer);
            strcat(EmployeeList, "\n");
        }
    }
    lseek(managerFileDescriptor, 0, SEEK_SET);

    strcpy(writeBuffer, EmployeeList);
    strcat(writeBuffer, MANAGER_UPDATE);

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
    while (read(managerFileDescriptor, &manager1, sizeof(struct manager_struct)) == sizeof(struct manager_struct))
    {
        if (strcmp(manager1.login, login) == 0)
        {
            lseek(managerFileDescriptor, -1 * sizeof(struct manager_struct), SEEK_CUR);
            // to update name
            bzero(writeBuffer, sizeof(writeBuffer));
            bzero(readBuffer, sizeof(readBuffer));

            strcpy(writeBuffer, MANAGER_NAME);
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
                strcpy(manager1.name, readBuffer);
            }

            // to update address
            bzero(writeBuffer, sizeof(writeBuffer));
            bzero(readBuffer, sizeof(readBuffer));
            strcpy(writeBuffer, MANAGER_ADDRESS);
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
                strcpy(manager1.address, readBuffer);
            }

            // to update email
            bzero(writeBuffer, sizeof(writeBuffer));
            bzero(readBuffer, sizeof(readBuffer));
            strcpy(writeBuffer, MANAGER_EMAIL);
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
                strcpy(manager1.email, readBuffer);
            }

            bzero(writeBuffer, sizeof(writeBuffer));
            bzero(readBuffer, sizeof(readBuffer));

            write(managerFileDescriptor, &manager1, sizeof(struct manager_struct));
            // unlocking
            lock.l_type = F_UNLCK;
            if (fcntl(managerFileDescriptor, F_SETLK, &lock) == -1)
            {
                perror("Error releasing the lock");
            }
            close(managerFileDescriptor);
            strcpy(writeBuffer, UPDATED);
            writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            readBytes = read(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            return true;
        }
    }
    // if ID not found
    //  unlocking
    lock.l_type = F_UNLCK;
    if (fcntl(managerFileDescriptor, F_SETLK, &lock) == -1)
    {
        perror("Error releasing the lock");
    }
    close(managerFileDescriptor);
    strcpy(writeBuffer, NOT_UPDATED);
    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    readBytes = read(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    return true;
}

bool view_manager_details(int connectionFileDescriptor)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    // the STUDENT_FILE has the details of the student
    int managerFileDescriptor = open("MANAGER_FILE", O_RDONLY, 0777);
    if (managerFileDescriptor == -1)
    {
        perror("Error while opening student file");
        return false;
    }
    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_RDLCK;    // Read lock
    lock.l_whence = SEEK_SET; // Start from the beginning of the file
    lock.l_start = 0;         // Offset 0
    lock.l_len = 0;           // Lock the entire file

    // Try to acquire the lock in blocking mode
    if (fcntl(managerFileDescriptor, F_SETLKW, &lock) == -1)
    {
        perror("Error locking the file");
        close(managerFileDescriptor);
        exit(EXIT_FAILURE);
    }

    struct manager_struct manager1, manager;
    char EmployeeList[1000];
    EmployeeList[0] = '\0';
    // getting the login id of all employees to choose from
    while (read(managerFileDescriptor, &manager1, sizeof(struct manager_struct)) == sizeof(struct manager_struct))
    {
        char tempBuffer[1000]; // Temporary buffer to construct the string
        sprintf(tempBuffer, "Login: %s ", manager1.login);
        strcat(EmployeeList, tempBuffer);
        sprintf(tempBuffer, "Name: %s ", manager1.name);
        strcat(EmployeeList, tempBuffer);
        strcat(EmployeeList, "\n");
    }
    lseek(managerFileDescriptor, 0, SEEK_SET);

    strcpy(writeBuffer, EmployeeList);
    strcat(writeBuffer, MANAGER_DETAILS);

    // writing the login id of all the employees avaliable blocked or unblocked
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

    bzero(writeBuffer, sizeof(writeBuffer));
    bzero(readBuffer, sizeof(readBuffer));
    // getting details of employees whose login id is provided by user
    while (read(managerFileDescriptor, &manager, sizeof(struct manager_struct)) == sizeof(struct manager_struct))
    {
        if (strcmp(manager.login, login) == 0)
        {
            char myStr[100];
            sprintf(myStr, "Emp No: %d\n", manager.mng_no);
            strcat(EmployeeList, myStr);

            sprintf(myStr, "Name: %s\n", manager.name);
            strcat(EmployeeList, myStr);

            sprintf(myStr, "Email: %s\n", manager.email);
            strcat(EmployeeList, myStr);

            sprintf(myStr, "Address: %s\n", manager.address);
            strcat(EmployeeList, myStr);

            sprintf(myStr, "Age: %s\n", manager.age);
            strcat(EmployeeList, myStr);

            sprintf(myStr, "Login ID: %s\n", manager.login);
            strcat(EmployeeList, myStr);

            strcpy(writeBuffer, EmployeeList);
            strcat(writeBuffer, "Press any character followed by  Enter key to exit\n");
            writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            if (writeBytes == -1)
            {
                perror("Error while writing to file!");
                return false;
            }

            // unlocking
            lock.l_type = F_UNLCK;
            if (fcntl(managerFileDescriptor, F_SETLK, &lock) == -1)
            {
                perror("Error releasing the lock");
            }

            close(managerFileDescriptor);
            readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer));
            return true;
        }
    }
    strcpy(writeBuffer, "Wrong Option!\n Press any character followed by  Enter key to exit\n");
    // unlocking
    lock.l_type = F_UNLCK;
    if (fcntl(managerFileDescriptor, F_SETLK, &lock) == -1)
    {
        perror("Error releasing the lock");
    }
    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer));
    close(managerFileDescriptor);
    bzero(writeBuffer, sizeof(writeBuffer));
    bzero(readBuffer, sizeof(readBuffer));
    return false;
}

bool add_customer_admin(int connectionFileDescriptor)
{
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

bool view_customer_details(int connectionFileDescriptor)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    // the STUDENT_FILE has the details of the student
    int customerFileDescriptor = open("CUSTOMER_FILE", O_RDONLY, 0777);
    if (customerFileDescriptor == -1)
    {
        perror("Error while opening student file");
        return false;
    }
    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_RDLCK;    // Read lock
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
    // getting the login id of all employees to choose from
    while (read(customerFileDescriptor, &customer1, sizeof(struct customer_struct)) == sizeof(struct customer_struct))
    {
        char tempBuffer[1000]; // Temporary buffer to construct the string
        sprintf(tempBuffer, "Login: %s ", customer1.login);
        strcat(EmployeeList, tempBuffer);
        sprintf(tempBuffer, "Name: %s ", customer1.name);
        strcat(EmployeeList, tempBuffer);
        strcat(EmployeeList, "\n");
    }
    lseek(customerFileDescriptor, 0, SEEK_SET);

    strcpy(writeBuffer, EmployeeList);
    strcat(writeBuffer, CUSTOMER_DETAILS);

    // writing the login id of all the employees avaliable blocked or unblocked
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

    bzero(writeBuffer, sizeof(writeBuffer));
    bzero(readBuffer, sizeof(readBuffer));
    // getting details of employees whose login id is provided by user
    while (read(customerFileDescriptor, &customer, sizeof(struct customer_struct)) == sizeof(struct customer_struct))
    {
        if (strcmp(customer.login, login) == 0)
        {
            char myStr[100];
            sprintf(myStr, "Customer No: %d\n", customer.cus_no);
            strcat(EmployeeList, myStr);

            sprintf(myStr, "Name: %s\n", customer.name);
            strcat(EmployeeList, myStr);

            sprintf(myStr, "Email: %s\n", customer.email);
            strcat(EmployeeList, myStr);

            sprintf(myStr, "Address: %s\n", customer.address);
            strcat(EmployeeList, myStr);

            sprintf(myStr, "Age: %s\n", customer.age);
            strcat(EmployeeList, myStr);

            sprintf(myStr, "Login ID: %s\n", customer.login);
            strcat(EmployeeList, myStr);

            sprintf(myStr, "Balance: %d\n", customer.balance);
            strcat(EmployeeList, myStr);

            strcpy(writeBuffer, EmployeeList);
            strcat(writeBuffer, "Press any character followed by  Enter key to exit\n");
            writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            if (writeBytes == -1)
            {
                perror("Error while writing to file!");
                return false;
            }

            // unlocking
            lock.l_type = F_UNLCK;
            if (fcntl(customerFileDescriptor, F_SETLK, &lock) == -1)
            {
                perror("Error releasing the lock");
            }

            close(customerFileDescriptor);
            readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer));
            return true;
        }
    }
    strcpy(writeBuffer, "Wrong Option!\n Press any character followed by  Enter key to exit\n");
    // unlocking
    lock.l_type = F_UNLCK;
    if (fcntl(customerFileDescriptor, F_SETLK, &lock) == -1)
    {
        perror("Error releasing the lock");
    }
    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer));
    close(customerFileDescriptor);
    bzero(writeBuffer, sizeof(writeBuffer));
    bzero(readBuffer, sizeof(readBuffer));
    return false;
}
bool change_password_admin(int connectionFileDescriptor)
{
    printf("Change password");
    return true;
}

#endif