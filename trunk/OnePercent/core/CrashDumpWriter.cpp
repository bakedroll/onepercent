#ifdef WIN32

#include "CrashDumpWriter.h"
#include <Dbghelp.h>

#include <QDir>

namespace onep
{
  void make_minidump(EXCEPTION_POINTERS* e)
  {
    auto hDbgHelp = LoadLibraryA("dbghelp");
    if (hDbgHelp == nullptr)
      return;
    auto pMiniDumpWriteDump = decltype(&MiniDumpWriteDump)(GetProcAddress(hDbgHelp, "MiniDumpWriteDump"));
    if (pMiniDumpWriteDump == nullptr)
      return;

    char name[MAX_PATH];
    {
      auto nameEnd = name + GetModuleFileNameA(GetModuleHandleA(nullptr), name, MAX_PATH);
      SYSTEMTIME t;
      GetSystemTime(&t);
      wsprintfA(nameEnd - strlen(".exe"),
        "_%4d-%02d-%02d_%02d.%02d.%02d.dmp",
        t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
    }

    QDir dir("./crashdumps");
    if (!dir.exists())
      dir.mkpath(".");

    QFileInfo fileinfo(name);
    QString filename = fileinfo.fileName();

    std::string path = std::string(".\\crashdumps\\") + filename.toStdString();

    printf("Path: %s\n", path.c_str());

    auto hFile = CreateFileA(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
      return;

    MINIDUMP_EXCEPTION_INFORMATION exceptionInfo;
    exceptionInfo.ThreadId = GetCurrentThreadId();
    exceptionInfo.ExceptionPointers = e;
    exceptionInfo.ClientPointers = FALSE;

    pMiniDumpWriteDump(
      GetCurrentProcess(),
      GetCurrentProcessId(),
      hFile,
      MINIDUMP_TYPE(MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory),
      e ? &exceptionInfo : nullptr,
      nullptr,
      nullptr);

    CloseHandle(hFile);
  }

  LONG __stdcall unhandled_handler(EXCEPTION_POINTERS* e)
  {
    make_minidump(e);
    return EXCEPTION_CONTINUE_SEARCH;
  }
}

#endif