# Banking Management System
Welcome to the Banking management system project. This document will guide you through the roles and functionality

### Roles
1. Admin
2. Customer
3. Manager
4. Employee

### Project Structure
1. common/login - houses all login functionality
    * login_ad.h - login function for admin
    * login_cu.h - login function for customer
    * login_em.h - login function for employee
    * login_mg.h - login function for manager
    * login_common.h - houses all common functionality required for above 4 header files such as logout, checking if user is already logged in etc
2. functions - all portal functions are handled here
    * admin.h - handles all admin functions
        * add_employee()
        * modify_employee()
        * view_employee_details()
        * add_manager()
        * modify_manager()
        * view_manager_details()
        * add_customer_admin()
        * modify_customer_admin()
        * view_customer_details()
        * change_password_admin()
        * add_admin()
    * customer.h - handles all customer functions
        * view_account_balance()
        * deposit_money()
        * withdraw_money()
        * transfer_funds()
        * apply_loan()
        * change_password_customer()
        * add_feedback()
        * view_transaction_details()
    * employee.h - handles all employee functions
        * add_customer_employee()
        * modify_customer_employee()
        * approve_reject_loans()
        * view_assigned_loan_applications()
        * view_customer_transactions()
        * change_password_employee() 
    * manager.h - handles all the manager functions
        * activate_customers_manager()
        * deactivate_customers_manager()
        * assign_loans()
        * review_feeback()
        * change_password()
3. struct - stores all the structures
    * struct_admin.h
    * struct_customer.h
    * struct_employee.h
    * struct_feedback.h
    * struct_loan.h
    * struct_manager.h
    * struct_transactions.h
4. files - stores all information
5. server & client functions - server.c and client.c with .out files

### How to start server and Client
* Go to directory where you have stored the file
    * To compile server - gcc server.c -lcrypt -o server.out
    * To execute - ./server.out
    * To compile client - gcc client.c -o client.out
    * To execute - ./client.out
    * user super admin credentialls initially to create additional admins, customer, employees
    
### Super Admin Credentials
Admin : admin
Password : admin
