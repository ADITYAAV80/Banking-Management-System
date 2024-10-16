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
    lock.l_type = F_RDLCK;    // Read lock
    lock.l_whence = SEEK_SET; // Start from the beginning of the file
    lock.l_start = 0;         // Offset 0
    lock.l_len = 0;           // Lock the entire file

    // Try to acquire the lock in blocking mode
    if (fcntl(loanFileDescriptor, F_SETLKW, &lock) == -1)
    {
        perror("Error locking the file");
        close(loanFileDescriptor);
        exit(EXIT_FAILURE);
    }
    struct loan_struct loan1;
    char LoanList[1000];
    LoanList[0] = '\0';

    // getting all the loans ID to choose from
    while (read(loanFileDescriptor, &loan1, sizeof(struct loan_struct)) == sizeof(struct loan_struct))
    {
        if (strcmp(loan1.assigned_employee, employee_id) == 0)
        {
            char tempBuffer[1000]; // Temporary buffer to construct the string
            sprintf(tempBuffer, "Loan no: %d ", loan1.ln_no);
            strcat(LoanList, tempBuffer);
            sprintf(tempBuffer, "Customer: %s ", loan1.customer_id);
            strcat(LoanList, tempBuffer);
            sprintf(tempBuffer, "Amount: %d ", loan1.amount);
            strcat(LoanList, tempBuffer);
            strcat(LoanList, "\n");
        }
    }
    lseek(loanFileDescriptor, 0, SEEK_SET);

    strcpy(writeBuffer, LoanList);
    strcat(writeBuffer, "Press a character followed by enter to exit\n");

    bzero(LoanList, sizeof(LoanList));
    LoanList[0] = '\0';

    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error while writing to file!");
        return false;
    }

    // unlocking
    lock.l_type = F_UNLCK;
    if (fcntl(loanFileDescriptor, F_SETLK, &lock) == -1)
    {
        perror("Error releasing the lock");
    }
    return true;
}