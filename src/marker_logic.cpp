#include "marker_logic.h"

//корректность выбора потока по номеру
bool is_valid_thread_number(int th_num, const std::vector<bool>& active) {
    return th_num >= 1 &&
        th_num <= static_cast<int>(active.size()) &&
        active[th_num - 1];
}

//логика очистки массива при завершении потока
void clear_marker_elements(std::vector<int>& arr, int marker_id) {
    for (int& x : arr) {
        if (x == marker_id) {
            x = 0;
        }
    }
}

//формирование списка активных потоков для ожидания в main
std::vector<HANDLE> collect_active_events(
    const std::vector<HANDLE>& events,
    const std::vector<bool>& active) {
    std::vector<HANDLE> result;

    const std::size_t count = (events.size() < active.size())
        ? events.size()
        : active.size();

    for (std::size_t i = 0; i < count; i++) {
        if (active[i]) {
            result.push_back(events[i]);
        }
    }

    return result;
}



void deactivate_marker(std::vector<bool>& active, int marker_id) {
    if (marker_id >= 1 && marker_id <= static_cast<int>(active.size())) {
        active[marker_id - 1] = false;
    }
}