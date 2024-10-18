bool found_employee = false;

while (read(employeeFileDescriptor, &receiver_employee, sizeof(struct employee_struct)) == sizeof(struct employee_struct))
{
    if (strcmp(receiver_employee.login, receiver_id) == 0)
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