#include "shared.h"
#include "winapi_error.h"
#include "marker_logic.h"
#include <iostream>
#include <cstdlib>
#include <windows.h>

DWORD WINAPI marker_work(LPVOID p) {
    try {
        DWORD start_res = WaitForSingleObject(start_ev, INFINITE);
        if (start_res == WAIT_FAILED) {
            throw WinApiError::from_last_error("WaitForSingleObject failed for start event");
        }

        int seed = *reinterpret_cast<int*>(p);
        int index = seed - 1;

        srand(seed);
        int visited = 0;

        while (true) {
            int num = rand() % n;

            EnterCriticalSection(&cs);

            if (a[num] == 0) {
                Sleep(5);
                a[num] = seed;
                Sleep(5);
                visited++;

                LeaveCriticalSection(&cs);
            }
            else {
                std::cout << "Thread number: " << seed << "\n"
                    << "Marked elements: " << visited << "\n"
                    << "Cannot mark index: " << num << "\n";

                LeaveCriticalSection(&cs);

                if (!SetEvent(cannot_continue[index])) {
                    throw WinApiError::from_last_error("Failed to set cannot_continue event");
                }

                HANDLE events[2] = {
                    continue_ev[index],
                    terminate_ev[index]
                };

                DWORD wr = WaitForMultipleObjects(2, events, FALSE, INFINITE);

                if (wr == WAIT_OBJECT_0) {
                    continue;
                }
                else if (wr == WAIT_OBJECT_0 + 1) {
                    EnterCriticalSection(&cs);

                    clear_marker_elements(a, seed);

                    LeaveCriticalSection(&cs);
                    return 0;
                }
                else if (wr == WAIT_FAILED) {
                    throw WinApiError::from_last_error("WaitForMultipleObjects failed in marker thread");
                }
                else {
                    throw std::runtime_error("Unexpected wait result in marker thread");
                }
            }
        }
    }
    catch (const WinApiError& ex) {
        std::cerr << "WinAPI error in marker thread: " << ex.what() << "\n";
        return ex.code();
    }
    catch (const std::exception& ex) {
        std::cerr << "Error in marker thread: " << ex.what() << "\n";
        return 1;
    }
}