#ifndef CONSTANTS
#define CONSTANTS

// server
#define INITIAL_PROMPT "*********** BANK EMPLOYEE PORTAL ***********\nPlease specify by pressing:\n1 for ADMIN login\n2 for EMPLOYEE login\n3 for MANAGER login\n4 for CUSTOMER Login\nPress any other number to exit\n"
#define MAX_LOGGED_IN 100

// admin
#define ADMIN_LOGIN_WELCOME "*********** WELCOME ADMIN ***********\nLOGIN_ID"
#define ADMIN_PASSWORD_DISPLAY "PASSWORD:"
#define ADMIN_PASSWORD "admin"
#define ADMIN_LOGIN_SUCCESS "******** WELCOME TO ADMIN PORTAL ********.\nYou are now logged in.\n"
#define ADMIN_MENU "Welcome to Admin Menu.\nChoose the option you want to proceed with\n1. ADD STUDENT\n2. VIEW STUDENT DETAILS\n3. ADD FACULTY\n4. VIEW FACULTY DETAILS\n5. ACTIVATE STUDENT\n6. BLOCK STUDENT\n7. MODIFY STUDENT DETAILS\n8. MODIFY FACULTY DETAILS\n9. LOGOUT AND EXIT\nCHOOSE THE OPTION\n"
#define ADMIN_LOGOUT "********** ADMIN LOGGED OUT **********\n"
#define ADMIN_AGAIN "You have entered wrong option\n"

#define EMPLOYEE_NAME "Update employee name\n"
#define EMPLOYEE_AGE "Update employee age\n"
#define EMPLOYEE_ADDRESS "Update the employee address\n"
#define EMPLOYEE_EMAIL "Update Email for the employee\n"
#define EMPLOYEE_LOGIN "Update LOGIN ID for the employee\n"
#define EMPLOYEE_PASSWORD "Update Password for the employee\n"
#define EMPLOYEE_ADDED "Employee updated successfully\nEnter any key to go to ADMIN MENU\n"

// employee
#define S_EMPLOYEE_LOGIN_WELCOME "*********** WELCOME EMPLOYEE ***********\nPlease authenticate yourself.\nLOGIN_ID"
#define S_EMPLOYEE_PASSWORD "PASSWORD:"

#endif
