#ifndef EMPLOYEE_PORTAL
#define EMPLOYEE_PORTAL

#include <unistd.h>
#include "../functions/server_const.h"
#include "../common/login/login_em.h"
#include "../struct/struct_customer.h"
#include "../struct/struct_employee.h"
#include "../struct/struct_loan.h"

bool add_customer_employee(int connectionFileDescriptor);
bool modify_customer_employee(int connectionFileDescriptor);
bool approve_reject_loans(int connectionFileDescriptor, char *employee_id);
bool view_assigned_loan_applications(int connectionFileDescriptor, char *employee_id);
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

    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error in writing\n");
        return false;
    }

    char employee_id[20];
    strcpy(employee_id, readBuffer);

    if (is_user_logged_in(employee_id) != 0)
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
    strcpy(writeBuffer, S_EMPLOYEE_PASSWORD);

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

            readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
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
                view_assigned_loan_applications(connectionFileDescriptor, employee_id);
                break;
            case 4:
                approve_reject_loans(connectionFileDescriptor, employee_id);
                break;
            case 5:
                view_customer_transactions(connectionFileDescriptor);
                break;
            case 6:
                change_password_employee(connectionFileDescriptor, employee_id);
                break;
            case 7:
                logout_user(employee_id);
                return true;
            default:
                logout_user(employee_id);
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

bool add_customer_employee(int connectionFileDescriptor)
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

    // HASHING
    char *hashed_password = crypt(readBuffer, HASH); // Use SHA-512 with a salt
    strcpy(new_customer.password, hashed_password);  // Store hashed password

    // make customer active from the beginning
    new_customer.active = true;

    customerFileDescriptor = open("CUSTOMER_FILE", O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);
    if (customerFileDescriptor == -1)
    {
        perror("Error while creating / opening customer file!");
        return false;
    }
    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK;    // Write lock
    lock.l_whence = SEEK_END; // Start from the beginning of the file
    lock.l_start = 0;         // Offset 0
    lock.l_len = sizeof(struct customer_struct);

    // Try to acquire the lock in blocking mode
    if (fcntl(customerFileDescriptor, F_SETLKW, &lock) == -1)
    {
        perror("Error locking the file");
        close(customerFileDescriptor);
        exit(EXIT_FAILURE);
    }
    // writing the customers data into the file
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(customerFileDescriptor, &new_customer, sizeof(struct customer_struct));
    if (writeBytes == -1)
    {
        perror("Error while writing customer record to file!");
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

bool modify_customer_employee(int connectionFileDescriptor)
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

bool approve_reject_loans(int connectionFileDescriptor, char *employee_id)
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
    LoanList[0] = '\0'; // Initialize LoanList as an empty string

    // Get all the loan IDs to choose from
    while (read(loanFileDescriptor, &loan1, sizeof(struct loan_struct)) == sizeof(struct loan_struct))
    {
        if (strcmp(loan1.employee_id, employee_id) == 0)
        {
            // Lock the current record
            lock.l_type = F_RDLCK;
            lock.l_whence = SEEK_CUR;
            lock.l_start = -sizeof(struct loan_struct);
            lock.l_len = sizeof(struct loan_struct);

            if (fcntl(loanFileDescriptor, F_SETLKW, &lock) == -1)
            {
                perror("Error locking the file");
                close(loanFileDescriptor);
                exit(EXIT_FAILURE);
            }

            // Append loan details to LoanList
            char tempBuffer[1000];
            sprintf(tempBuffer, "Loan no: %d ", loan1.ln_no);
            strcat(LoanList, tempBuffer);
            sprintf(tempBuffer, "Customer: %s ", loan1.customer_id);
            strcat(LoanList, tempBuffer);
            sprintf(tempBuffer, "Amount: %d ", loan1.amount);
            strcat(LoanList, tempBuffer);
            if (loan1.approved == 0)
            {
                strcat(LoanList, "Status: Rejected ");
            }
            else
            {
                strcat(LoanList, "Status: Accepted ");
            }
            strcat(LoanList, "\n");

            // Unlock the current record after reading
            lock.l_type = F_UNLCK;
            if (fcntl(loanFileDescriptor, F_SETLK, &lock) == -1)
            {
                perror("Error releasing the lock");
            }
        }
    }

    // Reset file descriptor to the beginning of the file
    lseek(loanFileDescriptor, 0, SEEK_SET);

    // Send the list of loans to the client
    strcpy(writeBuffer, LoanList);
    strcat(writeBuffer, "Select one of the loans\n");

    // Clear the LoanList buffer after use
    bzero(LoanList, sizeof(LoanList));

    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error while writing to client!");
        close(loanFileDescriptor);
        return false;
    }

    // Clear the read buffer before reading input
    bzero(readBuffer, sizeof(readBuffer));

    // Read the loan number from the client
    readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer) - 1);
    int ln_no = atoi(readBuffer);

    // Reset file pointer for loan details
    while (read(loanFileDescriptor, &loan1, sizeof(struct loan_struct)) == sizeof(struct loan_struct))
    {
        if (loan1.ln_no == ln_no)
        {
            lseek(loanFileDescriptor, -1 * sizeof(struct loan_struct), SEEK_CUR);

            // Clear buffers before next operation
            bzero(writeBuffer, sizeof(writeBuffer));
            bzero(readBuffer, sizeof(readBuffer));

            // Ask the client to approve/reject
            strcpy(writeBuffer, "Approve/Reject (y/n)");
            writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            if (writeBytes == -1)
            {
                perror("Error while writing to client");
                close(loanFileDescriptor);
                return false;
            }

            // Clear buffer and read the response from the client
            bzero(readBuffer, sizeof(readBuffer));
            readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer) - 1);
            if (readBytes == -1)
            {
                perror("Error while reading from client");
                close(loanFileDescriptor);
                return false;
            }

            // Update the loan status based on client response
            if (strcmp(readBuffer, "y") == 0)
            {
                loan1.approved = true;
            }
            else
            {
                loan1.approved = false;
            }

            // Clear buffers before locking and updating the loan record
            bzero(writeBuffer, sizeof(writeBuffer));
            bzero(readBuffer, sizeof(readBuffer));

            // Lock the current record for writing
            memset(&lock, 0, sizeof(lock));
            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_CUR;
            lock.l_start = 0;
            lock.l_len = sizeof(struct loan_struct);

            if (fcntl(loanFileDescriptor, F_SETLKW, &lock) == -1)
            {
                perror("Error locking the file for writing");
                close(loanFileDescriptor);
                exit(EXIT_FAILURE);
            }

            // Write the updated loan record
            write(loanFileDescriptor, &loan1, sizeof(struct loan_struct));

            // Unlock the record after updating
            lock.l_type = F_UNLCK;
            if (fcntl(loanFileDescriptor, F_SETLK, &lock) == -1)
            {
                perror("Error releasing the write lock");
            }

            // Close the file descriptor after processing
            close(loanFileDescriptor);

            // Send the success message to the client
            strcpy(writeBuffer, "The DATA has been UPDATED");
            writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            if (writeBytes == -1)
            {
                perror("Error while writing update message to client");
                return false;
            }

            // Clear buffer and read the client's acknowledgment
            bzero(readBuffer, sizeof(readBuffer));
            readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer) - 1);
            if (readBytes == -1)
            {
                perror("Error while reading acknowledgment from client");
                return false;
            }

            return true;
        }
    }

    // If loan number not found, close file and notify the client
    close(loanFileDescriptor);
    strcpy(writeBuffer, "Loan not found. Operation not updated.");
    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error while writing loan not found message to client");
    }

    // Read acknowledgment from client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer) - 1);
    if (readBytes == -1)
    {
        perror("Error while reading acknowledgment from client");
    }

    return true;
}

bool view_assigned_loan_applications(int connectionFileDescriptor, char *employee_id)
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
    LoanList[0] = '\0'; // Initialize LoanList as empty

    // Get all the loan applications assigned to the employee
    while (read(loanFileDescriptor, &loan1, sizeof(struct loan_struct)) == sizeof(struct loan_struct))
    {
        if (strcmp(loan1.employee_id, employee_id) == 0)
        {
            // Lock the record
            lock.l_type = F_RDLCK;
            lock.l_whence = SEEK_CUR;
            lock.l_start = -sizeof(struct loan_struct); // Lock the current record
            lock.l_len = sizeof(struct loan_struct);

            if (fcntl(loanFileDescriptor, F_SETLKW, &lock) == -1)
            {
                perror("Error locking the file");
                close(loanFileDescriptor);
                return false;
            }

            // Build the list of loan details
            char tempBuffer[1000]; // Temporary buffer to construct the string
            sprintf(tempBuffer, "Loan no: %d ", loan1.ln_no);
            strcat(LoanList, tempBuffer);
            sprintf(tempBuffer, "Customer: %s ", loan1.customer_id);
            strcat(LoanList, tempBuffer);
            sprintf(tempBuffer, "Amount: %d ", loan1.amount);
            strcat(LoanList, tempBuffer);
            if (loan1.approved == 0)
            {
                strcat(LoanList, "Status: Rejected ");
            }
            else
            {
                strcat(LoanList, "Status: Accepted ");
            }
            strcat(LoanList, "\n");

            // Unlock the record after reading
            lock.l_type = F_UNLCK;
            if (fcntl(loanFileDescriptor, F_SETLK, &lock) == -1)
            {
                perror("Error releasing the lock");
            }
        }
    }

    // Reset the file pointer to the beginning
    lseek(loanFileDescriptor, 0, SEEK_SET);

    // Send the loan list to the client
    strcpy(writeBuffer, LoanList);
    strcat(writeBuffer, "Press any character followed by ENTER to exit\n");

    // Clear the LoanList buffer after use
    bzero(LoanList, sizeof(LoanList));

    // Write the loan list and exit prompt to the client
    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error while writing to client!");
        close(loanFileDescriptor);
        return false;
    }

    // Wait for a single character input from the client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error while reading from client");
        close(loanFileDescriptor);
        return false;
    }
    close(loanFileDescriptor);

    return true;
}

bool view_customer_transactions(int connectionFileDescriptor)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    int transactionFileDescriptor = open("TRANSACTION_FILE", O_RDWR, 0777);
    if (transactionFileDescriptor == -1)
    {
        perror("Error while opening Transaction file");
        return false;
    }

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
    bzero(customerlist, sizeof(customerlist)); // Clear customerlist buffer

    // Getting customer IDs to assign the loan
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
            return false;
        }

        char tempBuffer[1000];                 // Temporary buffer for constructing customer list
        bzero(tempBuffer, sizeof(tempBuffer)); // Reset tempBuffer
        sprintf(tempBuffer, "Login: %s ", customer1.login);
        strcat(customerlist, tempBuffer);
        sprintf(tempBuffer, "Name: %s ", customer1.name);
        strcat(customerlist, tempBuffer);
        strcat(customerlist, "\n");

        // Unlocking the file
        lock.l_type = F_UNLCK;
        if (fcntl(customerFileDescriptor, F_SETLK, &lock) == -1)
        {
            perror("Error releasing the lock");
        }
    }
    close(customerFileDescriptor);

    strcpy(writeBuffer, customerlist);
    strcat(writeBuffer, "Select one of the customers to assign/reassign\n");

    bzero(customerlist, sizeof(customerlist)); // Clear customerlist buffer after use

    // Write customer list to client
    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error while writing to file!");
        return false;
    }

    // Reset buffers before reading from client
    bzero(writeBuffer, sizeof(writeBuffer));
    bzero(readBuffer, sizeof(readBuffer));

    // Read customer selection from the client
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    char assigned_customer[20];
    strcpy(assigned_customer, readBuffer);                     // Copy the customer ID
    printf("\nEmployee assigned to: %s\n", assigned_customer); // Log the customer assignment

    memset(&lock, 0, sizeof(lock));

    struct transaction_struct transaction;
    char TransactionList[1000];
    char login[20];
    strcpy(login, assigned_customer);

    bzero(writeBuffer, sizeof(writeBuffer));
    bzero(readBuffer, sizeof(readBuffer));
    TransactionList[0] = '\0';

    char d[2] = "D";
    char c[2] = "C";

    while (read(transactionFileDescriptor, &transaction, sizeof(struct transaction_struct)) == sizeof(struct transaction_struct))
    {
        if ((strcmp(transaction.source_account, login) == 0 && strcmp(transaction.type, d) == 0) ||
            (strcmp(transaction.destination_account, login) == 0) && strcmp(transaction.type, c) == 0)
        {
            lock.l_type = F_RDLCK;
            lock.l_whence = SEEK_CUR;
            lock.l_start = -sizeof(struct transaction_struct);
            lock.l_len = sizeof(struct transaction_struct);

            // Try to acquire the lock in blocking mode
            if (fcntl(transactionFileDescriptor, F_SETLKW, &lock) == -1)
            {
                perror("Error locking the file");
                close(transactionFileDescriptor);
                exit(EXIT_FAILURE);
            }
            // Process the transaction details
            char myStr[100];
            sprintf(myStr, "Transaction No: %d\n", transaction.t_id);
            strcat(TransactionList, myStr);

            sprintf(myStr, "Type: %s\n", transaction.type);
            strcat(TransactionList, myStr);

            sprintf(myStr, "Amount: %d\n", transaction.amount);
            strcat(TransactionList, myStr);

            sprintf(myStr, "Source Account: %s\n", transaction.source_account);
            strcat(TransactionList, myStr);

            sprintf(myStr, "Destination Account: %s\n", transaction.destination_account);
            strcat(TransactionList, myStr);

            sprintf(myStr, "Balance After: %d\n", transaction.balance_after);
            strcat(TransactionList, myStr);

            strcpy(writeBuffer, TransactionList);
            strcat(writeBuffer, "Press any character followed by  Enter key \n");
            writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            if (writeBytes == -1)
            {
                perror("Error while writing to file!");
                return false;
            }

            // unlocking
            lock.l_type = F_UNLCK;
            if (fcntl(transactionFileDescriptor, F_SETLK, &lock) == -1)
            {
                perror("Error releasing the lock");
            }
            readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
            // Clear buffers for next iteration
            bzero(TransactionList, sizeof(TransactionList));
            bzero(writeBuffer, sizeof(writeBuffer));
        }
    }
    strcpy(writeBuffer, "End of records please press any character followed by  Enter key to exit\n");
    // unlocking
    lock.l_type = F_UNLCK;
    if (fcntl(transactionFileDescriptor, F_SETLK, &lock) == -1)
    {
        perror("Error releasing the lock");
    }
    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    close(transactionFileDescriptor);
    bzero(writeBuffer, sizeof(writeBuffer));
    bzero(readBuffer, sizeof(readBuffer));
    return false;
}

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

            // HASHING
            char *hashed_password = crypt(readBuffer, HASH); // Use SHA-512 with a salt
            strcpy(employee1.password, hashed_password);     // Store hashed password

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