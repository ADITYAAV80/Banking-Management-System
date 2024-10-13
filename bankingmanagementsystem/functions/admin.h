#ifndef ADMIN_PORTAL
#define ADMIN_PORTAL

#include "../common/login/login_ad.h"

bool admin_portal(int connectionFileDescriptor);
bool add_employee(int connectionFileDescriptor);

bool admin_portal(int connectionFileDescriptor)
{
    bool retval = login_admin(connectionFileDescriptor);
    printf("%d", retval);
}

#endif