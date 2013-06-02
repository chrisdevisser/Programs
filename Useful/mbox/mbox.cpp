//Copyright Chris DeVisser 2013
//Code may be used freely in personal and commercial environments.
//Compiled with GCC 4.8.0 and MinGW-Builds - http://sourceforge.net/projects/mingwbuilds/
//Uses Boost 1.53.0 - http://www.boost.org/
// Distributed under the Boost Software License, Version 1.0. - http://www.boost.org/LICENSE_1_0.txt

//Uses the following C++11 features:
//std::thread
//uniform initialization
//initializer-list constructors for std::map
//lambdas
//raw string literals
//auto
//ranged-for

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <thread>
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <windows.h>

using namespace boost::program_options;

int main() {
    try {
        //get wide string argv
        auto cmdLine = GetCommandLineW();
        int argc;
        auto argv = CommandLineToArgvW(cmdLine, &argc);

        //message box variables
        std::wstring title;
        std::string iconStr;
        std::string buttonStr;
        int def{};
        std::string modalStr;
        int timeMs;
        std::vector<std::wstring> messageWords;

        //visible argument options
        options_description visible{"Options"};
        visible.add_options()
        ("help,?", "Shows this page.")
        ("title,t", wvalue<std::wstring>(&title)->default_value(L"Message", "Message"), "Sets the title.")
        ("icon,i", value<std::string>(&iconStr), "Sets the icon. Valid values are warning, info, question, and error.")
        ("button,b", value<std::string>(&buttonStr), "Sets the buttons. Valid values are abortretryignore, canceltrycontinue, help, ok, okcancel, retrycancel, yesno, and yesnocancel.")
        ("default,d", value<int>(&def), "Sets the default button. Valid values are 1, 2, and 3.")
        ("modal,m", value<std::string>(&modalStr), "Sets the modality. Valid values are app, system, and task.")
        ("right-justify,j", "Right-justifies the text.")
        ("rtl,r", "Displays text from right to left.")
        ("foreground,f", "Makes the message box the foreground window.")
        ("topmost,p", "Makes the message box always on top.")
        ("time,e", value<int>(&timeMs)->default_value(60000), "Makes the message box disappear after a number of milliseconds.")
        ("timestamp,s", "Shows a timestamp in YYYY-MM-DD HH:MM:SS format below the message.");

        //hidden argument options
        options_description hidden{"Hidden Options"};
        hidden.add_options()
        ("message", wvalue<std::vector<std::wstring>>(&messageWords), "");

        //message is everything not used for an option
        positional_options_description pos;
        pos.add("message", -1);

        //command line options are visible and hidden combined
        options_description cmdOptions;
        cmdOptions.add(visible).add(hidden);

        //process options
        variables_map map;
        store(
            wcommand_line_parser(argc, argv)
            .options(cmdOptions)
            .style(command_line_style::allow_slash_for_short | command_line_style::long_allow_next | command_line_style::default_style)
            .positional(pos).run(), map
        );

        notify(map);

        //display help
        if (map.count("help")) {
            std::cout << visible << "\n\n" <<

R"(Displays a message box with the attributes specified.
Slashes also work for short versions.
Newlines may be inserted with \n.

Sample Usages
=============
mbox hi
mbox /t "Hello world"
mbox /p Fatal error /i error
mbox /j right-justified text
mbox Would you like to quit? /b yesno /d 2

The return value is how the message box was closed. It can be one of the
following values:

 0 - There was an error or help was displayed.
 1 - The ok button was pressed.
 2 - The cancel button was pressed or the message box was cancelled.
 3 - The abort button was pressed.
 4 - The retry button was pressed.
 5 - The ignore button was pressed.
 6 - The yes button was pressed.
 7 - The no button was pressed.
 8 - The message box timed out.
 9 - The help button was pressed.
 10 - The try again button was pressed.
 11 - The continue button was pressed.
)";

            return 0;
        }

        //icon strings to constants
        std::map<std::string, UINT> icons {
            {"warning", MB_ICONWARNING},
            {"info", MB_ICONINFORMATION},
            {"question", MB_ICONQUESTION},
            {"error", MB_ICONERROR},
        };

        //button strings to constants
        std::map<std::string, UINT> buttons {
            {"abortretryignore", MB_ABORTRETRYIGNORE},
            {"canceltrycontinue", MB_CANCELTRYCONTINUE},
            {"help", MB_HELP},
            {"ok", MB_OK},
            {"okcancel", MB_OKCANCEL},
            {"retrycancel", MB_RETRYCANCEL},
            {"yesno", MB_YESNO},
            {"yesnocancel", MB_YESNOCANCEL},
        };

        //default button numbers to constants
        std::map<int, UINT> defaults {
            {1, MB_DEFBUTTON1},
            {2, MB_DEFBUTTON2},
            {3, MB_DEFBUTTON3},
        };

        //modality strings to constants
        std::map<std::string, UINT> modality {
            {"app", MB_APPLMODAL},
            {"system", MB_SYSTEMMODAL},
            {"task", MB_TASKMODAL},
        };

        //final message box variables
        std::wstring text;
        UINT flags{};

        //apply flag options; if not found, 0 value created in map
        flags |= icons[iconStr];
        flags |= buttons[buttonStr];
        flags |= defaults[def];
        flags |= modality[modalStr];

        //check for additional flag options
        if (map.count("right-justify")) {
            flags |= MB_RIGHT;
        }

        if (map.count("rtl")) {
            flags |= MB_RTLREADING;
        }

        if (map.count("foreground")) {
            flags |= MB_SETFOREGROUND;
        }

        if (map.count("topmost")) {
            flags |= MB_TOPMOST;
        }

        //form text string from words
        for (const auto &word : messageWords) {
            text += word + L" ";
        }

        //trim ending whitespace and add newlines
        boost::algorithm::trim_right(text);
        boost::algorithm::replace_all(text, L"\\n", L"\n");

        //add time stamp below message if specified
        if (map.count("timestamp")) {
            std::time_t current = std::time(nullptr);
            std::tm tm = *std::localtime(&current);

            wchar_t timeStr[100]; //std::put_time not yet supported
            std::wcsftime(timeStr, 100, L"\n\n%H:%M:%S %Y-%m-%d", &tm);
            text += timeStr;
        }

        //return when the message box closes or exit via time out in other thread
        std::thread thread{[](int time){Sleep(time); std::exit(8);}, timeMs};
        auto ret = MessageBoxW(nullptr, text.c_str(), title.c_str(), flags);
        thread.detach();
        return ret;
    } catch (error &e) { //output information upon errors
        std::cout << e.what() << "\n";
    }
}

