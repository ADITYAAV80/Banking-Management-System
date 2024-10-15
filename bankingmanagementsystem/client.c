#include <stdio.h>      // Import for `printf` & `perror` functions
#include <errno.h>      // Import for `errno` variable
#include <fcntl.h>      // Import for `fcntl` functions
#include <unistd.h>     // Import for `fork`, `fcntl`, `read`, `write`, `lseek, `_exit` functions
#include <sys/types.h>  // Import for `socket`, `bind`, `listen`, `connect`, `fork`, `lseek` functions
#include <sys/socket.h> // Import for `socket`, `bind`, `listen`, `connect` functions
#include <netinet/ip.h> // Import for `sockaddr_in` stucture
#include <string.h>     // Import for string functions
#include <stdlib.h>
#include <termios.h>

void portal_handler(int socketFileDescriptor)
{
    char readBuffer[1000], writeBuffer[1000];
    ssize_t readBytes, writeBytes;
    char tempBuffer[1000];

    do
    {
        bzero(readBuffer, sizeof(readBuffer));
        bzero(tempBuffer, sizeof(tempBuffer));
        readBytes = read(socketFileDescriptor, readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error while reading from client server");
        }
        else if (readBytes == 0)
        {
            printf("Closing the connection\n");
        }
        else
        {
            bzero(writeBuffer, sizeof(writeBuffer));
            printf("%s\n", readBuffer);

            if (strcmp(readBuffer, "Exit") == 0)
            {
                printf("Finished......");
                break;
            }
            if (!strcmp(readBuffer, "PASSWORD:"))
            {
                struct termios oldt, newt;

                // Get the current terminal settings
                tcgetattr(STDIN_FILENO, &oldt);
                newt = oldt;

                // Disable echo
                newt.c_lflag &= ~(ECHO);
                tcsetattr(STDIN_FILENO, TCSANOW, &newt);

                scanf("%[^\n]%*c", writeBuffer);

                tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
            }
            else
            {
                scanf("%[^\n]%*c", writeBuffer);
            }

            writeBytes = write(socketFileDescriptor, writeBuffer, strlen(writeBuffer));
            /*printf("\nyou are writing : %s\n", writeBuffer);*/
            if (writeBytes == -1)
            {
                perror("Error while writing to client socket");
                printf("Closing the connection\n");
                break;
            }
        }
    } while (readBytes > 0);
    close(socketFileDescriptor);
}

void main()
{
    int socketFileDescriptor, connectStatus;
    struct sockaddr_in serverAddress;

    socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFileDescriptor == -1)
    {
        perror("Error while creating server socket!");
        exit(0);
    }

    serverAddress.sin_family = AF_INET;                // IPv4
    serverAddress.sin_port = htons(8080);              // Server will listen to port 8080
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY); // Binds the socket to all interfaces

    connectStatus = connect(socketFileDescriptor, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (connectStatus == -1)
    {
        perror("Error while connecting to server");
        close(socketFileDescriptor);
        exit(0);
    }

    portal_handler(socketFileDescriptor);

    close(socketFileDescriptor);
    exit(0);
}