#ifndef CONSTANTS
#define CONSTANTS

#define CHOICE "(y/n)"
#define EXIT "Enter any character and press ENTER to EXIT\n"

// server
#define INITIAL_PROMPT "*********** BANK EMPLOYEE PORTAL ***********\nPlease specify by pressing:\n1 for ADMIN login\n2 for EMPLOYEE login\n3 for MANAGER login\n4 for CUSTOMER Login\nPress any other number to exit\n"
#define MAX_LOGGED_IN 100

// admin
#define ADMIN_LOGIN_WELCOME "*********** WELCOME ADMIN ***********\nLOGIN_ID"
#define ADMIN_PASSWORD_DISPLAY "PASSWORD:"
#define ADMIN_PASSWORD "admin"
#define ADMIN_LOGIN_SUCCESS "******** WELCOME TO ADMIN PORTAL ********.\nYou are now logged in.\n"
#define ADMIN_MENU "Welcome to Admin Menu.\nChoose the option you want to proceed with\n1. ADD EMPLOYEE\n2. MODIFY EMPLOYEE\n3. ADD MANAGER\n4. MODIFY MANAGER\n5. ADD CUSTOMER\n6. MODIFY CUSTOMER\n7. CHANGE PASSWORD\n8. VIEW EMPLOYEE DETAILS\n9. VIEW MANAGER DETAILS\n10. LOGOUT AND EXIT\nCHOOSE THE OPTION\n"
#define ADMIN_LOGOUT "********** ADMIN LOGGED OUT **********\n"
#define ADMIN_AGAIN "You have entered wrong option\n"

#define EMPLOYEE_NAME "Update employee name\n"
#define EMPLOYEE_AGE "Update employee age\n"
#define EMPLOYEE_ADDRESS "Update the employee address\n"
#define EMPLOYEE_EMAIL "Update Email for the employee\n"
#define EMPLOYEE_LOGIN "Update LOGIN ID for the employee\n"
#define EMPLOYEE_PASSWORD "Update Password for the employee\n"
#define EMPLOYEE_ADDED "Employee updated successfully\nEnter any character and press ENTER to go to ADMIN MENU\n"
#define EMPLOYEE_UPDATE "Enter The LOGIN ID to be updated\n"
#define UPDATE "Enter the data for update\n"
#define UPDATED "The DATA has been UPDATED\nPress any character and press ENTER to exit\n"
#define NOT_UPDATED "The DATA has NOT been UPDATED\nPress any character and press ENTER to exit\n"
#define EMPLOYEE_DETAILS "Enter the Login ID whose details you want\n"

#define MANAGER_NAME "Update manager name\n"
#define MANAGER_AGE "Update manager age\n"
#define MANAGER_ADDRESS "Update the manager address\n"
#define MANAGER_EMAIL "Update Email for the manager\n"
#define MANAGER_LOGIN "Update LOGIN ID for the manager\n"
#define MANAGER_PASSWORD "Update Password for the manager\n"
#define MANAGER_ADDED "Employee updated successfully\nEnter any character and press ENTER to go to ADMIN MENU\n"
#define MANAGER_UPDATE "Enter The LOGIN ID to be updated\n"
#define MANAGER_DETAILS "Enter the Login ID whose details you want\n"

// employee
#define S_EMPLOYEE_LOGIN_WELCOME "*********** WELCOME EMPLOYEE ***********\nPlease authenticate yourself.\nLOGIN_ID"
#define S_EMPLOYEE_PASSWORD "PASSWORD:"

#endif
