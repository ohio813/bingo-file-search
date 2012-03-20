/*****************************************************************
* Bingo project config tool.
* Author: Xu Cheng
* Copyright (C) 2011-2012, this tool is under the license of GPLv3.
* More detial see ./license/license.txt
*****************************************************************/
#include <string>
#include <vector>
#include <Windows.h>
using namespace std;
using namespace EnvDTE;
using namespace System;
using namespace System::IO;
using namespace System::Collections;

String ^ STDStr2SYSStr (const char* buf)
{
    int len = MultiByteToWideChar (CP_ACP, 0, buf, -1, NULL, 0);
    std::vector<wchar_t> unicode (len);
    MultiByteToWideChar (CP_ACP, 0, buf, -1, &unicode[0], len);
    return gcnew String (std::wstring (unicode.begin(), unicode.end()).c_str());
}

int main (int argc, char *argv[])
{
    String ^ output;
    Queue ^ projfiles = gcnew Queue ();

    for (int i = 0; i < argc;)
    {
        if (string (argv[i]) == "-o")
            output = STDStr2SYSStr (argv[ (++i) ++]);
        else if (string (argv[i]) == "-proj")
        {
            ++i;

            while (i < argc && argv[i][0] != '-')
                projfiles->Enqueue (STDStr2SYSStr (argv[ i ++]));
        }
        else
            ++i;
    }

    System::Environment::CurrentDirectory = Path::GetDirectoryName (output);
    System::Type ^ t = System::Type::GetTypeFromProgID ("VisualStudio.DTE.10.0");

    if (t == nullptr) t = System::Type::GetTypeFromProgID ("VisualStudio.DTE.9.0");

    if (t == nullptr) t = System::Type::GetTypeFromProgID ("VisualStudio.DTE.8.0");

    if (t == nullptr)
    {
        Console::WriteLine ("No Visual Studio has been found.");
        exit (1);
    }

    DTE ^ dte = (DTE ^) Activator::CreateInstance (t, true);
    Solution ^ sln = dte->Solution;
    sln->Create (Path::GetDirectoryName (output), Path::GetFileName (output));

    while (projfiles->Count != 0)
        sln->AddFromFile ( (String ^) projfiles->Dequeue(), false);

    sln->SaveAs (Path::GetFullPath (output));
    dte->Quit();
    return 0;
}