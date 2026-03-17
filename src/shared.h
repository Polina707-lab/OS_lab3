#pragma once
#include <vector>
#include <windows.h>

extern std::size_t n;
extern std::size_t m_count;

extern std::vector<int> a;

extern CRITICAL_SECTION cs;
extern bool cs_initialized;

extern HANDLE start_ev;

extern std::vector<HANDLE> cannot_continue;
extern std::vector<HANDLE> continue_ev;
extern std::vector<HANDLE> terminate_ev;

extern std::vector<HANDLE> markers;
extern std::vector<bool> active;

DWORD WINAPI marker_work(LPVOID p);