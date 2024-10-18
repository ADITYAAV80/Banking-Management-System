#ifndef ADMIN_PORTAL
#define ADMIN_PORTAL

#include "../common/login/login_ad.h"

#include "../struct/struct_employee.h"
#include "../struct/struct_manager.h"
#include "../struct/struct_customer.h"

#include "../functions/server_const.h"
#include "../common/login/login_common.h"

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

bool change_password_admin(int connectionFileDescriptor, char *admin_id);
bool add_admin(int connectionFileDescriptor);

bool admin_portal(int connectionFileDescriptor)
{
    // creating read and write buffer
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];
    char tempBuffer[1000];

    strcpy(writeBuffer, S_ADMIN_LOGIN_WELCOME);
    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error in writing\n");
        return false;
    }

    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error in writing\n");
        return false;
    }

    char admin_id[20];
    strcpy(admin_id, readBuffer);

    if (is_user_logged_in(admin_id) != 0)
    {
        bzero(readBuffer, sizeof(readBuffer));
        bzero(writeBuffer, sizeof(writeBuffer));

        strcpy(writeBuffer, MULTIPLE_LOGIN);
        writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error in writing\n");
            return false;
        }

        readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error in reading\n");
            return false;
        }
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));

    bzero(writeBuffer, sizeof(writeBuffer));
    strcpy(writeBuffer, S_ADMIN_PASSWORD);

    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error in writing\n");
        return false;
    }
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error in writing\n");
        return false;
    }
    char pass_buffer[20];
    strcpy(pass_buffer, readBuffer);

    // Debug
    // printf("%s", admin_id);
    // printf("%s", pass_buffer);

    bzero(writeBuffer, sizeof(writeBuffer));
    if (login_admin(connectionFileDescriptor, admin_id, pass_buffer))
    {
        bzero(readBuffer, sizeof(readBuffer));
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, S_ADMIN_LOGIN_SUCCESS);
        while (1)
        {
            strcat(writeBuffer, "\n");
            strcat(writeBuffer, S_ADMIN_MENU);
            writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            if (writeBytes == -1)
            {
                perror("Error while writing to client!");
                return false;
            }
            bzero(writeBuffer, sizeof(writeBuffer));

            readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
            if (readBytes == -1)
            {
                perror("Error while reading client's choice for ADMIN_MENU");
                return false;
            }
            // all the admin options
            int choice = atoi(readBuffer);
            printf("Choice received: %d\n", choice);

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
                add_admin(connectionFileDescriptor);
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
                change_password_admin(connectionFileDescriptor, admin_id);
                break;
            case 12:
                logout_user(admin_id);
                return true; // Exit from the admin portal
            default:
                logout_user(admin_id);
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
    // for employee's emp no given in FCFS format

    int employeeFileDescriptor = open("EMPLOYEE_FILE", O_RDONLY);
    if (employeeFileDescriptor == -1 && errno == ENOENT)
    {
        //  employee.txt was never created
        new_employee.emp_no = 1;
    }

    else if (employeeFileDescriptor == -1)
    {
        perror("Error while opening employee file");
        return false;
    }

    else
    {
        int offset = lseek(employeeFileDescriptor, -sizeof(struct employee_struct), SEEK_END);
        if (offset == -1)
        {
            perror("Error seeking to last employee record!");
            return false;
        }

        struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct employee_struct), getpid()};
        int lockingStatus = fcntl(employeeFileDescriptor, F_SETLKW, &lock);
        if (lockingStatus == -1)
        {
            perror("Error obtaining read lock on employee record!");
            return false;
        }

        readBytes = read(employeeFileDescriptor, &prev_employee, sizeof(struct employee_struct));
        if (readBytes == -1)
        {
            perror("Error while reading employee record from file");
            return false;
        }

        lock.l_type = F_UNLCK;
        fcntl(employeeFileDescriptor, F_SETLK, &lock);

        close(employeeFileDescriptor);

        new_employee.emp_no = prev_employee.emp_no + 1;
    }
    close(employeeFileDescriptor);

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
        perror("Error reading employee name");
        return false;
    }

    strcpy(new_employee.name, readBuffer);

    // for employees age
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

    // for employees address
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

    // for employees email
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

    // for employees login
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

    // for employees password
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

    // make employee active from the beginning
    new_employee.active = true;

    employeeFileDescriptor = open("EMPLOYEE_FILE", O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);
    if (employeeFileDescriptor == -1)
    {
        perror("Error while creating / opening employee file!");
        return false;
    }
    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK;    // Write lock
    lock.l_whence = SEEK_END; // Start from the beginning of the file
    lock.l_start = 0;         // Offset 0
    lock.l_len = sizeof(struct employee_struct);

    // Try to acquire the lock in blocking mode
    if (fcntl(employeeFileDescriptor, F_SETLKW, &lock) == -1)
    {
        perror("Error locking the file");
        close(employeeFileDescriptor);
        exit(EXIT_FAILURE);
    }
    // writing the employees data into the file
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(employeeFileDescriptor, &new_employee, sizeof(struct employee_struct));
    if (writeBytes == -1)
    {
        perror("Error while writing employee record to file!");
        return false;
    }
    // releasing the lock
    lock.l_type = F_UNLCK;
    if (fcntl(employeeFileDescriptor, F_SETLK, &lock) == -1)
    {
        perror("Error releasing the lock");
    }
    close(employeeFileDescriptor);

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

    // the employee_FILE has the details of the employee
    int employeeFileDescriptor = open("EMPLOYEE_FILE", O_RDWR, 0777);
    if (employeeFileDescriptor == -1)
    {
        perror("Error while opening employee file");
        return false;
    }

    struct flock lock;
    memset(&lock, 0, sizeof(lock));

    struct employee_struct employee1, employee;
    char employeelist[1000];
    employeelist[0] = '\0';

    // getting the login id of all employees to choose from
    while (read(employeeFileDescriptor, &employee1, sizeof(struct employee_struct)) == sizeof(struct employee_struct))
    {

        lock.l_type = F_RDLCK;
        lock.l_whence = SEEK_CUR;
        lock.l_start = -sizeof(struct employee_struct);
        lock.l_len = sizeof(struct employee_struct);

        // Try to acquire the lock in blocking mode
        if (fcntl(employeeFileDescriptor, F_SETLKW, &lock) == -1)
        {
            perror("Error locking the file");
            close(employeeFileDescriptor);
            exit(EXIT_FAILURE);
        }

        char tempBuffer[1000]; // Temporary buffer to construct the string
        sprintf(tempBuffer, "Login: %s ", employee1.login);
        strcat(employeelist, tempBuffer);
        sprintf(tempBuffer, "Name: %s ", employee1.name);
        strcat(employeelist, tempBuffer);
        strcat(employeelist, "\n");

        // Unlock the current employee record after reading
        lock.l_type = F_UNLCK; // Unlock
        if (fcntl(employeeFileDescriptor, F_SETLK, &lock) == -1)
        {
            perror("Error unlocking the employee record");
        }
    }
    lseek(employeeFileDescriptor, 0, SEEK_SET);

    strcpy(writeBuffer, employeelist);
    strcat(writeBuffer, EMPLOYEE_DETAILS);

    // writing the login id of all the employee available blocked or unblocked
    bzero(employeelist, sizeof(employeelist));
    employeelist[0] = '\0';
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

    // getting details of employee whose login id is provided by user
    while (read(employeeFileDescriptor, &employee, sizeof(struct employee_struct)) == sizeof(struct employee_struct))
    {
        if (strcmp(employee.login, login) == 0)
        {
            lock.l_type = F_RDLCK;
            lock.l_whence = SEEK_CUR;
            lock.l_start = -sizeof(struct employee_struct);
            lock.l_len = sizeof(struct employee_struct);

            // Try to acquire the lock in blocking mode
            if (fcntl(employeeFileDescriptor, F_SETLKW, &lock) == -1)
            {
                perror("Error locking the file");
                close(employeeFileDescriptor);
                exit(EXIT_FAILURE);
            }
            char myStr[100];
            sprintf(myStr, "Emp No: %d\n", employee.emp_no);
            strcat(employeelist, myStr);

            sprintf(myStr, "Name: %s\n", employee.name);
            strcat(employeelist, myStr);

            sprintf(myStr, "Email: %s\n", employee.email);
            strcat(employeelist, myStr);

            sprintf(myStr, "Address: %s\n", employee.address);
            strcat(employeelist, myStr);

            sprintf(myStr, "Age: %s\n", employee.age);
            strcat(employeelist, myStr);

            sprintf(myStr, "Login ID: %s\n", employee.login);
            strcat(employeelist, myStr);

            strcpy(writeBuffer, employeelist);
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
            readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
            close(employeeFileDescriptor);

            return true;
        }
    }
    strcpy(writeBuffer, "Wrong Option!\nPress any character followed by  Enter key to exit\n");
    // unlocking
    lock.l_type = F_UNLCK;
    if (fcntl(employeeFileDescriptor, F_SETLK, &lock) == -1)
    {
        perror("Error releasing the lock");
    }
    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
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

    struct flock lock;
    memset(&lock, 0, sizeof(lock));

    struct employee_struct employee1, employee;
    char employeelist[1000];
    employeelist[0] = '\0';

    // giving all the employee id to be modified as choice
    while (read(employeeFileDescriptor, &employee1, sizeof(struct employee_struct)) == sizeof(struct employee_struct))
    {
        if (employee1.active == 1)
        {
            lock.l_type = F_RDLCK;
            lock.l_whence = SEEK_CUR;
            lock.l_start = -sizeof(struct employee_struct);
            lock.l_len = sizeof(struct employee_struct);

            // Try to acquire the lock in blocking mode
            if (fcntl(employeeFileDescriptor, F_SETLKW, &lock) == -1)
            {
                perror("Error locking the file");
                close(employeeFileDescriptor);
                exit(EXIT_FAILURE);
            }

            char tempBuffer[1000]; // Temporary buffer to construct the string
            sprintf(tempBuffer, "Login ID : %s", employee1.login);
            strcat(employeelist, tempBuffer);
            sprintf(tempBuffer, "\tName : %s ", employee1.name);
            strcat(employeelist, tempBuffer);
            strcat(employeelist, "\n");

            lock.l_type = F_UNLCK;
            if (fcntl(employeeFileDescriptor, F_SETLK, &lock) == -1)
            {
                perror("Error releasing the lock");
            }

            // Unlock the current employee record after reading
            lock.l_type = F_UNLCK; // Unlock
            if (fcntl(employeeFileDescriptor, F_SETLK, &lock) == -1)
            {
                perror("Error unlocking the employee record");
            }
        }
    }
    lseek(employeeFileDescriptor, 0, SEEK_SET);

    strcpy(writeBuffer, employeelist);
    strcat(writeBuffer, EMPLOYEE_UPDATE);

    bzero(employeelist, sizeof(employeelist));
    employeelist[0] = '\0';

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
            readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
            if (strcmp(readBuffer, "y") == 0)
            {
                bzero(writeBuffer, sizeof(writeBuffer));
                bzero(readBuffer, sizeof(readBuffer));

                strcpy(writeBuffer, UPDATE);
                writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
                readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
                strcpy(employee1.name, readBuffer);
            }

            // to update address
            bzero(writeBuffer, sizeof(writeBuffer));
            bzero(readBuffer, sizeof(readBuffer));
            strcpy(writeBuffer, EMPLOYEE_ADDRESS);
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
                strcpy(employee1.address, readBuffer);
            }

            // to update email
            bzero(writeBuffer, sizeof(writeBuffer));
            bzero(readBuffer, sizeof(readBuffer));
            strcpy(writeBuffer, EMPLOYEE_EMAIL);
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
                strcpy(employee1.email, readBuffer);
            }

            bzero(writeBuffer, sizeof(writeBuffer));
            bzero(readBuffer, sizeof(readBuffer));

            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_CUR;
            lock.l_start = 0;
            lock.l_len = sizeof(struct employee_struct);

            // Try to acquire the lock in blocking mode
            if (fcntl(employeeFileDescriptor, F_SETLKW, &lock) == -1)
            {
                perror("Error locking the file");
                close(employeeFileDescriptor);
                exit(EXIT_FAILURE);
            }
            writeBytes = write(employeeFileDescriptor, &employee1, sizeof(struct employee_struct));
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
    close(employeeFileDescriptor);
    strcpy(writeBuffer, NOT_UPDATED);
    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, strlen(readBuffer));
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
    lock.l_whence = SEEK_END; // Start from the beginning of the file
    lock.l_start = 0;         // Offset 0
    lock.l_len = sizeof(struct manager_struct);

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

bool view_manager_details(int connectionFileDescriptor)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    // the manager_FILE has the details of the manager
    int managerFileDescriptor = open("MANAGER_FILE", O_RDWR, 0777);
    if (managerFileDescriptor == -1)
    {
        perror("Error while opening manager file");
        return false;
    }

    struct flock lock;
    memset(&lock, 0, sizeof(lock));

    struct manager_struct manager1, manager;
    char managerlist[1000];
    managerlist[0] = '\0';

    // getting the login id of all managers to choose from
    while (read(managerFileDescriptor, &manager1, sizeof(struct manager_struct)) == sizeof(struct manager_struct))
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
    lseek(managerFileDescriptor, 0, SEEK_SET);

    strcpy(writeBuffer, managerlist);
    strcat(writeBuffer, MANAGER_DETAILS);

    // writing the login id of all the manager available blocked or unblocked
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

    // getting details of manager whose login id is provided by user
    while (read(managerFileDescriptor, &manager, sizeof(struct manager_struct)) == sizeof(struct manager_struct))
    {
        if (strcmp(manager.login, login) == 0)
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
            char myStr[100];
            sprintf(myStr, "Mng No: %d\n", manager.mng_no);
            strcat(managerlist, myStr);

            sprintf(myStr, "Name: %s\n", manager.name);
            strcat(managerlist, myStr);

            sprintf(myStr, "Email: %s\n", manager.email);
            strcat(managerlist, myStr);

            sprintf(myStr, "Address: %s\n", manager.address);
            strcat(managerlist, myStr);

            sprintf(myStr, "Age: %s\n", manager.age);
            strcat(managerlist, myStr);

            sprintf(myStr, "Login ID: %s\n", manager.login);
            strcat(managerlist, myStr);

            strcpy(writeBuffer, managerlist);
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
            readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
            close(managerFileDescriptor);

            return true;
        }
    }
    strcpy(writeBuffer, "Wrong Option!\nPress any character followed by  Enter key to exit\n");
    // unlocking
    lock.l_type = F_UNLCK;
    if (fcntl(managerFileDescriptor, F_SETLK, &lock) == -1)
    {
        perror("Error releasing the lock");
    }
    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    close(managerFileDescriptor);
    bzero(writeBuffer, sizeof(writeBuffer));
    bzero(readBuffer, sizeof(readBuffer));
    return false;
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

    struct flock lock;
    memset(&lock, 0, sizeof(lock));

    struct manager_struct manager1, manager;
    char managerlist[1000];
    managerlist[0] = '\0';

    // giving all the manager id to be modified as choice
    while (read(managerFileDescriptor, &manager1, sizeof(struct manager_struct)) == sizeof(struct manager_struct))
    {
        if (manager1.active == 1)
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
            sprintf(tempBuffer, "Login ID : %s", manager1.login);
            strcat(managerlist, tempBuffer);
            sprintf(tempBuffer, "\tName : %s ", manager1.name);
            strcat(managerlist, tempBuffer);
            strcat(managerlist, "\n");

            lock.l_type = F_UNLCK;
            if (fcntl(managerFileDescriptor, F_SETLK, &lock) == -1)
            {
                perror("Error releasing the lock");
            }

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
    strcat(writeBuffer, MANAGER_UPDATE);

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
            readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
            if (strcmp(readBuffer, "y") == 0)
            {
                bzero(writeBuffer, sizeof(writeBuffer));
                bzero(readBuffer, sizeof(readBuffer));

                strcpy(writeBuffer, UPDATE);
                writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
                readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
                strcpy(manager1.name, readBuffer);
            }

            // to update address
            bzero(writeBuffer, sizeof(writeBuffer));
            bzero(readBuffer, sizeof(readBuffer));
            strcpy(writeBuffer, MANAGER_ADDRESS);
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
                strcpy(manager1.address, readBuffer);
            }

            // to update email
            bzero(writeBuffer, sizeof(writeBuffer));
            bzero(readBuffer, sizeof(readBuffer));
            strcpy(writeBuffer, MANAGER_EMAIL);
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
                strcpy(manager1.email, readBuffer);
            }

            bzero(writeBuffer, sizeof(writeBuffer));
            bzero(readBuffer, sizeof(readBuffer));

            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_CUR;
            lock.l_start = 0;
            lock.l_len = sizeof(struct manager_struct);

            // Try to acquire the lock in blocking mode
            if (fcntl(managerFileDescriptor, F_SETLKW, &lock) == -1)
            {
                perror("Error locking the file");
                close(managerFileDescriptor);
                exit(EXIT_FAILURE);
            }
            writeBytes = write(managerFileDescriptor, &manager1, sizeof(struct manager_struct));
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
    close(managerFileDescriptor);
    strcpy(writeBuffer, NOT_UPDATED);
    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, strlen(readBuffer));
    return true;
}

bool add_customer_admin(int connectionFileDescriptor)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct customer_struct new_customer, prev_customer;
    // for customer's emp no given in FCFS format

    int customerFileDescriptor = open("CUSTOMER_FILE", O_RDONLY);
    if (customerFileDescriptor == -1 && errno == ENOENT)
    {
        //  customer.txt was never created
        new_customer.cus_no = 1;
    }

    else if (customerFileDescriptor == -1)
    {
        perror("Error while opening customer file");
        return false;
    }

    else
    {
        int offset = lseek(customerFileDescriptor, -sizeof(struct customer_struct), SEEK_END);
        if (offset == -1)
        {
            perror("Error seeking to last customer record!");
            return false;
        }

        struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct customer_struct), getpid()};
        int lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lock);
        if (lockingStatus == -1)
        {
            perror("Error obtaining read lock on customer record!");
            return false;
        }

        readBytes = read(customerFileDescriptor, &prev_customer, sizeof(struct customer_struct));
        if (readBytes == -1)
        {
            perror("Error while reading customer record from file");
            return false;
        }

        lock.l_type = F_UNLCK;
        fcntl(customerFileDescriptor, F_SETLK, &lock);

        close(customerFileDescriptor);

        new_customer.cus_no = prev_customer.cus_no + 1;
    }
    close(customerFileDescriptor);

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
        perror("Error reading customer name");
        return false;
    }

    strcpy(new_customer.name, readBuffer);

    // for customers age
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

    // for customers address
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

    // for customers email
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

    // for customers login
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

    // for customers password
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

    customerFileDescriptor = open("CUSTOMER_FILE", O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);
    if (customerFileDescriptor == -1)
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
    if (fcntl(customerFileDescriptor, F_SETLKW, &lock) == -1)
    {
        perror("Error locking the file");
        close(customerFileDescriptor);
        exit(EXIT_FAILURE);
    }
    // writing the CUSTOMERs data into the file
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(customerFileDescriptor, &new_customer, sizeof(struct customer_struct));
    if (writeBytes == -1)
    {
        perror("Error while writing CUSTOMER record to file!");
        return false;
    }
    // releasing the lock
    lock.l_type = F_UNLCK;
    if (fcntl(customerFileDescriptor, F_SETLK, &lock) == -1)
    {
        perror("Error releasing the lock");
    }
    close(customerFileDescriptor);

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

bool view_customer_details(int connectionFileDescriptor)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    // the customer_FILE has the details of the customer
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

    // getting the login id of all customers to choose from
    while (read(customerFileDescriptor, &customer1, sizeof(struct customer_struct)) == sizeof(struct customer_struct))
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
        sprintf(tempBuffer, "Login: %s ", customer1.login);
        strcat(customerlist, tempBuffer);
        sprintf(tempBuffer, "Name: %s ", customer1.name);
        strcat(customerlist, tempBuffer);
        strcat(customerlist, "\n");

        // Unlock the current customer record after reading
        lock.l_type = F_UNLCK; // Unlock
        if (fcntl(customerFileDescriptor, F_SETLK, &lock) == -1)
        {
            perror("Error unlocking the customer record");
        }
    }
    lseek(customerFileDescriptor, 0, SEEK_SET);

    strcpy(writeBuffer, customerlist);
    strcat(writeBuffer, CUSTOMER_DETAILS);

    // writing the login id of all the customer available blocked or unblocked
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

    bzero(writeBuffer, sizeof(writeBuffer));
    bzero(readBuffer, sizeof(readBuffer));

    // getting details of customer whose login id is provided by user
    while (read(customerFileDescriptor, &customer, sizeof(struct customer_struct)) == sizeof(struct customer_struct))
    {
        if (strcmp(customer.login, login) == 0)
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
            char myStr[100];
            sprintf(myStr, "Cus No: %d\n", customer.cus_no);
            strcat(customerlist, myStr);

            sprintf(myStr, "Name: %s\n", customer.name);
            strcat(customerlist, myStr);

            sprintf(myStr, "Email: %s\n", customer.email);
            strcat(customerlist, myStr);

            sprintf(myStr, "Address: %s\n", customer.address);
            strcat(customerlist, myStr);

            sprintf(myStr, "Age: %s\n", customer.age);
            strcat(customerlist, myStr);

            sprintf(myStr, "Login ID: %s\n", customer.login);
            strcat(customerlist, myStr);

            sprintf(myStr, "Balance: %d\n", customer.balance);
            strcat(customerlist, myStr);

            strcpy(writeBuffer, customerlist);
            strcat(writeBuffer, "Press any character followed by  Enter key \n");
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
            readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
            close(customerFileDescriptor);

            return true;
        }
    }
    strcpy(writeBuffer, "Wrong Option!\nPress any character followed by  Enter key to exit\n");
    // unlocking
    lock.l_type = F_UNLCK;
    if (fcntl(customerFileDescriptor, F_SETLK, &lock) == -1)
    {
        perror("Error releasing the lock");
    }
    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    close(customerFileDescriptor);
    bzero(writeBuffer, sizeof(writeBuffer));
    bzero(readBuffer, sizeof(readBuffer));
    return false;
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

bool add_admin(int connectionFileDescriptor)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct admin_struct new_admin, prev_admin;
    // for Admin's emp no given in FCFS format

    int AdminFileDescriptor = open("ADMIN_FILE", O_RDONLY);
    if (AdminFileDescriptor == -1 && errno == ENOENT)
    {
        //  admin.txt was never created
        new_admin.ad_no = 1;
    }

    else if (AdminFileDescriptor == -1)
    {
        perror("Error while opening admin file");
        return false;
    }

    else
    {
        int offset = lseek(AdminFileDescriptor, -sizeof(struct admin_struct), SEEK_END);
        if (offset == -1)
        {
            perror("Error seeking to last admin record!");
            return false;
        }

        struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct admin_struct), getpid()};
        int lockingStatus = fcntl(AdminFileDescriptor, F_SETLKW, &lock);
        if (lockingStatus == -1)
        {
            perror("Error obtaining read lock on ADMIN record!");
            return false;
        }

        readBytes = read(AdminFileDescriptor, &prev_admin, sizeof(struct admin_struct));
        if (readBytes == -1)
        {
            perror("Error while reading ADMIN record from file");
            return false;
        }

        lock.l_type = F_UNLCK;
        fcntl(AdminFileDescriptor, F_SETLK, &lock);

        close(AdminFileDescriptor);

        new_admin.ad_no = prev_admin.ad_no + 1;
    }
    close(AdminFileDescriptor);

    // for admin name
    writeBytes = write(connectionFileDescriptor, ADMIN_NAME, strlen(ADMIN_NAME));
    if (writeBytes == -1)
    {
        perror("Error writing ADMIN_NAME message to client!");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading ADMIN name");
        return false;
    }

    strcpy(new_admin.name, readBuffer);

    // for ADMINs age
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(connectionFileDescriptor, ADMIN_AGE, strlen(ADMIN_AGE));
    if (writeBytes == -1)
    {
        perror("Error writing ADMIN_AGE message to client!");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    strcpy(new_admin.age, readBuffer);

    // for ADMINs address
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(connectionFileDescriptor, ADMIN_ADDRESS, strlen(ADMIN_ADDRESS));
    if (writeBytes == -1)
    {
        perror("Error writing ADMIN_ADDRESS message to client!");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    strcpy(new_admin.address, readBuffer);

    // for ADMINs email
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(connectionFileDescriptor, ADMIN_EMAIL, strlen(ADMIN_EMAIL));
    if (writeBytes == -1)
    {
        perror("Error writing ADMIN_EMAIL message to client!");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    strcpy(new_admin.email, readBuffer);

    // for ADMINs login
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(connectionFileDescriptor, ADMIN_LOGIN, strlen(ADMIN_LOGIN));
    if (writeBytes == -1)
    {
        perror("Error writing ADMIN_LOGIN message to client!");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    strcpy(new_admin.login, readBuffer);

    // for ADMINs password
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(connectionFileDescriptor, ADMIN_PASSWORD, strlen(ADMIN_PASSWORD));
    if (writeBytes == -1)
    {
        perror("Error writing ADMIN_PASSWORD message to client!");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    strcpy(new_admin.password, readBuffer);

    // make ADMIN active from the beginning
    new_admin.active = true;

    AdminFileDescriptor = open("ADMIN_FILE", O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);
    if (AdminFileDescriptor == -1)
    {
        perror("Error while creating / opening ADMIN file!");
        return false;
    }
    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK;    // Write lock
    lock.l_whence = SEEK_END; // Start from the beginning of the file
    lock.l_start = 0;         // Offset 0
    lock.l_len = sizeof(struct admin_struct);

    // Try to acquire the lock in blocking mode
    if (fcntl(AdminFileDescriptor, F_SETLKW, &lock) == -1)
    {
        perror("Error locking the file");
        close(AdminFileDescriptor);
        exit(EXIT_FAILURE);
    }
    // writing the ADMINs data into the file
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(AdminFileDescriptor, &new_admin, sizeof(struct admin_struct));
    if (writeBytes == -1)
    {
        perror("Error while writing ADMIN record to file!");
        return false;
    }
    // releasing the lock
    lock.l_type = F_UNLCK;
    if (fcntl(AdminFileDescriptor, F_SETLK, &lock) == -1)
    {
        perror("Error releasing the lock");
    }
    close(AdminFileDescriptor);

    // writing a message for add confirmation
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(connectionFileDescriptor, ADMIN_ADDED, strlen(ADMIN_ADDED));
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

bool change_password_admin(int connectionFileDescriptor, char *admin_id)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];
    int adminFileDescriptor = open("ADMIN_FILE", O_RDWR, 0777);
    if (adminFileDescriptor == -1)
    {
        perror("Error while opening file");
        return false;
    }

    struct admin_struct admin1;
    while (read(adminFileDescriptor, &admin1, sizeof(struct admin_struct)) == sizeof(struct admin_struct))
    {
        if (strcmp(admin1.login, admin_id) == 0)
        {
            lseek(adminFileDescriptor, -1 * sizeof(struct admin_struct), SEEK_CUR);

            // to update password
            bzero(writeBuffer, sizeof(writeBuffer));
            bzero(readBuffer, sizeof(readBuffer));

            strcpy(writeBuffer, S_ADMIN_CHANGE_PASSWORD);

            writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));

            strcpy(admin1.password, readBuffer);

            struct flock lock;
            memset(&lock, 0, sizeof(lock));
            lock.l_type = F_WRLCK;    // Write lock
            lock.l_whence = SEEK_CUR; // Start from the current position of the file
            lock.l_start = 0;         // Offset 0
            lock.l_len = sizeof(struct admin_struct);

            // Try to acquire the lock in blocking mode
            if (fcntl(adminFileDescriptor, F_SETLKW, &lock) == -1)
            {
                perror("Error locking the file");
                close(adminFileDescriptor);
                exit(EXIT_FAILURE);
            }

            write(adminFileDescriptor, &admin1, sizeof(struct admin_struct));

            lock.l_type = F_UNLCK; // to unlock file
            if (fcntl(adminFileDescriptor, F_SETLK, &lock) == -1)
            {
                perror("Error releasing the lock");
            }
            close(adminFileDescriptor);
            strcpy(writeBuffer, UPDATED);
            writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            readBytes = read(connectionFileDescriptor, &readBuffer, strlen(readBuffer));
            break;
        }
    }
    return true;
}

#endif