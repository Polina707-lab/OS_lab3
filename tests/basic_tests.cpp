#include <gtest/gtest.h>
#include <windows.h>
#include <vector>
#include "marker_logic.h"

TEST(MarkerLogicTest, ValidThreadNumberReturnsTrueForActiveThread) {
    std::vector<bool> active = { true, true, false };

    EXPECT_TRUE(is_valid_thread_number(1, active));
    EXPECT_TRUE(is_valid_thread_number(2, active));
}

TEST(MarkerLogicTest, ValidThreadNumberReturnsFalseForInactiveThread) {
    std::vector<bool> active = { true, false, true };

    EXPECT_FALSE(is_valid_thread_number(2, active));
}

TEST(MarkerLogicTest, ValidThreadNumberReturnsFalseForOutOfRangeValues) {
    std::vector<bool> active = { true, true, true };

    EXPECT_FALSE(is_valid_thread_number(0, active));
    EXPECT_FALSE(is_valid_thread_number(-1, active));
    EXPECT_FALSE(is_valid_thread_number(4, active));
}

TEST(MarkerLogicTest, ValidThreadNumberReturnsFalseForEmptyActiveList) {
    std::vector<bool> active;

    EXPECT_FALSE(is_valid_thread_number(1, active));
    EXPECT_FALSE(is_valid_thread_number(0, active));
}

TEST(MarkerLogicTest, ClearMarkerElementsRemovesOnlySpecifiedMarkerValues) {
    std::vector<int> arr = { 1, 2, 1, 3, 1, 0, 2 };

    clear_marker_elements(arr, 1);

    std::vector<int> expected = { 0, 2, 0, 3, 0, 0, 2 };
    EXPECT_EQ(arr, expected);
}

TEST(MarkerLogicTest, ClearMarkerElementsDoesNothingIfMarkerNotPresent) {
    std::vector<int> arr = { 2, 2, 3, 4, 0 };
    std::vector<int> expected = arr;

    clear_marker_elements(arr, 1);

    EXPECT_EQ(arr, expected);
}

TEST(MarkerLogicTest, ClearMarkerElementsCanClearAllElements) {
    std::vector<int> arr = { 3, 3, 3, 3 };

    clear_marker_elements(arr, 3);

    std::vector<int> expected = { 0, 0, 0, 0 };
    EXPECT_EQ(arr, expected);
}

TEST(MarkerLogicTest, ClearMarkerElementsHandlesEmptyArray) {
    std::vector<int> arr;

    clear_marker_elements(arr, 1);

    EXPECT_TRUE(arr.empty());
}

TEST(MarkerLogicTest, ClearMarkerElementsDoesNotChangeZerosOrOtherMarkers) {
    std::vector<int> arr = { 0, 2, 0, 3, 0 };
    std::vector<int> expected = arr;

    clear_marker_elements(arr, 1);

    EXPECT_EQ(arr, expected);
}

TEST(MarkerLogicTest, CollectActiveEventsReturnsOnlyEventsForActiveThreads) {
    HANDLE h1 = reinterpret_cast<HANDLE>(1);
    HANDLE h2 = reinterpret_cast<HANDLE>(2);
    HANDLE h3 = reinterpret_cast<HANDLE>(3);
    HANDLE h4 = reinterpret_cast<HANDLE>(4);

    std::vector<HANDLE> events = { h1, h2, h3, h4 };
    std::vector<bool> active = { true, false, true, false };

    std::vector<HANDLE> result = collect_active_events(events, active);

    ASSERT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], h1);
    EXPECT_EQ(result[1], h3);
}

TEST(MarkerLogicTest, CollectActiveEventsReturnsEmptyWhenNoThreadsAreActive) {
    HANDLE h1 = reinterpret_cast<HANDLE>(1);
    HANDLE h2 = reinterpret_cast<HANDLE>(2);

    std::vector<HANDLE> events = { h1, h2 };
    std::vector<bool> active = { false, false };

    std::vector<HANDLE> result = collect_active_events(events, active);

    EXPECT_TRUE(result.empty());
}

TEST(MarkerLogicTest, CollectActiveEventsHandlesAllThreadsActive) {
    HANDLE h1 = reinterpret_cast<HANDLE>(1);
    HANDLE h2 = reinterpret_cast<HANDLE>(2);
    HANDLE h3 = reinterpret_cast<HANDLE>(3);

    std::vector<HANDLE> events = { h1, h2, h3 };
    std::vector<bool> active = { true, true, true };

    std::vector<HANDLE> result = collect_active_events(events, active);

    ASSERT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], h1);
    EXPECT_EQ(result[1], h2);
    EXPECT_EQ(result[2], h3);
}

TEST(MarkerLogicTest, CollectActiveEventsPreservesOrderOfActiveThreads) {
    HANDLE h1 = reinterpret_cast<HANDLE>(1);
    HANDLE h2 = reinterpret_cast<HANDLE>(2);
    HANDLE h3 = reinterpret_cast<HANDLE>(3);
    HANDLE h4 = reinterpret_cast<HANDLE>(4);

    std::vector<HANDLE> events = { h1, h2, h3, h4 };
    std::vector<bool> active = { false, true, false, true };

    std::vector<HANDLE> result = collect_active_events(events, active);

    ASSERT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], h2);
    EXPECT_EQ(result[1], h4);
}

TEST(MarkerLogicTest, CollectActiveEventsHandlesDifferentVectorSizes) {
    HANDLE h1 = reinterpret_cast<HANDLE>(1);
    HANDLE h2 = reinterpret_cast<HANDLE>(2);
    HANDLE h3 = reinterpret_cast<HANDLE>(3);

    std::vector<HANDLE> events = { h1, h2, h3 };
    std::vector<bool> active = { true, false };

    std::vector<HANDLE> result = collect_active_events(events, active);

    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], h1);
}