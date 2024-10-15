#ifndef LOGIN_COMMON
#define LOGIN_COMMON

// Function to add a user
void add_user(const char *username)
{
    if (current_user_count < MAX_USERS)
    {
        // Copy the username into the array at the current index
        strncpy(logged_in_users[current_user_count], username, USERNAME_LENGTH - 1);
        logged_in_users[current_user_count][USERNAME_LENGTH - 1] = '\0'; // Null-terminate
        current_user_count++;                                            // Increment user count
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
    for (int i = 0; i < current_user_count; i++)
    {
        printf("%s\n", logged_in_users[i]);
    }
}

// Function to logout a user
void logout_user(const char *username)
{
    for (int i = 0; i < current_user_count; i++)
    {
        if (strcmp(logged_in_users[i], username) == 0)
        {
            // Shift users left to remove the logged-out user
            for (int j = i; j < current_user_count - 1; j++)
            {
                strncpy(logged_in_users[j], logged_in_users[j + 1], USERNAME_LENGTH);
            }
            // Clear the last user
            memset(logged_in_users[current_user_count - 1], 0, USERNAME_LENGTH);
            current_user_count--; // Decrement user count
            printf("%s has logged out.\n", username);
            return;
        }
    }
    printf("User %s is not logged in.\n", username);
}

// Function to check if a user is logged in
bool is_user_logged_in(const char *username)
{
    for (int i = 0; i < current_user_count; i++)
    {
        if (strcmp(logged_in_users[i], username) == 0)
        {
            return true; // User found
        }
    }
    return false; // User not found
}

#endif