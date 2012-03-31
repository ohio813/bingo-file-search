/**
* Copyright (C) 2011-2012  Xu Cheng, Yang Zhengyu, Zuo Zhiheng, Yao Wenjie
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 3 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.'
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/
///:bingomgr.cpp

#include "global.h"
#include "bingomgr.h"
#include "log.h"
#include <Windows.h>
#include <stdio.h>

BINGO_BEGIN_NAMESPACE

#define ACCESS_READ 1
#define ACCESS_WRITE 2
BOOL AccessCheckIsAdmin (void)
{
    HANDLE hToken;
    DWORD dwStatus;
    DWORD dwAccessMask;
    DWORD dwAccessDesired;
    DWORD dwACLSize;
    DWORD dwStructureSize = sizeof (PRIVILEGE_SET);
    PACL pACL = NULL;
    PSID psidAdmin = NULL;
    BOOL bReturn = FALSE;
    PRIVILEGE_SET ps;
    GENERIC_MAPPING GenericMapping;
    PSECURITY_DESCRIPTOR psdAdmin = NULL;
    SID_IDENTIFIER_AUTHORITY SystemSidAuthority = SECURITY_NT_AUTHORITY;

    __try
    {
        // AccessCheck() requires an impersonation token.
        ImpersonateSelf (SecurityImpersonation);

        if (!OpenThreadToken (GetCurrentThread(), TOKEN_QUERY, FALSE,
                              &hToken))
        {
            if (GetLastError() != ERROR_NO_TOKEN)
                __leave;

            // If the thread does not have an access token, we'll
            // examine the access token associated with the process.
            if (!OpenProcessToken (GetCurrentProcess(), TOKEN_QUERY,
                                   &hToken))
                __leave;
        }

        if (!AllocateAndInitializeSid (&SystemSidAuthority, 2,
                                       SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
                                       0, 0, 0, 0, 0, 0, &psidAdmin))
            __leave;

        psdAdmin = LocalAlloc (LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);

        if (psdAdmin == NULL)
            __leave;

        if (!InitializeSecurityDescriptor (psdAdmin,
                                           SECURITY_DESCRIPTOR_REVISION))
            __leave;

        // Compute size needed for the ACL.
        dwACLSize = sizeof (ACL) + sizeof (ACCESS_ALLOWED_ACE) +
                    GetLengthSid (psidAdmin) - sizeof (DWORD);
        // Allocate memory for ACL.
        pACL = (PACL) LocalAlloc (LPTR, dwACLSize);

        if (pACL == NULL)
            __leave;

        // Initialize the new ACL.
        if (!InitializeAcl (pACL, dwACLSize, ACL_REVISION2))
            __leave;

        dwAccessMask = ACCESS_READ | ACCESS_WRITE;

        // Add the access-allowed ACE to the DACL.
        if (!AddAccessAllowedAce (pACL, ACL_REVISION2,
                                  dwAccessMask, psidAdmin))
            __leave;

        // Set our DACL to the SD.
        if (!SetSecurityDescriptorDacl (psdAdmin, TRUE, pACL, FALSE))
            __leave;

        // AccessCheck is sensitive about what is in the SD; set
        // the group and owner.
        SetSecurityDescriptorGroup (psdAdmin, psidAdmin, FALSE);
        SetSecurityDescriptorOwner (psdAdmin, psidAdmin, FALSE);

        if (!IsValidSecurityDescriptor (psdAdmin))
            __leave;

        dwAccessDesired = ACCESS_READ;
        //
        // Initialize GenericMapping structure even though we
        // won't be using generic rights.
        //
        GenericMapping.GenericRead = ACCESS_READ;
        GenericMapping.GenericWrite = ACCESS_WRITE;
        GenericMapping.GenericExecute = 0;
        GenericMapping.GenericAll = ACCESS_READ | ACCESS_WRITE;

        if (!AccessCheck (psdAdmin, hToken, dwAccessDesired,
                          &GenericMapping, &ps, &dwStructureSize, &dwStatus,
                          &bReturn))
        {
            Log::e (L"AccessCheckIsAdmin() failed with error %lu\n", GetLastError());
            __leave;
        }

        RevertToSelf();
    }
    __finally
    {
        // Cleanup
        if (pACL) LocalFree (pACL);

        if (psdAdmin) LocalFree (psdAdmin);

        if (psidAdmin) FreeSid (psidAdmin);
    }

    return bReturn;
}

BINGO_END_NAMESPACE

///:~
