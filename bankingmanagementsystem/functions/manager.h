#ifndef MANAGER_PORTAL
#define MANAGER_PORTAL

#include <unistd.h>
#include "../functions/server_const.h"
#include "../common/login/login_mg.h"
#include "../struct/struct_customer.h"
#include "../struct/struct_manager.h"
#include "../struct/struct_loan.h"

bool activate_customers_manager(int connectionFileDescriptor, char *manager_id);
bool deactivate_customers_manager(int connectionFileDescriptor, char *manager_id);
bool assign_loans(int connectionFileDescriptor, char *manager_id);
bool review_feedback(int connectionFileDescriptor, char *manager_id);
bool change_password_manager(int connectionFileDescriptor, char *manager_id);

bool manager_portal(int connectionFileDescriptor)
{
    // creating read and write buffer
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];
    char tempBuffer[1000];

    strcpy(writeBuffer, S_MANAGER_LOGIN_WELCOME);
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

    char manager_id[20];
    strcpy(manager_id, readBuffer);

    if (is_user_logged_in(manager_id) != 0)
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
    strcpy(writeBuffer, S_MANAGER_PASSWORD);

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
    // printf("%s", manager_id);
    // printf("%s", pass_buffer);

    bzero(writeBuffer, sizeof(writeBuffer));
    if (login_manager(connectionFileDescriptor, manager_id, pass_buffer))
    {
        bzero(readBuffer, sizeof(readBuffer));
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, S_MANAGER_LOGIN_SUCCESS);
        while (1)
        {
            strcat(writeBuffer, "\n");
            strcat(writeBuffer, S_MANAGER_MENU);
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
            // all the manager options
            int choice = atoi(readBuffer);
            printf("Choice received: %d\n", choice);

            switch (choice)
            {
            case 1:
                activate_customers_manager(connectionFileDescriptor, manager_id);
                break;
            case 2:
                deactivate_customers_manager(connectionFileDescriptor, manager_id);
                break;
            case 3:
                assign_loans(connectionFileDescriptor, manager_id);
                break;
            case 4:
                change_password_manager(connectionFileDescriptor, manager_id);
                break;
            case 5:
                review_feedback(connectionFileDescriptor, manager_id);
                break;
            case 6:
                logout_user(manager_id);
                return true;
            default:
                logout_user(manager_id);
                return true;
            }
        }
    }
    else
    {
        return false; // manager login failed
    }
    return true;
}

bool deactivate_customers_manager(int connectionFileDescriptor, char *manager_id)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];
    int customerFileDescriptor = open("CUSTOMER_FILE", O_RDWR, 0777);
    if (customerFileDescriptor == -1)
    {
        perror("Error while opening customer file");
        return false;
    }
    struct flock lock;
    memset(&lock, 0, sizeof(lock));

    struct customer_struct customer1;
    char customerlist[1000];
    customerlist[0] = '\0';

    // getting all the customers ID to choose from
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
    }
    lseek(customerFileDescriptor, 0, SEEK_SET);

    strcpy(writeBuffer, customerlist);
    strcat(writeBuffer, CUSTOMER_BLOCK);

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

    // blocking the customer
    while (read(customerFileDescriptor, &customer1, sizeof(struct customer_struct)) == sizeof(struct customer_struct))
    {
        if (strcmp(customer1.login, login) == 0 && customer1.active == true)
        {

            // when ID found to be blocked
            customer1.active = false;
            lseek(customerFileDescriptor, -1 * sizeof(struct customer_struct), SEEK_CUR);
            write(customerFileDescriptor, &customer1, sizeof(struct customer_struct));
            strcpy(writeBuffer, CUSTOMER_BLOCKED);
            writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));

            lock.l_type = F_WRLCK;    // Write lock
            lock.l_whence = SEEK_CUR; // Start from the current position of the file
            lock.l_start = 0;         // Offset 0
            lock.l_len = sizeof(struct customer_struct);

            // Try to acquire the lock in blocking mode
            if (fcntl(customerFileDescriptor, F_SETLKW, &lock) == -1)
            {
                perror("Error locking the file");
                close(customerFileDescriptor);
                exit(EXIT_FAILURE);
            }

            readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
            // unlocking
            lock.l_type = F_UNLCK;
            if (fcntl(customerFileDescriptor, F_SETLK, &lock) == -1)
            {
                perror("Error releasing the lock");
            }
            close(customerFileDescriptor);
            return true;
        }
    }
    // if CUSTOMER already blocked or not blocked
    strcpy(writeBuffer, CUSTOMER_NOT_BLOCKED);
    close(customerFileDescriptor);
    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    return false;
}

bool activate_customers_manager(int connectionFileDescriptor, char *manager_id)
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

    struct customer_struct customer1;
    char customerlist[1000];
    customerlist[0] = '\0';

    // getting all the customers ID to choose from
    while (read(customerFileDescriptor, &customer1, sizeof(struct customer_struct)) == sizeof(struct customer_struct))
    {
        if (customer1.active == 0)
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
    }
    lseek(customerFileDescriptor, 0, SEEK_SET);

    strcpy(writeBuffer, customerlist);
    strcat(writeBuffer, CUSTOMER_ACTIVE);

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

    // searching the login id and activating them
    while (read(customerFileDescriptor, &customer1, sizeof(struct customer_struct)) == sizeof(struct customer_struct))
    {
        if (strcmp(customer1.login, login) == 0)
        {
            customer1.active = true;
            lseek(customerFileDescriptor, -1 * sizeof(struct customer_struct), SEEK_CUR);
            write(customerFileDescriptor, &customer1, sizeof(struct customer_struct));
            strcpy(writeBuffer, CUSTOMER_ACTIVATED);
            writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));

            lock.l_type = F_WRLCK;    // Write lock
            lock.l_whence = SEEK_CUR; // Start from the current position of the file
            lock.l_start = 0;         // Offset 0
            lock.l_len = sizeof(struct customer_struct);

            // Try to acquire the lock in blocking mode
            if (fcntl(customerFileDescriptor, F_SETLKW, &lock) == -1)
            {
                perror("Error locking the file");
                close(customerFileDescriptor);
                exit(EXIT_FAILURE);
            }

            readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
            // unlocking
            lock.l_type = F_UNLCK;
            if (fcntl(customerFileDescriptor, F_SETLK, &lock) == -1)
            {
                perror("Error releasing the lock");
            }
            close(customerFileDescriptor);
            return true;
        }
    }
    // if id not found in blocked list
    strcpy(writeBuffer, CUSTOMER_NOT_ACTIVATED);

    close(customerFileDescriptor);
    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    return false;
}

bool assign_loans(int connectionFileDescriptor, char *manager_id)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    int loanFileDescriptor = open("LOAN_FILE", O_RDWR, 0777);
    if (loanFileDescriptor == -1)
    {
        perror("Error while opening loan file");
        return false;
    }

    struct flock lock;
    memset(&lock, 0, sizeof(lock));

    struct loan_struct loan1;
    char LoanList[1000];
    bzero(LoanList, sizeof(LoanList)); // Clear the LoanList buffer

    // Getting all the loans ID to choose from
    while (read(loanFileDescriptor, &loan1, sizeof(struct loan_struct)) == sizeof(struct loan_struct))
    {
        lock.l_type = F_RDLCK;
        lock.l_whence = SEEK_CUR;
        lock.l_start = -sizeof(struct loan_struct);
        lock.l_len = sizeof(struct loan_struct);

        // Try to acquire the lock in blocking mode
        if (fcntl(loanFileDescriptor, F_SETLKW, &lock) == -1)
        {
            perror("Error locking the file");
            close(loanFileDescriptor);
            return false;
        }

        char tempBuffer[1000];                 // Temporary buffer to construct the string
        bzero(tempBuffer, sizeof(tempBuffer)); // Reset tempBuffer
        sprintf(tempBuffer, "Loan no: %d ", loan1.ln_no);
        strcat(LoanList, tempBuffer);
        sprintf(tempBuffer, "Customer: %s ", loan1.customer_id);
        strcat(LoanList, tempBuffer);
        sprintf(tempBuffer, "Amount: %d ", loan1.amount);
        strcat(LoanList, tempBuffer);
        if (strlen(loan1.employee_id) == 0)
        {
            strcat(LoanList, "Employee : None");
        }
        else
        {
            sprintf(tempBuffer, "Employee: %s ", loan1.employee_id);
            strcat(LoanList, tempBuffer);
        }
        strcat(LoanList, "\n");
        // Unlocking the file
        lock.l_type = F_UNLCK;
        if (fcntl(loanFileDescriptor, F_SETLK, &lock) == -1)
        {
            perror("Error releasing the lock");
        }
    }
    lseek(loanFileDescriptor, 0, SEEK_SET); // Reset file pointer to the start

    strcpy(writeBuffer, LoanList);
    strcat(writeBuffer, "Select one of the loans to assign/reassign\n");

    bzero(LoanList, sizeof(LoanList)); // Clear LoanList buffer after use

    // Write loan list to client
    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error while writing to file!");
        return false;
    }

    // Reset buffers before reading from client
    bzero(writeBuffer, sizeof(writeBuffer));
    bzero(readBuffer, sizeof(readBuffer));

    // Read loan selection from the client
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer) - 1); // Prevent buffer overflow
    if (readBytes == -1)
    {
        perror("Error while reading from client!");
        return false;
    }
    readBuffer[readBytes] = '\0'; // Null-terminate the input to make it a valid string

    int ln_no = atoi(readBuffer);  // Convert input to integer
    printf("Loan id %d\n", ln_no); // Log loan ID

    // Reset buffers
    bzero(writeBuffer, sizeof(writeBuffer));
    bzero(readBuffer, sizeof(readBuffer));

    int employeeFileDescriptor = open("EMPLOYEE_FILE", O_RDWR, 0777);
    if (employeeFileDescriptor == -1)
    {
        perror("Error while opening employee file");
        return false;
    }

    struct employee_struct employee1;
    char employeelist[1000];
    bzero(employeelist, sizeof(employeelist)); // Clear employeelist buffer

    // Getting employee IDs to assign the loan
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
            return false;
        }

        char tempBuffer[1000];                 // Temporary buffer for constructing employee list
        bzero(tempBuffer, sizeof(tempBuffer)); // Reset tempBuffer
        sprintf(tempBuffer, "Login: %s ", employee1.login);
        strcat(employeelist, tempBuffer);
        sprintf(tempBuffer, "Name: %s ", employee1.name);
        strcat(employeelist, tempBuffer);
        strcat(employeelist, "\n");

        // Unlocking the file
        lock.l_type = F_UNLCK;
        if (fcntl(employeeFileDescriptor, F_SETLK, &lock) == -1)
        {
            perror("Error releasing the lock");
        }
    }
    lseek(employeeFileDescriptor, 0, SEEK_SET); // Reset file pointer to the start

    strcpy(writeBuffer, employeelist);
    strcat(writeBuffer, "Select one of the employees to assign/reassign\n");

    bzero(employeelist, sizeof(employeelist)); // Clear employeelist buffer after use

    // Write employee list to client
    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error while writing to file!");
        return false;
    }

    // Reset buffers before reading from client
    bzero(writeBuffer, sizeof(writeBuffer));
    bzero(readBuffer, sizeof(readBuffer));

    // Read employee selection from the client
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer)); // Prevent buffer overflow
    char assigned_employee[20];
    strcpy(assigned_employee, readBuffer); // Copy the employee ID

    bool found_employee = false;

    while (read(employeeFileDescriptor, &employee1, sizeof(struct employee_struct)) == sizeof(struct employee_struct))
    {
        if (strcmp(employee1.login, assigned_employee) == 0)
        {
            found_employee = true;
            break;
        }
    }
    lseek(employeeFileDescriptor, 0, SEEK_SET); // Move the file descriptor pointer to the start

    if (!found_employee)
    {
        // Write employee list to client
        strcpy(writeBuffer, "No employee found\nPress character followed by Enter to exit\n");
        writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing to file!");
            return false;
        }
        readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
        return false;
    }

    printf("\nEmployee assigned to: %s\n", assigned_employee); // Log the employee assignment

    // Reset buffers
    bzero(writeBuffer, sizeof(writeBuffer));
    bzero(readBuffer, sizeof(readBuffer));

    // Find and update the loan with the selected loan number and assign the employee
    while (read(loanFileDescriptor, &loan1, sizeof(struct loan_struct)) == sizeof(struct loan_struct))
    {
        if (loan1.ln_no == ln_no)
        {
            lseek(loanFileDescriptor, -1 * sizeof(struct loan_struct), SEEK_CUR);

            strcpy(loan1.employee_id, assigned_employee); // Assign the employee to the loan

            lock.l_type = F_WRLCK; // Write lock
            lock.l_whence = SEEK_CUR;
            lock.l_start = 0; // Offset 0
            lock.l_len = sizeof(struct loan_struct);

            if (fcntl(loanFileDescriptor, F_SETLKW, &lock) == -1)
            {
                perror("Error locking the file");
                close(loanFileDescriptor);
                return false;
            }

            // Write updated loan back to the file
            write(loanFileDescriptor, &loan1, sizeof(struct loan_struct));

            lock.l_type = F_UNLCK; // Unlock the file after updating
            if (fcntl(loanFileDescriptor, F_SETLK, &lock) == -1)
            {
                perror("Error releasing the lock");
            }

            close(loanFileDescriptor); // Close loan file after updating

            strcpy(writeBuffer, UPDATED); // Notify client about the update
            writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer) - 1); // Wait for any input to proceed
            readBuffer[readBytes] = '\0';                                                    // Null-terminate the input
            break;
        }
    }
    return true;
}

bool review_feedback(int connectionFileDescriptor, char *manager_id)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    // the FEEDBACK_FILE has the details of the feedback
    int feedbackFileDescriptor = open("FEEDBACK_FILE", O_RDONLY, 0777);
    if (feedbackFileDescriptor == -1)
    {
        perror("Error while opening feedback file");
        return false;
    }

    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_RDLCK;    // Read lock
    lock.l_whence = SEEK_SET; // Start from the beginning of the file
    lock.l_start = 0;         // Offset 0
    lock.l_len = 0;           // Lock the entire file

    // Try to acquire the lock in blocking mode
    if (fcntl(feedbackFileDescriptor, F_SETLKW, &lock) == -1)
    {
        perror("Error locking the file");
        close(feedbackFileDescriptor);
        return false;
    }

    struct feedback_struct feedback1;
    char FeedbackList[2000]; // Increased size to accommodate larger strings
    FeedbackList[0] = '\0';  // Initialize the feedback list string

    while (read(feedbackFileDescriptor, &feedback1, sizeof(struct feedback_struct)) == sizeof(struct feedback_struct))
    {
        char myStr[1100];
        sprintf(myStr, "Customer: %s\n", feedback1.customer);
        strcat(FeedbackList, myStr);

        sprintf(myStr, "Feedback: %s\n", feedback1.feedback);
        strcat(FeedbackList, myStr);

        strcpy(writeBuffer, FeedbackList);
        strcat(writeBuffer, "To see next record press a character followed by Enter\n");
        writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing to connection!");
            close(feedbackFileDescriptor);
            return false;
        }
        readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
        bzero(FeedbackList, sizeof(FeedbackList));
        bzero(writeBuffer, sizeof(writeBuffer));
    }

    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error while writing to connection!");
        close(feedbackFileDescriptor);
        return false;
    }

    // Release the lock after reading all feedback
    lock.l_type = F_UNLCK;
    if (fcntl(feedbackFileDescriptor, F_SETLK, &lock) == -1)
    {
        perror("Error releasing the lock");
    }

    close(feedbackFileDescriptor);
    return true;
}

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

            // HASHING
            char *hashed_password = crypt(readBuffer, HASH); // Use SHA-512 with a salt
            strcpy(manager1.password, hashed_password);      // Store hashed password

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