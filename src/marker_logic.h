#pragma once
#include <vector>
#include <windows.h>

bool is_valid_thread_number(int th_num, const std::vector<bool>& active);

void clear_marker_elements(std::vector<int>& arr, int marker_id);

std::vector<HANDLE> collect_active_events(
    const std::vector<HANDLE>& events,
    const std::vector<bool>& active);

void deactivate_marker(std::vector<bool>& active, int marker_id);