#include <stdio.h> // Import for `printf` & `perror` functions
#include <errno.h> // Import for `errno` variable

#include <fcntl.h>		// Import for `fcntl` functions
#include <unistd.h>		// Import for `fork`, `fcntl`, `read`, `write`, `lseek, `_exit` functions
#include <sys/types.h>	// Import for `socket`, `bind`, `listen`, `accept`, `fork`, `lseek` functions
#include <sys/socket.h> // Import for `socket`, `bind`, `listen`, `accept` functions
#include <netinet/ip.h> // Import for `sockaddr_in` structure

#include <string.h>	 // Import for string functions
#include <stdbool.h> // Import for `bool` data type
#include <stdlib.h>	 // Import for `atoi` functions
#include <sys/ipc.h> // Import for IPC keys
#include <sys/shm.h> // Import for shared memory

#include "functions/server_const.h"
#include "functions/customer.h"
#include "functions/employee.h"
#include "functions/manager.h"
#include "functions/admin.h"

// Global variables for user management in shared memory
char (*shared_logged_in_users)[USERNAME_LENGTH];
int *shared_current_user_count;

void portal_handler(int connectionFileDescriptor)
{
	printf("Connection is made\n");

	char readBuffer[1000], writeBuffer[1000];
	ssize_t readBytes, writeBytes;
	int choice;
	while (1)
	{
		writeBytes = write(connectionFileDescriptor, INITIAL_PROMPT, strlen(INITIAL_PROMPT));
		if (writeBytes == -1)
		{
			perror("Error while sending data to the user");
		}
		else
		{
			bzero(readBuffer, sizeof(readBuffer));
			readBytes = read(connectionFileDescriptor, readBuffer, sizeof(readBuffer));
			if (readBytes == -1)
			{
				perror("Error while reading from client");
			}
			else if (readBytes == 0)
			{
				printf("No data was sent to the server\n");
			}
			else
			{
				choice = atoi(readBuffer);
				switch (choice)
				{
				case 1:
					admin_portal(connectionFileDescriptor);
					break;
				case 2:
					employee_portal(connectionFileDescriptor);
					break;
				case 3:
					manager_portal(connectionFileDescriptor);
					break;
				case 4:
					customer_portal(connectionFileDescriptor);
					break;
				default:
					printf("Closing the connection to server\n");
					bzero(writeBuffer, sizeof(writeBuffer));
					strcpy(writeBuffer, "Exit");
					writeBytes = write(connectionFileDescriptor, writeBuffer, strlen(writeBuffer));
					return;
				}
			}
		}
	}
	printf("Closing the connection to server\n");
}

int main()
{
	int socketFileDescriptor, socketBindStatus, socketListenStatus, connectionFileDescriptor;
	struct sockaddr_in serverAddress, clientAddress;

	// Create shared memory for logged in users and user count
	int shmid = shmget(IPC_PRIVATE, sizeof(char[MAX_USERS][USERNAME_LENGTH]) + sizeof(int), IPC_CREAT | 0666);
	if (shmid == -1)
	{
		perror("Error creating shared memory");
		exit(1);
	}

	shared_logged_in_users = shmat(shmid, NULL, 0);
	shared_current_user_count = (int *)(shared_logged_in_users + MAX_USERS);
	*shared_current_user_count = 0; // Initialize user count

	int option = 1;
	socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(socketFileDescriptor, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
	if (socketFileDescriptor == -1)
	{
		perror("Error while creating server socket!");
		_exit(0);
	}

	serverAddress.sin_family = AF_INET;				   // IPv4
	serverAddress.sin_port = htons(8080);			   // Server will listen to port 8080
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY); // Binds the socket to all interfaces

	socketBindStatus = bind(socketFileDescriptor, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
	if (socketBindStatus == -1)
	{
		perror("Error while binding to server socket!");
		exit(0);
	}

	socketListenStatus = listen(socketFileDescriptor, 20);
	if (socketListenStatus == -1)
	{
		perror("Error while listening for connections on the server socket!");
		close(socketFileDescriptor);
		exit(0);
	}

	int clientSize;
	while (1)
	{
		clientSize = (int)sizeof(clientAddress);
		connectionFileDescriptor = accept(socketFileDescriptor, (struct sockaddr *)&clientAddress, &clientSize);
		if (connectionFileDescriptor == -1)
		{
			perror("Error while connecting to client!");
			close(socketFileDescriptor);
			exit(1);
		}
		else
		{
			if (!fork())
			{ // child process will handle this client socket
				portal_handler(connectionFileDescriptor);
				close(connectionFileDescriptor);
			}
		}
	}
	close(socketFileDescriptor);
	shmdt(shared_logged_in_users); // Detach shared memory
	shmctl(shmid, IPC_RMID, NULL); // Remove shared memory
	_exit(0);
}
