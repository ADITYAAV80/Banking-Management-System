#ifndef CONSTANTS
#define CONSTANTS

#include <sys/ipc.h>
#include <sys/shm.h>
#include <crypt.h>
#include <semaphore.h>

#define CHOICE "(y/n)"
#define EXIT "Enter any character and press ENTER to EXIT\n"

// server
#define INITIAL_PROMPT "*********** BANK EMPLOYEE PORTAL ***********\nPlease specify by pressing:\n1 for ADMIN login\n2 for EMPLOYEE login\n3 for MANAGER login\n4 for CUSTOMER Login\nEnter any option or press any other character and press ENTER to EXIT\n"
#define MAX_LOGGED_IN 100

// admin
#define S_ADMIN_LOGIN_WELCOME "*********** WELCOME ADMIN ***********\nLOGIN_ID"
#define S_ADMIN_PASSWORD "PASSWORD:"
#define ADMIN_PASSWORD___ "admin"
#define S_ADMIN_LOGIN_SUCCESS "******** WELCOME TO ADMIN PORTAL ********.\nYou are now logged in.\n"
#define S_ADMIN_MENU "Welcome to Admin Menu.\nChoose the option you want to proceed with\n1. ADD EMPLOYEE\n2. MODIFY EMPLOYEE\n3. ADD MANAGER\n4. MODIFY MANAGER\n5. ADD CUSTOMER\n6. MODIFY CUSTOMER\n7. ADD ADMIN\n8. VIEW EMPLOYEE DETAILS\n9. VIEW MANAGER DETAILS\n10. VIEW  CUSTOMER DETAILS\n11. CHANGE PASSWORD\n12. DEACTIVATE EMPLOYEE\n13. ACTIVATE EMPLOYEE\n14. DEACTIVATE MANAGER\n15. ACTIVATE MANAGER\n16. LOGOUT AND EXIT\nCHOOSE THE OPTION\n"
#define ADMIN_AGAIN "You have entered wrong option\n"
#define S_ADMIN_LOGOUT "ADMIN has been logged out\n"
#define ADMIN_NAME "Update admin name\n"
#define ADMIN_AGE "Update admin age\n"
#define ADMIN_ADDRESS "Update the admin address\n"
#define ADMIN_EMAIL "Update Email for the admin\n"
#define ADMIN_LOGIN "Update LOGIN ID for the admin\n"
#define ADMIN_PASSWORD "Update Password for the admin\n"
#define ADMIN_ADDED "Admin updated successfully\nEnter any character and press ENTER to go to ADMIN MENU\n"
#define ADMIN_UPDATE "Enter The LOGIN ID to be updated\n"
#define ADMIN_DETAILS "Enter the Login ID whose details you want\n"
#define S_ADMIN_CHANGE_PASSWORD "********Enter the new PASSWORD********\nPlease remember it\n"

#define EMPLOYEE_BLOCK "Enter the Login ID of the employee to be BLOCKED\n(you can always UNBLOCK them)\n"
#define EMPLOYEE_BLOCKED "The Employee has been BLOCKED\nPress any key to exit!\n"
#define EMPLOYEE_NOT_BLOCKED "The Employee has NOT been BLOCKED\nWrong LOGIN ID!\nPress any key to exit!\n"

#define EMPLOYEE_ACTIVE "Enter the Login ID of the employee to be ACTIVATED\n"
#define EMPLOYEE_ACTIVATED "The employee has been ACTIVATED\nPress any key to exit!\n"
#define EMPLOYEE_NOT_ACTIVATED "The employee has NOT been ACTIVATED\nWrong LOGIN ID!\nPress any key to exit!\n"

#define MANAGER_BLOCK "Enter the Login ID of the manager to be BLOCKED\n(you can always UNBLOCK them)\n"
#define MANAGER_BLOCKED "The Manager has been BLOCKED\nPress any key to exit!\n"
#define MANAGER_NOT_BLOCKED "The Manager has NOT been BLOCKED\nWrong LOGIN ID!\nPress any key to exit!\n"

#define MANAGER_ACTIVE "Enter the Login ID of the manager to be ACTIVATED\n"
#define MANAGER_ACTIVATED "The manager has been ACTIVATED\nPress any key to exit!\n"
#define MANAGER_NOT_ACTIVATED "The manager has NOT been ACTIVATED\nWrong LOGIN ID!\nPress any key to exit!\n"

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
#define MANAGER_ADDED "Manager updated successfully\nEnter any character and press ENTER to go to ADMIN MENU\n"
#define MANAGER_UPDATE "Enter The LOGIN ID to be updated\n"
#define MANAGER_DETAILS "Enter the Login ID whose details you want\n"

#define CUSTOMER_NAME "Update customer name\n"
#define CUSTOMER_AGE "Update customer age\n"
#define CUSTOMER_ADDRESS "Update the customer address\n"
#define CUSTOMER_EMAIL "Update Email for the customer\n"
#define CUSTOMER_LOGIN "Update LOGIN ID for the customer\n"
#define CUSTOMER_PASSWORD "Update Password for the customer\n"
#define CUSTOMER_ADDED "Customer updated successfully\nEnter any character and press ENTER to go to ADMIN MENU\n"
#define CUSTOMER_UPDATE "Enter The LOGIN ID to be updated\n"
#define CUSTOMER_DETAILS "Enter the Login ID whose details you want\n"

// employee
#define S_EMPLOYEE_LOGIN_WELCOME "*********** WELCOME EMPLOYEE ***********\nPlease authenticate yourself.\nLOGIN_ID"
#define S_EMPLOYEE_PASSWORD "PASSWORD:"
#define S_EMPLOYEE_LOGIN_SUCCESS "****** Welcome to Employee Portal ******\nYou are now logged in\n"
#define S_EMPLOYEE_MENU "Choose the option you want to proceed with\n1. ADD CUSTOMER\n2. MODIFY CUSTOMER\n3. VIEW LOAN APPLICATIONS\n4. ACCEPT REJECT LOAN APPLICATIONS\n5. VIEW CUSTOMER TRANSACTIONS\n6. CHANGE PASSWORD\n7. LOGOUT &EXIT\n"
#define S_EMPLOYEE_LOGOUT "EMPLOYEE has been logged out\n"
#define S_EMPLOYEE_AGAIN "You have entered wrong option\n"
#define S_EMPLOYEE_CHANGE_PASSWORD "********Enter the new PASSWORD********\nPlease remember it\n"

// manager
#define S_MANAGER_LOGIN_WELCOME "*********** WELCOME MANAGER ***********\nPlease authenticate yourself.\nLOGIN_ID"
#define S_MANAGER_PASSWORD "PASSWORD:"
#define S_MANAGER_LOGIN_SUCCESS "****** Welcome to Manager Portal ******\nYou are now logged in\n"
#define S_MANAGER_MENU "Choose the option you want to proceed with\n1. ACTIVATE CUSTOMER ACCOUNTS\n2. DEACTIVATE CUSTOMER ACCOUNTS\n3. ASSIGN LOAN TO EMPLOYEES\n4. CHANGE PASSWORD\n5. SEE CUSTOMER FEEDBACK\n6. LOGOUT & EXIT\n"
#define S_MANAGER_LOGOUT "MANAGER has been logged out\n"
#define S_MANAGER_AGAIN "You have entered wrong option\n"
#define S_MANAGER_CHANGE_PASSWORD "********Enter the new PASSWORD********\nPlease remember it\n"
#define CUSTOMER_BLOCK "Enter the Login ID of the customer to be BLOCKED\n(you can always UNBLOCK them)\n"
#define CUSTOMER_BLOCKED "The Customer has been BLOCKED\nPress any key to exit!\n"
#define CUSTOMER_NOT_BLOCKED "The Customer has NOT been BLOCKED\nWrong LOGIN ID!\nPress any key to exit!\n"
#define CUSTOMER_ACTIVE "Enter the Login ID of the customer to be ACTIVATED\n"
#define CUSTOMER_ACTIVATED "The Customer has been ACTIVATED\nPress any key to exit!\n"
#define CUSTOMER_NOT_ACTIVATED "The Customer has NOT been ACTIVATED\nWrong LOGIN ID!\nPress any key to exit!\n"

// customer
#define S_CUSTOMER_LOGIN_WELCOME "*********** WELCOME CUSTOMER ***********\nPlease authenticate yourself.\nLOGIN_ID"
#define S_CUSTOMER_PASSWORD "PASSWORD:"
#define S_CUSTOMER_LOGIN_SUCCESS "****** Welcome to Employee Portal ******\nYou are now logged in\n"
#define S_CUSTOMER_MENU "Choose the option you want to proceed with\n1. VIEW ACCOUNT BALANCE\n2. DEPOSIT MONEY\n3. WITHDRAW MONEY \n4. TRANSFER FUNDS\n5. APPLY LOAN\n6. CHANGE PASSWORD\n7. ADD FEEDBACK\n8. VIEW_TRANSACTION_HISTORY\n9. LOGOUT & EXIT"
#define S_CUSTOMER_LOGOUT "CUSTOMER has been logged out\n"
#define S_CUSTOMER_AGAIN "You have entered wrong option\n"
#define S_CUSTOMER_CHANGE_PASSWORD "********Enter the new PASSWORD********\nPlease remember it\n"
#define ENTER_FEEDBACK "*******ENTER FEEDBACK [limit to 999 characters]*********\n"
#define FEEDBACK_ADDED "Feedback updated successfully\nEnter any character and press ENTER to go to ADMIN MENU\n"
#define LOAN_AMOUNT "Please Enter Loan Amount:\n"
#define LOAN_ADDED "Loan updated successfully\nEnter any character and press ENTER to go to ADMIN MENU\n"
#define TRANSACTION_AMOUNT "Please enter money to deposit\n"
#define TRANSACTION_AMOUNT_W "Please enter money to withdraw\n"
#define TRANSACTION_AMOUNT_S "Please enter money to send\n"
#define TRANSACTION_ADDED "Transaction updated successfully\nEnter any character and press ENTER to go to ADMIN MENU\n"

#define MULTIPLE_LOGIN "\nUser already logged in \nEnter any character and press ENTER to go to ADMIN MENU\n"
#define MAX_USERS 100
#define USERNAME_LENGTH 30
#define HASH "$6$kavaludaari$"

extern sem_t *account_sem;

#endif
