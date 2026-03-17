#include <iostream>
#include <vector>
#include <windows.h>
#include "shared.h"
#include "winapi_error.h"
#include "marker_logic.h"

std::size_t n = 0;
std::size_t m_count = 0;

std::vector<int> a;

CRITICAL_SECTION cs;
bool cs_initialized = false;

HANDLE start_ev = NULL;

std::vector<HANDLE> cannot_continue;
std::vector<HANDLE> continue_ev;
std::vector<HANDLE> terminate_ev;

std::vector<HANDLE> markers;
std::vector<bool> active;

void close_handle_if_valid(HANDLE& h) {
    if (h != NULL) {
        CloseHandle(h);
        h = NULL;
    }
}

void print_array() {
    for (int el : a) {
        std::cout << el << " ";
    }
    std::cout << "\n";
}

void print_active_threads() {
    for (std::size_t i = 0; i < m_count; i++) {
        if (active[i]) {
            std::cout << (i + 1) << " ";
        }
    }
    std::cout << "\n";
}

void cleanup() {
    close_handle_if_valid(start_ev);

    for (std::size_t i = 0; i < cannot_continue.size(); i++) {
        close_handle_if_valid(cannot_continue[i]);
    }
    for (std::size_t i = 0; i < continue_ev.size(); i++) {
        close_handle_if_valid(continue_ev[i]);
    }
    for (std::size_t i = 0; i < terminate_ev.size(); i++) {
        close_handle_if_valid(terminate_ev[i]);
    }
    for (std::size_t i = 0; i < markers.size(); i++) {
        close_handle_if_valid(markers[i]);
    }

    if (cs_initialized) {
        DeleteCriticalSection(&cs);
        cs_initialized = false;
    }
}

int main() {
    try {
        InitializeCriticalSection(&cs);
        cs_initialized = true;

        std::cout << "Enter array size: ";
        if (!(std::cin >> n) || n == 0U) {
            throw std::invalid_argument("Invalid array size");
        }

        a.resize(n, 0);

        std::cout << "Enter the number of threads marker: ";
        if (!(std::cin >> m_count) || m_count == 0U) {
            throw std::invalid_argument("Invalid count");
        }

        cannot_continue.resize(m_count);
        continue_ev.resize(m_count);
        terminate_ev.resize(m_count);
        markers.resize(m_count);
        active.resize(m_count, true);

        std::vector<int> ids(m_count);

        for (std::size_t i = 0; i < m_count; i++) {
            cannot_continue[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
            continue_ev[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
            terminate_ev[i] = CreateEvent(NULL, FALSE, FALSE, NULL);

            if (cannot_continue[i] == NULL ||
                continue_ev[i] == NULL ||
                terminate_ev[i] == NULL) {
                throw WinApiError::from_last_error("Failed to create thread events");
            }
        }

        start_ev = CreateEvent(NULL, TRUE, FALSE, NULL);
        if (start_ev == NULL) {
            throw WinApiError::from_last_error("Failed to create start event");
        }

        for (std::size_t i = 0; i < m_count; i++) {
            ids[i] = static_cast<int>(i + 1);
            DWORD idMarker = 0;

            markers[i] = CreateThread(
                nullptr,
                0,
                marker_work,
                &ids[i],
                0,
                &idMarker
            );

            if (markers[i] == NULL) {
                throw WinApiError::from_last_error("Failed to create marker thread");
            }
        }

        if (!SetEvent(start_ev)) {
            throw WinApiError::from_last_error("Failed to set start event");
        }

        std::size_t active_count = m_count;

        while (active_count > 0) {
            std::vector<HANDLE> wait_events = collect_active_events(cannot_continue, active);

            DWORD wr = WaitForMultipleObjects(
                static_cast<DWORD>(wait_events.size()),
                wait_events.data(),
                TRUE,
                INFINITE
            );

            if (wr == WAIT_FAILED) {
                throw WinApiError::from_last_error("WaitForMultipleObjects failed");
            }

            std::cout << "All active marker threads cannot continue.\n";
            std::cout << "Array: ";
            print_array();

            std::cout << "Active threads: ";
            print_active_threads();

            std::cout << "Enter thread number to terminate: ";
            int th_num;

            while (true) {
                if (!(std::cin >> th_num)) {
                    std::cin.clear();
                    std::cin.ignore(10000, '\n');
                    std::cout << "Invalid input. Enter a number: ";
                    continue;
                }

                if (is_valid_thread_number(th_num, active)) {
                    break;
                }

                std::cout << "Invalid thread number. Enter again: ";
            }

            while (!is_valid_thread_number(th_num, active)) {
                std::cout << "Invalid thread number. Enter again: ";
                std::cin >> th_num;
            }

            if (!SetEvent(terminate_ev[th_num - 1])) {
                throw WinApiError::from_last_error("Failed to set terminate event");
            }

            DWORD wr_thread = WaitForSingleObject(markers[th_num - 1], INFINITE);
            if (wr_thread == WAIT_FAILED) {
                throw WinApiError::from_last_error("WaitForSingleObject failed for marker thread");
            }

            active[th_num - 1] = false;
            active_count--;

            std::cout << "Array after thread termination: ";
            print_array();

            for (std::size_t i = 0; i < m_count; i++) {
                if (active[i]) {
                    ResetEvent(cannot_continue[i]);

                    if (!SetEvent(continue_ev[i])) {
                        throw WinApiError::from_last_error("Failed to set continue event");
                    }
                }
            }
        }

        cleanup();
        return 0;
    }
    catch (const WinApiError& ex) {
        std::cerr << "WinAPI error: " << ex.what() << "\n";
        cleanup();
        return static_cast<int>(ex.code());
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        cleanup();
        return 1;
    }
}