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
        exit(EXIT_FAILURE);
    }

    struct feedback_struct feedback1, feedback;
    char FeedbackList[1000];
    FeedbackList[0] = '\0';

    while (read(feedbackFileDescriptor, &feedback1, sizeof(struct feedback_struct)) == sizeof(struct feedback_struct))
    {
        char tempBuffer[4000]; // Temporary buffer to construct the string

        sprintf(tempBuffer, "Customer: %s ", feedback1.customer);
        strcat(FeedbackList, tempBuffer);
        sprintf(tempBuffer, "\nFeedback: %s ", feedback1.feedback);
        strcat(FeedbackList, tempBuffer);
        strcat(FeedbackList, "\n");

        strcpy(writeBuffer, tempBuffer);
        strcat(writeBuffer, "Press any character followed by  Enter key to exit\n");
        writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing to file!");
            return false;
        }

        // unlocking
        lock.l_type = F_UNLCK;
        if (fcntl(feedbackFileDescriptor, F_SETLK, &lock) == -1)
        {
            perror("Error releasing the lock");
        }

        close(feedbackFileDescriptor);
        readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer));
        return true;
    }
    strcpy(writeBuffer, "Wrong Option!\n Press any character followed by  Enter key to exit\n");
    // unlocking
    lock.l_type = F_UNLCK;
    if (fcntl(feedbackFileDescriptor, F_SETLK, &lock) == -1)
    {
        perror("Error releasing the lock");
    }
    writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
    readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer));
    close(feedbackFileDescriptor);
    bzero(writeBuffer, sizeof(writeBuffer));
    bzero(readBuffer, sizeof(readBuffer));
    return false;
}
