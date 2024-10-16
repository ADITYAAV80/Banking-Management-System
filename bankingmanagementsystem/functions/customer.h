#ifndef CUSTOMER_PORTAL
#define CUSTOMER_PORTAL

#include <unistd.h>
#include "../functions/server_const.h"
#include "../common/login/login_cu.h"
#include "../struct/struct_customer.h"
// #include "../struct/struct_transaction.h"
#include "../struct/struct_feedback.h"

bool view_account_balance(int connectionFileDescriptor, char *customer_id);
bool deposit_money(int connectionFileDescriptor, char *customer_id);
bool withdraw_money(int connectionFileDescriptor, char *customer_id);
bool transfer_funds(int connectionFileDescriptor, char *customer_id);
bool apply_loan(int connectionFileDescriptor, char *customer_id);
bool change_password_customer(int connectionFileDescriptor, char *customer_id);
bool add_feedback(int connectionFileDescriptor, char *customer_id);
bool view_transaction_history(int connectionFileDescriptor, char *customer_id);

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

    readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error in writing\n");
        return false;
    }

    char customer_id[20];
    strcpy(customer_id, readBuffer);

    if (is_user_logged_in(customer_id) != 0)
    {
        return 0;
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
    readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer));
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

            readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer));
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
                view_transaction_history(connectionFileDescriptor, customer_id);
                break;
            case 9:
                return true;
            default:
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
    int customerFileDescriptor = open("CUSTOMER_FILE", O_RDONLY, 0777);
    if (customerFileDescriptor == -1)
    {
        perror("Error while opening customer file");
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
            char myStr[100];
            sprintf(myStr, "\n Your Balance is: %d\n", customer.balance);

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

bool deposit_money(int connectionFileDescriptor, char *customer_id) {}

bool withdraw_money(int connectionFileDescriptor, char *customer_id) {}

bool transfer_funds(int connectionFileDescriptor, char *customer_id) {}

bool apply_loan(int connectionFileDescriptor, char *customer_id) {}

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
            readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer));

            strcpy(customer1.password, readBuffer);

            write(customerFileDescriptor, &customer1, sizeof(struct customer_struct));

            lock.l_type = F_UNLCK; // to unlock file
            if (fcntl(customerFileDescriptor, F_SETLK, &lock) == -1)
            {
                perror("Error releasing the lock");
            }
            close(customerFileDescriptor);
            strcpy(writeBuffer, UPDATED);
            writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            readBytes = read(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
            break;
        }
    }
    return true;
}
/*
bool add_feedback(int connectionFileDescriptor, char *customer_id) {}
*/

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

bool view_transaction_history(int connectionFileDescriptor, char *customer_id) {}

#endif