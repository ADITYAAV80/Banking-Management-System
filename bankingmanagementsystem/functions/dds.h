memset(&lock, 0, sizeof(lock));
lock.l_type = F_WRLCK;    // Write lock
lock.l_whence = SEEK_SET; // Start from the beginning of the file
lock.l_start = 0;         // Offset 0
lock.l_len = 0;

if (fcntl(loanFileDescriptor, F_SETLKW, &lock) == -1)
{
    perror("Error locking the file");
    close(loanFileDescriptor);
    exit(EXIT_FAILURE);
}
struct loan_struct loan1;

while (read(loanFileDescriptor, &loan1, sizeof(struct loan_struct)) == sizeof(struct loan_struct))
{
    if (loan1.ln_no == ln_no)
    {
        lseek(loanFileDescriptor, -1 * sizeof(struct loan_struct), SEEK_CUR);

        bzero(writeBuffer, sizeof(writeBuffer));
        bzero(readBuffer, sizeof(readBuffer));

        strcpy(writeBuffer, "Approve/Reject (y/n)");
        writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
        readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer));
        if (strcmp(readBuffer, "y") == 0)
        {
            loan1.approved = true;
        }
        else
        {
            loan1.approved = false;
        }

        bzero(writeBuffer, sizeof(writeBuffer));
        bzero(readBuffer, sizeof(readBuffer));

        write(loanFileDescriptor, &loan1, sizeof(struct loan_struct));

        // unlocking
        lock.l_type = F_UNLCK;
        if (fcntl(loanFileDescriptor, F_SETLK, &lock) == -1)
        {
            perror("Error releasing the lock");
        }
        close(loanFileDescriptor);
        strcpy(writeBuffer, UPDATED);
        writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
        readBytes = read(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
        return true;
    }
}
// if ID not found
//  unlocking
lock.l_type = F_UNLCK;
if (fcntl(loanFileDescriptor, F_SETLK, &lock) == -1)
{
    perror("Error releasing the lock");
}
close(loanFileDescriptor);
strcpy(writeBuffer, NOT_UPDATED);
writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
readBytes = read(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
return true;
