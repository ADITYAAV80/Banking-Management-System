#ifndef LOGIN_COMMON
#define LOGIN_COMMON

#include <stdbool.h>
#include <string.h>
#include <stdio.h>

extern char (*shared_logged_in_users)[USERNAME_LENGTH]; // Shared memory for logged-in users
extern int *shared_current_user_count;                  // Shared memory for current user count

// Function to add a user
void add_user(const char *username)
{
    if (*shared_current_user_count < MAX_USERS)
    {
        // Copy the username into the array at the current index
        strncpy(shared_logged_in_users[*shared_current_user_count], username, USERNAME_LENGTH - 1);
        shared_logged_in_users[*shared_current_user_count][USERNAME_LENGTH - 1] = '\0'; // Null-terminate
        (*shared_current_user_count)++;                                                 // Increment user count
        printf("User added. Total users: %d\n", *shared_current_user_count);
    }
    else
    {
        printf("User limit reached. Cannot add more users.\n");
    }
}

// Function to print logged in users
void print_logged_in_users()
{
    printf("Logged in users:\n");
    for (int i = 0; i < *shared_current_user_count; i++)
    {
        printf("%s\n", shared_logged_in_users[i]);
    }
}

// Function to log out a user
void logout_user(const char *username)
{
    for (int i = 0; i < *shared_current_user_count; i++)
    {
        if (strcmp(shared_logged_in_users[i], username) == 0)
        {
            // Shift users left to remove the logged-out user
            for (int j = i; j < *shared_current_user_count - 1; j++)
            {
                strncpy(shared_logged_in_users[j], shared_logged_in_users[j + 1], USERNAME_LENGTH);
            }
            // Clear the last user
            memset(shared_logged_in_users[*shared_current_user_count - 1], 0, USERNAME_LENGTH);
            (*shared_current_user_count)--; // Decrement user count
            printf("%s has logged out. Remaining users: %d\n", username, *shared_current_user_count);
            return;
        }
    }
    printf("User %s is not logged in.\n", username);
}

// Function to check if a user is logged in
bool is_user_logged_in(const char *username)
{
    for (int i = 0; i < *shared_current_user_count; i++)
    {
        printf("Checking logged-in user: %s\n", shared_logged_in_users[i]);
        if (strcmp(shared_logged_in_users[i], username) == 0)
        {
            return true; // User is logged in
        }
    }
    return false; // User is not logged in
}

#endif
