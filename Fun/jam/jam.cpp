#define _WIN32_WINNT 0x0501

#include <iostream>
#include <stdexcept>
#include <string>

#include <boost/program_options.hpp>
#include <shlwapi.h>
#include <tlhelp32.h>
#include <windows.h>

std::runtime_error MakeError(const std::string &functionName, bool getLastError = true) {
    return std::runtime_error(functionName + " failed with error code " + (getLastError ? std::to_string(GetLastError()) : ""));
}

std::wstring GetDllPath();
void InjectDll(HANDLE);
void AdjustPrivileges();
DWORD PidFromName(const std::wstring &);

int main() {
    using namespace boost::program_options;

    try {
        AdjustPrivileges();
    } catch (std::runtime_error &ex) {
        std::cout << ex.what() << "\n";
    }

    auto cmdLine = GetCommandLineW();
    int argc;
    auto argv = CommandLineToArgvW(cmdLine, &argc);

    std::wstring process;

    options_description options{"Options"};
    options.add_options()
    ("help,?", "Shows this message.")
    ("process,p", wvalue<std::wstring>(&process)->required(), "The name or PID of the process to jam. The PID is tried first. To make it only a name, specify the nopid option.")
    ("nopid,n", "Specifies that the process option is a name. It will not be tried as a PID if the name fails.");

    positional_options_description pos;
    pos.add("process", 1);

    try {
        variables_map map;
        store(
            wcommand_line_parser(argc, argv)
            .options(options)
            .style((command_line_style::default_style & ~command_line_style::allow_guessing) | command_line_style::allow_slash_for_short | command_line_style::allow_long_disguise)
            .positional(pos)
            .run(), map
        );

        if (map.count("help")) {
            std::cout << options << "\n\n";
            std::cout <<
R"(Creates one thread in a process to make it use CPU.
jam.dll must be in the same directory.

Sample Usage
============

jam 1234
jam notepad
jam /nopid 5678
jam "process with spaces"

Returns 0 on success and 1 on failure.
)";
            return 0;
        }

        notify(map);

        DWORD pid = 0;

        if (!map.count("nopid")) {
            try {
                pid = std::stoi(process);
            } catch (std::exception &) {}
        }

        if (!pid) {
            process += (process.rfind(L".exe") == std::wstring::npos) ? L".exe" : L"";
            pid = PidFromName(process);
        }

        HANDLE proc = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pid);
        if (!proc) {
            throw MakeError("OpenProcess");
        }

        InjectDll(proc);
    } catch (std::exception &ex) {
        std::cout << ex.what() << "\n";
        return 1;
    }
}

std::wstring GetDllPath() {
    std::wstring ret(MAX_PATH, '\0');
    const auto copied = GetModuleFileNameW(nullptr, &ret[0], ret.size());

    if (!copied) {
        throw MakeError("GetModuleFileName");
    }

    if (!PathRemoveFileSpecW(&ret[0])) {
        throw MakeError("PathRemoveFileSpec", false);
    }

    ret = ret.c_str();
    return ret + L"/jam.dll";
}

void InjectDll(HANDLE process) {
    const std::wstring dllPath = GetDllPath();
    const auto size = dllPath.size() * sizeof(wchar_t);

    void *remoteString = VirtualAllocEx(process, nullptr, size, MEM_COMMIT, PAGE_READWRITE);
    if (!remoteString) {
        throw MakeError("VirtualAllocEx");
    }

    if (!WriteProcessMemory(process, remoteString, dllPath.c_str(), size, nullptr)) {
        throw MakeError("WriteProcessMemory");
    }

    HANDLE thread = CreateRemoteThread(process, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(GetProcAddress(GetModuleHandleW(L"kernel32"), "LoadLibraryW")), remoteString, 0, nullptr );
    if (!thread) {
        throw MakeError("CreateRemoteThread");
    }

    WaitForSingleObject(thread, 2000);
    CloseHandle(thread);

    VirtualFreeEx(process, remoteString, size, MEM_RELEASE);
}

void AdjustPrivileges() {
    HANDLE token;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token)) {
       throw MakeError("OpenProcessToken");
    }

    TOKEN_PRIVILEGES tp;
    ZeroMemory(&tp, sizeof tp);
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if (!LookupPrivilegeValue(nullptr, SE_DEBUG_NAME, &tp.Privileges[0].Luid)) {
       CloseHandle(token);
       throw MakeError("LookupPrivilegeValue(SE_DEBUG_NAME)");
    }

    if (!AdjustTokenPrivileges(token, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), nullptr, nullptr)) {
       CloseHandle(token);
       throw MakeError("AdjustTokenPrivileges(SE_DEBUG_NAME)");
    }

    if (!LookupPrivilegeValue(nullptr, SE_TCB_NAME, &tp.Privileges[0].Luid)) {
       CloseHandle(token);
       throw MakeError("LookupPrivilegeValue(SE_TCB_NAME)");
    }

    if (!AdjustTokenPrivileges(token, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), nullptr, nullptr)) {
       CloseHandle(token);
       throw MakeError("AdjustTokenPrivileges(SE_TCB_NAME)");
    }
}

DWORD PidFromName(const std::wstring &name) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (snapshot == INVALID_HANDLE_VALUE) {
        throw MakeError("CreateToolhelp32Snapshot");
    }

    PROCESSENTRY32W pe;
    pe.dwSize = sizeof pe;

    if (!Process32FirstW(snapshot, &pe)) {
        throw MakeError("Process32First");
    }

    bool found{};
    DWORD ret{};
    bool done;
    do {
        std::wstring filename = pe.szExeFile;

        if (filename == name) {
            if (found) {
                CloseHandle(snapshot);
                throw std::runtime_error("More than one process with that name was found. Specify a unique name or PID.");
            }

            ret = pe.th32ProcessID;
            found = true;
        }

        const BOOL next = Process32NextW(snapshot, &pe);
        if (!next && GetLastError() != ERROR_NO_MORE_FILES) {
            CloseHandle(snapshot);
            throw MakeError("Process32Next");
        }

        done = !next;
    } while (!done);

    if (!found) {
        CloseHandle(snapshot);
        throw std::runtime_error("No process with that name was found.");
    }

    return ret;
}
