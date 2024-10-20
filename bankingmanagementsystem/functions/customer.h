#ifndef CUSTOMER_PORTAL
#define CUSTOMER_PORTAL

#include <unistd.h>
#include <semaphore.h>
#include "../functions/server_const.h"
#include "../common/login/login_cu.h"
#include "../struct/struct_customer.h"
#include "../struct/struct_loan.h"
#include "../struct/struct_transaction.h"
#include "../struct/struct_feedback.h"

bool view_account_balance(int connectionFileDescriptor, char *customer_id);
bool deposit_money(int connectionFileDescriptor, char *customer_id);
bool withdraw_money(int connectionFileDescriptor, char *customer_id);
bool transfer_funds(int connectionFileDescriptor, char *customer_id);
bool apply_loan(int connectionFileDescriptor, char *customer_id);
bool change_password_customer(int connectionFileDescriptor, char *customer_id);
bool add_feedback(int connectionFileDescriptor, char *customer_id);
bool view_transaction_details(int connectionFileDescriptor, char *customer_id);

bool customer_portal(int connectionFileDescriptor)
{
    // creating read and write buffer
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];
    char tempBuffer[1000];

    strcpy(writeBuffer, S_CUSTOMER_LOGIN_WELCOME);
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

    char customer_id[20];
    strcpy(customer_id, readBuffer);

    if (is_user_logged_in(customer_id) != 0)
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
    strcpy(writeBuffer, S_CUSTOMER_PASSWORD);

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
    // printf("%s", customer_id);
    // printf("%s", pass_buffer);

    bzero(writeBuffer, sizeof(writeBuffer));
    if (login_customer(connectionFileDescriptor, customer_id, pass_buffer))
    {
        bzero(readBuffer, sizeof(readBuffer));
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, S_CUSTOMER_LOGIN_SUCCESS);
        while (1)
        {
            strcat(writeBuffer, "\n");
            strcat(writeBuffer, S_CUSTOMER_MENU);
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
            // all the customer options
            int choice = atoi(readBuffer);
            printf("Choice received: %d\n", choice);

            switch (choice)
            {
            case 1:
                view_account_balance(connectionFileDescriptor, customer_id);
                break;
            case 2:
                deposit_money(connectionFileDescriptor, customer_id);
                break;
            case 3:
                withdraw_money(connectionFileDescriptor, customer_id);
                break;
            case 4:
                transfer_funds(connectionFileDescriptor, customer_id);
                break;
            case 5:
                apply_loan(connectionFileDescriptor, customer_id);
                break;
            case 6:
                change_password_customer(connectionFileDescriptor, customer_id);
                break;
            case 7:
                add_feedback(connectionFileDescriptor, customer_id);
                break;
            case 8:
                view_transaction_details(connectionFileDescriptor, customer_id);
                break;
            case 9:
                logout_user(customer_id);
                return true;
            default:
                logout_user(customer_id);
                return true;
            }
        }
    }
    else
    {
        return false; // customer login failed
    }
    return true;
}

bool view_account_balance(int connectionFileDescriptor, char *customer_id)
{

    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    // the CUSTOMER_FILE has the details of the customer
    int customerFileDescriptor = open("CUSTOMER_FILE", O_RDWR, 0777);
    if (customerFileDescriptor == -1)
    {
        perror("Error while opening customer file");
        return false;
    }

    struct customer_struct customer;
    char login[20];
    strcpy(login, customer_id);

    bzero(writeBuffer, sizeof(writeBuffer));
    bzero(readBuffer, sizeof(readBuffer));

    // getting details of employees whose login id is provided by user
    while (read(customerFileDescriptor, &customer, sizeof(struct customer_struct)) == sizeof(struct customer_struct))
    {
        if (strcmp(customer.login, login) == 0)
        {
            struct flock lock;
            memset(&lock, 0, sizeof(lock));
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
            sprintf(myStr, "\nYour Balance is: %d\n", customer.balance);

            strcpy(writeBuffer, myStr);
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

            readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));

            return true;
        }
    }
    strcpy(writeBuffer, "Wrong Option!\n Press any character followed by  Enter key to exit\n");

    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    close(customerFileDescriptor);
    bzero(writeBuffer, sizeof(writeBuffer));
    bzero(readBuffer, sizeof(readBuffer));
    return false;
}

bool deposit_money(int connectionFileDescriptor, char *customer_id)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct transaction_struct new_transaction, prev_transaction;
    struct customer_struct new_customer;

    // Open the transaction file in read-only mode to determine the last transaction ID
    int TransactionFileDescriptor = open("TRANSACTION_FILE", O_RDONLY);
    if (TransactionFileDescriptor == -1 && errno == ENOENT)
    {
        new_transaction.t_id = 1; // No file exists, this is the first transaction
    }
    else if (TransactionFileDescriptor == -1)
    {
        perror("Error while opening transaction file");
        return false;
    }
    else
    {
        struct stat fileStat;
        if (fstat(TransactionFileDescriptor, &fileStat) == -1)
        {
            perror("Error getting file stats");
            close(TransactionFileDescriptor);
            return false;
        }

        // If the file is empty, this is the first transaction
        if (fileStat.st_size < sizeof(struct transaction_struct))
        {
            new_transaction.t_id = 1;
        }
        else
        {
            // Move the file pointer to the last transaction record
            int offset = lseek(TransactionFileDescriptor, -sizeof(struct transaction_struct), SEEK_END);
            if (offset == -1)
            {
                perror("Error seeking to last transaction record");
                close(TransactionFileDescriptor);
                return false;
            }

            // Lock the last transaction record only
            struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct transaction_struct), getpid()};
            if (fcntl(TransactionFileDescriptor, F_SETLKW, &lock) == -1)
            {
                perror("Error obtaining read lock on last transaction record");
                close(TransactionFileDescriptor);
                return false;
            }

            // Read the last transaction record
            readBytes = read(TransactionFileDescriptor, &prev_transaction, sizeof(struct transaction_struct));
            if (readBytes == -1)
            {
                perror("Error reading last transaction record");
                close(TransactionFileDescriptor);
                return false;
            }

            // Unlock the last transaction record
            lock.l_type = F_UNLCK;
            if (fcntl(TransactionFileDescriptor, F_SETLK, &lock) == -1)
            {
                perror("Error unlocking the last transaction record");
            }

            // Set the new transaction ID based on the last transaction
            new_transaction.t_id = prev_transaction.t_id + 1;
        }

        close(TransactionFileDescriptor);
    }

    // Prepare the new transaction
    strcpy(new_transaction.source_account, customer_id);
    strcpy(new_transaction.type, "C"); // Deposit transaction
    strcpy(new_transaction.destination_account, customer_id);

    // Ask client for the amount to deposit
    writeBytes = write(connectionFileDescriptor, TRANSACTION_AMOUNT, strlen(TRANSACTION_AMOUNT));
    if (writeBytes == -1)
    {
        perror("Error writing TRANSACTION_AMOUNT message to client");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading TRANSACTION_AMOUNT");
        return false;
    }
    new_transaction.amount = atoi(readBuffer);

    // Check for invalid amounts
    if (new_transaction.amount < 0)
    {
        strcpy(writeBuffer, "Invalid operation\nPress Character followed by Enter to Exit\n");
        writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error writing invalid amount message");
            return false;
        }
        readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
        return false;
    }

    // Open transaction and customer files for updating
    TransactionFileDescriptor = open("TRANSACTION_FILE", O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);
    if (TransactionFileDescriptor == -1)
    {
        perror("Error opening transaction file");
        return false;
    }

    int CustomerFileDescriptor = open("CUSTOMER_FILE", O_RDWR, S_IRWXU);
    if (CustomerFileDescriptor == -1)
    {
        perror("Error opening customer file");
        close(TransactionFileDescriptor);
        return false;
    }

    struct flock tlock;
    memset(&tlock, 0, sizeof(tlock));

    struct flock clock;
    memset(&clock, 0, sizeof(clock));

    // Lock the customer record to update the balance
    while (read(CustomerFileDescriptor, &new_customer, sizeof(struct customer_struct)) == sizeof(struct customer_struct))
    {
        if (strcmp(new_customer.login, customer_id) == 0)
        {

            // Update the customer's balance
            new_customer.balance += new_transaction.amount;
            new_transaction.balance_after = new_customer.balance;

            // Seek back to the customer record and write the updated balance
            if (lseek(CustomerFileDescriptor, -sizeof(struct customer_struct), SEEK_CUR) == -1)
            {
                perror("Error seeking back to customer record");
                close(CustomerFileDescriptor);
                close(TransactionFileDescriptor);
                return false;
            }

            clock.l_type = F_WRLCK;
            clock.l_whence = SEEK_CUR;
            clock.l_start = 0;
            clock.l_len = sizeof(struct customer_struct);

            if (fcntl(CustomerFileDescriptor, F_SETLKW, &clock) == -1)
            {
                perror("Error locking customer record");
                close(CustomerFileDescriptor);
                close(TransactionFileDescriptor);
                return false;
            }

            tlock.l_type = F_WRLCK;    // Write tlock
            tlock.l_whence = SEEK_END; // Start from the current position of the file
            tlock.l_start = 0;         // Offset 0
            tlock.l_len = sizeof(struct customer_struct);

            if (fcntl(TransactionFileDescriptor, F_SETLKW, &tlock) == -1)
            {
                perror("Error locking customer record");
                close(CustomerFileDescriptor);
                close(TransactionFileDescriptor);
                return false;
            }
            write(CustomerFileDescriptor, &new_customer, sizeof(struct customer_struct));
            write(TransactionFileDescriptor, &new_transaction, sizeof(struct transaction_struct));

            // Unlock the customer record
            clock.l_type = F_UNLCK;
            if (fcntl(CustomerFileDescriptor, F_SETLK, &clock) == -1)
            {
                perror("Error unlocking customer record");
            }

            // Unlock the transaction record
            tlock.l_type = F_UNLCK;
            if (fcntl(TransactionFileDescriptor, F_SETLK, &tlock) == -1)
            {
                perror("Error unlocking customer record");
            }

            break;
        }
    }

    // Notify the client that the balance has been updated
    strcpy(writeBuffer, "Balance updated\nPress character followed by Enter key to exit menu");
    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error writing balance update confirmation");
        return false;
    }
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    close(CustomerFileDescriptor);
    close(TransactionFileDescriptor);
    return true;
}

bool withdraw_money(int connectionFileDescriptor, char *customer_id)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct transaction_struct new_transaction, prev_transaction;
    struct customer_struct new_customer;

    // Open the transaction file in read-only mode to determine the last transaction ID
    int TransactionFileDescriptor = open("TRANSACTION_FILE", O_RDONLY);
    if (TransactionFileDescriptor == -1 && errno == ENOENT)
    {
        new_transaction.t_id = 1; // No file exists, this is the first transaction
    }
    else if (TransactionFileDescriptor == -1)
    {
        perror("Error while opening transaction file");
        return false;
    }
    else
    {
        struct stat fileStat;
        if (fstat(TransactionFileDescriptor, &fileStat) == -1)
        {
            perror("Error getting file stats");
            close(TransactionFileDescriptor);
            return false;
        }

        // If the file is empty, this is the first transaction
        if (fileStat.st_size < sizeof(struct transaction_struct))
        {
            new_transaction.t_id = 1;
        }
        else
        {
            // Move the file pointer to the last transaction record
            int offset = lseek(TransactionFileDescriptor, -sizeof(struct transaction_struct), SEEK_END);
            if (offset == -1)
            {
                perror("Error seeking to last transaction record");
                close(TransactionFileDescriptor);
                return false;
            }

            // Lock the last transaction record only
            struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct transaction_struct), getpid()};
            if (fcntl(TransactionFileDescriptor, F_SETLKW, &lock) == -1)
            {
                perror("Error obtaining read lock on last transaction record");
                close(TransactionFileDescriptor);
                return false;
            }

            // Read the last transaction record
            readBytes = read(TransactionFileDescriptor, &prev_transaction, sizeof(struct transaction_struct));
            if (readBytes == -1)
            {
                perror("Error reading last transaction record");
                close(TransactionFileDescriptor);
                return false;
            }

            // Unlock the last transaction record
            lock.l_type = F_UNLCK;
            if (fcntl(TransactionFileDescriptor, F_SETLK, &lock) == -1)
            {
                perror("Error unlocking the last transaction record");
            }

            // Set the new transaction ID based on the last transaction
            new_transaction.t_id = prev_transaction.t_id + 1;
        }

        close(TransactionFileDescriptor);
    }

    // Prepare the new transaction
    strcpy(new_transaction.source_account, customer_id);
    strcpy(new_transaction.type, "D"); // Deposit transaction
    strcpy(new_transaction.destination_account, customer_id);

    // Ask client for the amount to withdraw
    writeBytes = write(connectionFileDescriptor, TRANSACTION_AMOUNT_W, strlen(TRANSACTION_AMOUNT_W));
    if (writeBytes == -1)
    {
        perror("Error writing TRANSACTION_AMOUNT message to client");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading TRANSACTION_AMOUNT");
        return false;
    }
    new_transaction.amount = atoi(readBuffer);

    // Check for invalid amounts
    if (new_transaction.amount < 0)
    {
        strcpy(writeBuffer, "Invalid operation\nPress Character followed by Enter to Exit\n");
        writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error writing invalid amount message");
            return false;
        }
        readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
        return false;
    }

    // Open transaction and customer files for updating
    TransactionFileDescriptor = open("TRANSACTION_FILE", O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);
    if (TransactionFileDescriptor == -1)
    {
        perror("Error opening transaction file");
        return false;
    }

    int CustomerFileDescriptor = open("CUSTOMER_FILE", O_RDWR, S_IRWXU);
    if (CustomerFileDescriptor == -1)
    {
        perror("Error opening customer file");
        close(TransactionFileDescriptor);
        return false;
    }

    struct flock tlock;
    memset(&tlock, 0, sizeof(tlock));

    struct flock clock;
    memset(&clock, 0, sizeof(clock));

    // Lock the customer record to update the balance
    while (read(CustomerFileDescriptor, &new_customer, sizeof(struct customer_struct)) == sizeof(struct customer_struct))
    {
        if (strcmp(new_customer.login, customer_id) == 0)
        {

            if ((new_customer.balance - new_transaction.amount) < 0)
            {
                strcpy(writeBuffer, "Low Balance\nPress Character followed by Enter to Exit\n");
                writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
                if (writeBytes == -1)
                {
                    perror("Error writing invalid amount message");
                    return false;
                }
                readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
                return false;
            }
            // Update the customer's balance
            new_customer.balance -= new_transaction.amount;
            new_transaction.balance_after = new_customer.balance;

            // Seek back to the customer record and write the updated balance
            lseek(CustomerFileDescriptor, -sizeof(struct customer_struct), SEEK_CUR);

            clock.l_type = F_WRLCK;    // Write clock
            clock.l_whence = SEEK_CUR; // Start from the current position of the file
            clock.l_start = 0;         // Offset 0
            clock.l_len = sizeof(struct customer_struct);

            if (fcntl(CustomerFileDescriptor, F_SETLKW, &clock) == -1)
            {
                perror("Error locking customer record");
                close(CustomerFileDescriptor);
                close(TransactionFileDescriptor);
                return false;
            }

            tlock.l_type = F_WRLCK;    // Write tlock
            tlock.l_whence = SEEK_END; // Start from the current position of the file
            tlock.l_start = 0;         // Offset 0
            tlock.l_len = sizeof(struct transaction_struct);

            if (fcntl(TransactionFileDescriptor, F_SETLKW, &tlock) == -1)
            {
                perror("Error locking customer record");
                close(CustomerFileDescriptor);
                close(TransactionFileDescriptor);
                return false;
            }
            write(CustomerFileDescriptor, &new_customer, sizeof(struct customer_struct));
            write(TransactionFileDescriptor, &new_transaction, sizeof(struct transaction_struct));

            // Unlock the customer record
            clock.l_type = F_UNLCK;
            if (fcntl(CustomerFileDescriptor, F_SETLK, &clock) == -1)
            {
                perror("Error unlocking customer record");
            }

            // Unlock the transaction record
            tlock.l_type = F_UNLCK;
            if (fcntl(TransactionFileDescriptor, F_SETLK, &tlock) == -1)
            {
                perror("Error unlocking customer record");
            }

            break;
        }
    }

    // Notify the client that the balance has been updated
    strcpy(writeBuffer, "Balance updated\nPress character followed by Enter key to exit menu");
    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error writing balance update confirmation");
        return false;
    }
    // Unlock the customer record
    clock.l_type = F_UNLCK;
    if (fcntl(CustomerFileDescriptor, F_SETLK, &clock) == -1)
    {
        perror("Error unlocking customer record");
    }

    // Unlock the transaction record
    tlock.l_type = F_UNLCK;
    if (fcntl(TransactionFileDescriptor, F_SETLK, &tlock) == -1)
    {
        perror("Error unlocking customer record");
    }
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    close(CustomerFileDescriptor);
    close(TransactionFileDescriptor);
    return true;
}

bool apply_loan(int connectionFileDescriptor, char *customer_id)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct loan_struct new_loan, prev_loan;

    int LoanFileDescriptor = open("LOAN_FILE", O_RDONLY);
    if (LoanFileDescriptor == -1 && errno == ENOENT)
    {
        new_loan.ln_no = 1;
    }

    else if (LoanFileDescriptor == -1)
    {
        perror("Error while opening loan file");
        return false;
    }

    else
    {
        int offset = lseek(LoanFileDescriptor, -sizeof(struct loan_struct), SEEK_END);
        if (offset == -1)
        {
            perror("Error seeking to last loan record!");
            return false;
        }

        struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct loan_struct), getpid()};

        int lockingStatus = fcntl(LoanFileDescriptor, F_SETLKW, &lock);
        if (lockingStatus == -1)
        {
            perror("Error obtaining read lock on EMPLOYEE record!");
            return false;
        }

        readBytes = read(LoanFileDescriptor, &prev_loan, sizeof(struct loan_struct));
        if (readBytes == -1)
        {
            perror("Error while reading EMPLOYEE record from file");
            return false;
        }

        lock.l_type = F_UNLCK;
        fcntl(LoanFileDescriptor, F_SETLK, &lock);

        close(LoanFileDescriptor);

        new_loan.ln_no = prev_loan.ln_no + 1;
    }
    close(LoanFileDescriptor);
    strcpy(new_loan.employee_id, "");
    // for name of application
    strcpy(new_loan.customer_id, customer_id);

    writeBytes = write(connectionFileDescriptor, LOAN_AMOUNT, strlen(LOAN_AMOUNT));
    if (writeBytes == -1)
    {
        perror("Error writing LOAN_AMOUNT message to client!");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading LOAN_AMOUNT");
        return false;
    }
    new_loan.amount = atoi(readBuffer);

    // Make Loan not approved from the beginning
    new_loan.approved = false;

    LoanFileDescriptor = open("LOAN_FILE", O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);
    if (LoanFileDescriptor == -1)
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
    if (fcntl(LoanFileDescriptor, F_SETLKW, &lock) == -1)
    {
        perror("Error locking the file");
        close(LoanFileDescriptor);
        exit(EXIT_FAILURE);
    }

    // writing the EMPLOYEEs data into the file
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(LoanFileDescriptor, &new_loan, sizeof(struct loan_struct));
    if (writeBytes == -1)
    {
        perror("Error while writing LOAN record to file!");
        return false;
    }
    // releasing the lock
    lock.l_type = F_UNLCK;
    if (fcntl(LoanFileDescriptor, F_SETLK, &lock) == -1)
    {
        perror("Error releasing the lock");
    }
    close(LoanFileDescriptor);

    // writing a message for add confirmation
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(connectionFileDescriptor, LOAN_ADDED, strlen(LOAN_ADDED));
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

bool add_feedback(int connectionFileDescriptor, char *customer_id)
{

    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];
    struct feedback_struct new_feedback, prev_feedback;

    int FeedbackFileDescriptor = open("FEEDBACK_FILE", O_RDONLY);
    if (FeedbackFileDescriptor == -1 && errno == ENOENT)
    {
        //  feedback.txt was never created
        new_feedback.f_no = 1;
    }

    else if (FeedbackFileDescriptor == -1)
    {
        perror("Error while opening feedback file");
        return false;
    }

    else
    {
        int offset = lseek(FeedbackFileDescriptor, -sizeof(struct feedback_struct), SEEK_END);
        if (offset == -1)
        {
            perror("Error seeking to last feedback record!");
            return false;
        }

        struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct feedback_struct), getpid()};
        int lockingStatus = fcntl(FeedbackFileDescriptor, F_SETLKW, &lock);
        if (lockingStatus == -1)
        {
            perror("Error obtaining read lock on FEEDBACK record!");
            return false;
        }

        readBytes = read(FeedbackFileDescriptor, &prev_feedback, sizeof(struct feedback_struct));
        if (readBytes == -1)
        {
            perror("Error while reading FEEDBACK record from file");
            return false;
        }

        lock.l_type = F_UNLCK;
        fcntl(FeedbackFileDescriptor, F_SETLK, &lock);

        close(FeedbackFileDescriptor);

        new_feedback.f_no = prev_feedback.f_no + 1;
    }
    close(FeedbackFileDescriptor);

    strcpy(new_feedback.customer, customer_id);

    // for  feedback
    writeBytes = write(connectionFileDescriptor, ENTER_FEEDBACK, strlen(ENTER_FEEDBACK));
    if (writeBytes == -1)
    {
        perror("Error writing ENTER_FEEDBACK message to client!");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading FEEDBACK name");
        return false;
    }
    strcpy(new_feedback.feedback, readBuffer);

    FeedbackFileDescriptor = open("FEEDBACK_FILE", O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);
    if (FeedbackFileDescriptor == -1)
    {
        perror("Error while creating / opening FEEDBACK file!");
        return false;
    }

    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK;    // Write lock
    lock.l_whence = SEEK_SET; // Start from the beginning of the file
    lock.l_start = 0;         // Offset 0
    lock.l_len = 0;           // Lock the entire file

    // Try to acquire the lock in blocking mode
    if (fcntl(FeedbackFileDescriptor, F_SETLKW, &lock) == -1)
    {
        perror("Error locking the file");
        close(FeedbackFileDescriptor);
        exit(EXIT_FAILURE);
    }
    // writing the FEEDBACKs data into the file
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(FeedbackFileDescriptor, &new_feedback, sizeof(struct feedback_struct));
    if (writeBytes == -1)
    {
        perror("Error while writing FEEDBACK record to file!");
        return false;
    }
    // releasing the lock
    lock.l_type = F_UNLCK;
    if (fcntl(FeedbackFileDescriptor, F_SETLK, &lock) == -1)
    {
        perror("Error releasing the lock");
    }
    close(FeedbackFileDescriptor);

    // writing a message for add confirmation
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(connectionFileDescriptor, FEEDBACK_ADDED, strlen(FEEDBACK_ADDED));
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

bool change_password_customer(int connectionFileDescriptor, char *customer_id)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];
    int customerFileDescriptor = open("CUSTOMER_FILE", O_RDWR, 0777);
    if (customerFileDescriptor == -1)
    {
        perror("Error while opening file");
        return false;
    }

    struct customer_struct customer1;
    while (read(customerFileDescriptor, &customer1, sizeof(struct customer_struct)) == sizeof(struct customer_struct))
    {
        if (strcmp(customer1.login, customer_id) == 0)
        {
            lseek(customerFileDescriptor, -1 * sizeof(struct customer_struct), SEEK_CUR);

            // to update password
            bzero(writeBuffer, sizeof(writeBuffer));
            bzero(readBuffer, sizeof(readBuffer));

            strcpy(writeBuffer, S_CUSTOMER_CHANGE_PASSWORD);

            writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));

            // HASHING
            char *hashed_password = crypt(readBuffer, HASH); // Use SHA-512 with a salt
            strcpy(customer1.password, hashed_password);

            struct flock lock;
            memset(&lock, 0, sizeof(lock));
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

            write(customerFileDescriptor, &customer1, sizeof(struct customer_struct));

            lock.l_type = F_UNLCK; // to unlock file
            if (fcntl(customerFileDescriptor, F_SETLK, &lock) == -1)
            {
                perror("Error releasing the lock");
            }
            close(customerFileDescriptor);
            strcpy(writeBuffer, UPDATED);
            writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            readBytes = read(connectionFileDescriptor, &readBuffer, strlen(readBuffer));
            break;
        }
    }
    return true;
}

bool view_transaction_details(int connectionFileDescriptor, char *customer_id)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    int transactionFileDescriptor = open("TRANSACTION_FILE", O_RDWR, 0777);
    if (transactionFileDescriptor == -1)
    {
        perror("Error while opening Transaction file");
        return false;
    }

    // Set up the lock for this specific record
    struct flock lock;
    memset(&lock, 0, sizeof(lock));

    struct transaction_struct transaction;
    char TransactionList[1000];

    bzero(writeBuffer, sizeof(writeBuffer));
    bzero(readBuffer, sizeof(readBuffer));
    TransactionList[0] = '\0';

    char d[2] = "D";
    char c[2] = "C";

    while (read(transactionFileDescriptor, &transaction, sizeof(struct transaction_struct)) == sizeof(struct transaction_struct))
    {
        if ((strcmp(transaction.source_account, customer_id) == 0 && strcmp(transaction.type, d) == 0) ||
            (strcmp(transaction.destination_account, customer_id) == 0) && strcmp(transaction.type, c) == 0)
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

bool transfer_funds(int connectionFileDescriptor, char *customer_id)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct transaction_struct sender_transaction, receiver_transaction, prev_transaction;
    struct customer_struct sender_customer, receiver_customer;

    // Consolidated transaction file logic
    int TransactionFileDescriptor = open("TRANSACTION_FILE", O_RDWR | O_CREAT, S_IRWXU);
    if (TransactionFileDescriptor == -1)
    {
        perror("Error while opening transaction file");
        return false;
    }

    // Get the last transaction ID
    off_t fileSize = lseek(TransactionFileDescriptor, 0, SEEK_END);
    if (fileSize < sizeof(struct transaction_struct))
    {
        sender_transaction.t_id = 1; // First transaction
        receiver_transaction.t_id = 2;
    }
    else
    {
        // Move to the last transaction record and read it
        lseek(TransactionFileDescriptor, -sizeof(struct transaction_struct), SEEK_END);
        readBytes = read(TransactionFileDescriptor, &prev_transaction, sizeof(struct transaction_struct));
        if (readBytes == -1)
        {
            perror("Error reading last transaction");
            close(TransactionFileDescriptor);
            return false;
        }
        sender_transaction.t_id = prev_transaction.t_id + 1;
        receiver_transaction.t_id = sender_transaction.t_id + 1;
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

    /*
    char customerlist[1000];
    bzero(customerlist, sizeof(customerlist));
    // Clear customerlist buffer


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
    lseek(customerFileDescriptor, 0, SEEK_SET); // Move the file descriptor pointer to the start

    strcpy(writeBuffer, customerlist);

    */
    bzero(writeBuffer, sizeof(writeBuffer));
    strcat(writeBuffer, "Select one of the customers to send money\n");

    // bzero(customerlist, sizeof(customerlist)); // Clear customerlist buffer after use

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
    char receiver_id[30];
    strcpy(receiver_id, readBuffer); // Copy the reciever ID
    bool found_receiver = false;

    while (read(customerFileDescriptor, &receiver_customer, sizeof(struct customer_struct)) == sizeof(struct customer_struct))
    {
        if (strcmp(receiver_customer.login, receiver_id) == 0)
        {
            found_receiver = true;
            break;
        }
    }
    lseek(customerFileDescriptor, 0, SEEK_SET); // Move the file descriptor pointer to the start

    if (!found_receiver)
    {
        // Write customer list to client
        strcpy(writeBuffer, "No customer found\nPress character followed by Enter to exit\n");
        writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing to file!");
            return false;
        }
        readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
        close(TransactionFileDescriptor);
        close(customerFileDescriptor);
        return false;
    }

    // Prepare the sender transaction
    strcpy(sender_transaction.source_account, customer_id);
    strcpy(sender_transaction.destination_account, receiver_id);
    strcpy(sender_transaction.type, "D"); // Deposit transaction

    // Prepare the new transaction
    strcpy(receiver_transaction.source_account, customer_id);
    strcpy(receiver_transaction.destination_account, receiver_id);
    strcpy(receiver_transaction.type, "C"); // Credit transaction

    // Ask client for the amount to send
    bzero(writeBuffer, sizeof(writeBuffer));
    writeBytes = write(connectionFileDescriptor, TRANSACTION_AMOUNT_S, strlen(TRANSACTION_AMOUNT_S));
    if (writeBytes == -1)
    {
        perror("Error writing TRANSACTION_AMOUNT message to client");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading TRANSACTION_AMOUNT");
        return false;
    }
    sender_transaction.amount = atoi(readBuffer);
    receiver_transaction.amount = atoi(readBuffer);

    printf("\nSender\n%d", sender_transaction.t_id);
    printf("%s\n", sender_transaction.source_account);
    printf("%s\n", sender_transaction.destination_account);
    printf("%s\n", sender_transaction.type);
    printf("%d\n", sender_transaction.amount);

    printf("\nReceiver\n%d", receiver_transaction.t_id);
    printf("%s\n", receiver_transaction.source_account);
    printf("%s\n", receiver_transaction.destination_account);
    printf("%s\n", receiver_transaction.type);
    printf("%d\n", receiver_transaction.amount);

    // Check for invalid amounts
    if (sender_transaction.amount < 0)
    {
        strcpy(writeBuffer, "Invalid operation\nPress Character followed by Enter to Exit\n");
        writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error writing invalid amount message");
            return false;
        }
        readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
        close(customerFileDescriptor);
        close(TransactionFileDescriptor);
        return false;
    }

    // Open transaction and customer files for updating
    TransactionFileDescriptor = open("TRANSACTION_FILE", O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);
    if (TransactionFileDescriptor == -1)
    {
        perror("Error opening transaction file");
        return false;
    }

    struct flock clock;
    memset(&clock, 0, sizeof(clock));

    struct flock tlock;
    memset(&tlock, 0, sizeof(tlock));

    // Assuming sender_id and receiver_id are the IDs of sender and receiver
    int lock_first_fd, lock_second_fd;

    // Determine the order of locking
    if (sender_customer.cus_no < receiver_customer.cus_no)
    {
    }
    else
    {
        while (read(customerFileDescriptor, &sender_customer, sizeof(struct customer_struct)) == sizeof(struct customer_struct))
        {
            if (strcmp(sender_customer.login, customer_id) == 0)
            {

                if ((sender_customer.balance - sender_transaction.amount) < 0)
                {
                    strcpy(writeBuffer, "Low Balance\nPress Character followed by Enter to Exit\n");
                    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
                    if (writeBytes == -1)
                    {
                        perror("Error writing invalid amount message");
                        return false;
                    }
                    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
                    return false;
                }

                // Seek back to the customer record and write the updated balance
                lseek(customerFileDescriptor, -sizeof(struct customer_struct), SEEK_CUR);

                clock.l_type = F_WRLCK;    // Write clock
                clock.l_whence = SEEK_CUR; // Start from the current position of the file
                clock.l_start = 0;         // Offset 0
                clock.l_len = sizeof(struct customer_struct);

                if (fcntl(customerFileDescriptor, F_SETLKW, &clock) == -1)
                {
                    perror("Error locking customer record");
                    close(customerFileDescriptor);
                    close(TransactionFileDescriptor);
                    return false;
                }

                sender_customer.balance -= sender_transaction.amount;
                write(customerFileDescriptor, &sender_customer, sizeof(struct customer_struct));

                // Unlock the customer record
                clock.l_type = F_UNLCK;
                if (fcntl(customerFileDescriptor, F_SETLK, &clock) == -1)
                {
                    perror("Error unlocking customer record");
                    close(customerFileDescriptor);
                    close(TransactionFileDescriptor);
                    return false;
                }

                tlock.l_type = F_WRLCK;    // Write tlock
                tlock.l_whence = SEEK_END; // Start from the current position of the file
                tlock.l_start = 0;         // Offset 0
                tlock.l_len = sizeof(struct transaction_struct);

                if (fcntl(TransactionFileDescriptor, F_SETLKW, &tlock) == -1)
                {
                    perror("Error locking customer record");
                    close(customerFileDescriptor);
                    close(TransactionFileDescriptor);
                    return false;
                }
                sender_transaction.balance_after = sender_customer.balance;
                write(TransactionFileDescriptor, &sender_transaction, sizeof(struct transaction_struct));

                // Unlock the transaction record
                tlock.l_type = F_UNLCK;
                if (fcntl(TransactionFileDescriptor, F_SETLK, &tlock) == -1)
                {
                    perror("Error unlocking customer record");
                    close(customerFileDescriptor);
                    close(TransactionFileDescriptor);
                    return false;
                }
                break;
            }
            else
            {
                // Unlock the customer record
                clock.l_type = F_UNLCK;
                if (fcntl(customerFileDescriptor, F_SETLK, &clock) == -1)
                {
                    perror("Error unlocking customer record");
                    close(customerFileDescriptor);
                    close(TransactionFileDescriptor);
                    return false;
                }
                // Unlock the transaction record
                tlock.l_type = F_UNLCK;
                if (fcntl(TransactionFileDescriptor, F_SETLK, &tlock) == -1)
                {
                    perror("Error unlocking customer record");
                    close(customerFileDescriptor);
                    close(TransactionFileDescriptor);
                    return false;
                }
            }
        }

        while (read(customerFileDescriptor, &receiver_customer, sizeof(struct customer_struct)) == sizeof(struct customer_struct))
        {
            if (strcmp(receiver_customer.login, receiver_id) == 0)
            {

                // Seek back to the customer record and write the updated balance
                lseek(customerFileDescriptor, -sizeof(struct customer_struct), SEEK_CUR);

                struct flock clock;
                memset(&clock, 0, sizeof(clock));
                clock.l_type = F_WRLCK;    // Write clock
                clock.l_whence = SEEK_CUR; // Start from the current position of the file
                clock.l_start = 0;         // Offset 0
                clock.l_len = sizeof(struct customer_struct);

                if (fcntl(customerFileDescriptor, F_SETLKW, &clock) == -1)
                {
                    perror("Error locking customer record");
                    close(customerFileDescriptor);
                    close(TransactionFileDescriptor);
                    return false;
                }

                receiver_customer.balance += receiver_transaction.amount;
                write(customerFileDescriptor, &receiver_customer, sizeof(struct customer_struct));

                // Unlock the customer record
                clock.l_type = F_UNLCK;
                if (fcntl(customerFileDescriptor, F_SETLK, &clock) == -1)
                {
                    perror("Error unlocking customer record");
                    close(customerFileDescriptor);
                    close(TransactionFileDescriptor);
                    return false;
                }

                struct flock tlock;
                memset(&tlock, 0, sizeof(tlock));
                tlock.l_type = F_WRLCK;    // Write tlock
                tlock.l_whence = SEEK_END; // Start from the current position of the file
                tlock.l_start = 0;         // Offset 0
                tlock.l_len = sizeof(struct transaction_struct);

                if (fcntl(TransactionFileDescriptor, F_SETLKW, &tlock) == -1)
                {
                    perror("Error locking customer record");
                    close(customerFileDescriptor);
                    close(TransactionFileDescriptor);
                    return false;
                }
                receiver_transaction.balance_after = receiver_customer.balance;
                write(TransactionFileDescriptor, &receiver_transaction, sizeof(struct transaction_struct));
                // Unlock the transaction record
                tlock.l_type = F_UNLCK;
                if (fcntl(TransactionFileDescriptor, F_SETLK, &tlock) == -1)
                {
                    perror("Error unlocking customer record");
                    close(customerFileDescriptor);
                    close(TransactionFileDescriptor);
                    return false;
                }

                // Notify the client that the balance has been updated
                strcpy(writeBuffer, "Press character followed by Enter key to exit");
                writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
                if (writeBytes == -1)
                {
                    perror("Error writing balance update confirmation");
                    close(customerFileDescriptor);
                    close(TransactionFileDescriptor);
                    return false;
                }
                readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
                close(customerFileDescriptor);
                close(TransactionFileDescriptor);
                return true;
            }
        }

        lseek(customerFileDescriptor, 0, SEEK_SET); // Move the file descriptor pointer to the start
        // Notify the client that the balance has been updated
        strcpy(writeBuffer, "Failed to send\nPress character followed by Enter key to exit menu");
        writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error writing balance update confirmation");
            close(customerFileDescriptor);
            close(TransactionFileDescriptor);
            return false;
        }
        readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
        close(customerFileDescriptor);
        close(TransactionFileDescriptor);
        return true;
    }

    while (read(customerFileDescriptor, &sender_customer, sizeof(struct customer_struct)) == sizeof(struct customer_struct))
    {
        if (strcmp(sender_customer.login, customer_id) == 0)
        {

            if ((sender_customer.balance - sender_transaction.amount) < 0)
            {
                strcpy(writeBuffer, "Low Balance\nPress Character followed by Enter to Exit\n");
                writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
                if (writeBytes == -1)
                {
                    perror("Error writing invalid amount message");
                    return false;
                }
                readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
                return false;
            }

            // Seek back to the customer record and write the updated balance
            lseek(customerFileDescriptor, -sizeof(struct customer_struct), SEEK_CUR);

            clock.l_type = F_WRLCK;    // Write clock
            clock.l_whence = SEEK_CUR; // Start from the current position of the file
            clock.l_start = 0;         // Offset 0
            clock.l_len = sizeof(struct customer_struct);

            if (fcntl(customerFileDescriptor, F_SETLKW, &clock) == -1)
            {
                perror("Error locking customer record");
                close(customerFileDescriptor);
                close(TransactionFileDescriptor);
                return false;
            }

            sender_customer.balance -= sender_transaction.amount;
            write(customerFileDescriptor, &sender_customer, sizeof(struct customer_struct));

            // Unlock the customer record
            clock.l_type = F_UNLCK;
            if (fcntl(customerFileDescriptor, F_SETLK, &clock) == -1)
            {
                perror("Error unlocking customer record");
                close(customerFileDescriptor);
                close(TransactionFileDescriptor);
                return false;
            }

            tlock.l_type = F_WRLCK;    // Write tlock
            tlock.l_whence = SEEK_END; // Start from the current position of the file
            tlock.l_start = 0;         // Offset 0
            tlock.l_len = sizeof(struct transaction_struct);

            if (fcntl(TransactionFileDescriptor, F_SETLKW, &tlock) == -1)
            {
                perror("Error locking customer record");
                close(customerFileDescriptor);
                close(TransactionFileDescriptor);
                return false;
            }
            sender_transaction.balance_after = sender_customer.balance;
            write(TransactionFileDescriptor, &sender_transaction, sizeof(struct transaction_struct));

            // Unlock the transaction record
            tlock.l_type = F_UNLCK;
            if (fcntl(TransactionFileDescriptor, F_SETLK, &tlock) == -1)
            {
                perror("Error unlocking customer record");
                close(customerFileDescriptor);
                close(TransactionFileDescriptor);
                return false;
            }
            break;
        }
        else
        {
            // Unlock the customer record
            clock.l_type = F_UNLCK;
            if (fcntl(customerFileDescriptor, F_SETLK, &clock) == -1)
            {
                perror("Error unlocking customer record");
                close(customerFileDescriptor);
                close(TransactionFileDescriptor);
                return false;
            }
            // Unlock the transaction record
            tlock.l_type = F_UNLCK;
            if (fcntl(TransactionFileDescriptor, F_SETLK, &tlock) == -1)
            {
                perror("Error unlocking customer record");
                close(customerFileDescriptor);
                close(TransactionFileDescriptor);
                return false;
            }
        }
    }

    lseek(customerFileDescriptor, 0, SEEK_SET); // Move the file descriptor pointer to the start

    while (read(customerFileDescriptor, &receiver_customer, sizeof(struct customer_struct)) == sizeof(struct customer_struct))
    {
        if (strcmp(receiver_customer.login, receiver_id) == 0)
        {

            // Seek back to the customer record and write the updated balance
            lseek(customerFileDescriptor, -sizeof(struct customer_struct), SEEK_CUR);

            struct flock clock;
            memset(&clock, 0, sizeof(clock));
            clock.l_type = F_WRLCK;    // Write clock
            clock.l_whence = SEEK_CUR; // Start from the current position of the file
            clock.l_start = 0;         // Offset 0
            clock.l_len = sizeof(struct customer_struct);

            if (fcntl(customerFileDescriptor, F_SETLKW, &clock) == -1)
            {
                perror("Error locking customer record");
                close(customerFileDescriptor);
                close(TransactionFileDescriptor);
                return false;
            }

            receiver_customer.balance += receiver_transaction.amount;
            write(customerFileDescriptor, &receiver_customer, sizeof(struct customer_struct));

            // Unlock the customer record
            clock.l_type = F_UNLCK;
            if (fcntl(customerFileDescriptor, F_SETLK, &clock) == -1)
            {
                perror("Error unlocking customer record");
                close(customerFileDescriptor);
                close(TransactionFileDescriptor);
                return false;
            }

            struct flock tlock;
            memset(&tlock, 0, sizeof(tlock));
            tlock.l_type = F_WRLCK;    // Write tlock
            tlock.l_whence = SEEK_END; // Start from the current position of the file
            tlock.l_start = 0;         // Offset 0
            tlock.l_len = sizeof(struct transaction_struct);

            if (fcntl(TransactionFileDescriptor, F_SETLKW, &tlock) == -1)
            {
                perror("Error locking customer record");
                close(customerFileDescriptor);
                close(TransactionFileDescriptor);
                return false;
            }
            receiver_transaction.balance_after = receiver_customer.balance;
            write(TransactionFileDescriptor, &receiver_transaction, sizeof(struct transaction_struct));
            // Unlock the transaction record
            tlock.l_type = F_UNLCK;
            if (fcntl(TransactionFileDescriptor, F_SETLK, &tlock) == -1)
            {
                perror("Error unlocking customer record");
                close(customerFileDescriptor);
                close(TransactionFileDescriptor);
                return false;
            }
            close(customerFileDescriptor);
            close(TransactionFileDescriptor);
            // Notify the client that the balance has been updated
            strcpy(writeBuffer, "Press character followed by Enter key to exit");
            writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            if (writeBytes == -1)
            {
                perror("Error writing balance update confirmation");
                close(customerFileDescriptor);
                close(TransactionFileDescriptor);
                return false;
            }
            readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
            return true;
        }
    }

    lseek(customerFileDescriptor, 0, SEEK_SET); // Move the file descriptor pointer to the start
    // Notify the client that the balance has been updated
    strcpy(writeBuffer, "Failed to send\nPress character followed by Enter key to exit menu");
    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error writing balance update confirmation");
        close(customerFileDescriptor);
        close(TransactionFileDescriptor);
        return false;
    }
    readBytes = read(connectionFileDescriptor, &readBuffer, sizeof(readBuffer));
    close(customerFileDescriptor);
    close(TransactionFileDescriptor);
    return true;
}

#endif