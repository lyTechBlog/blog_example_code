#include <iostream>
#include <chrono>
#include <ctime>
#include <sys/time.h>
#include <thread>
#include <vector>

using namespace std;
using namespace std::chrono;

const int TEST_TIMES = 1000 * 1000 * 10;

long long getCurrentTimeByClockGetTime() {
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    return spec.tv_sec * 1000LL + spec.tv_nsec / 1000000;
}

long long getCurrentTimeByGetTimeOfDay() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000LL + tv.tv_usec / 1000;
}

long long getCurrentTimeByChrono() {
    return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
}

void testClockGetTime() {
    long long start = getCurrentTimeByChrono();
    for (int i = 0; i < TEST_TIMES; ++i) {
        getCurrentTimeByClockGetTime();
    }
    long long end = getCurrentTimeByChrono();
    cout << "Using clock_gettime in thread " << this_thread::get_id() << ": " << end - start << " ms\n";
}

void testGetTimeOfDay() {
    long long start = getCurrentTimeByChrono();
    for (int i = 0; i < TEST_TIMES; ++i) {
        getCurrentTimeByGetTimeOfDay();
    }
    long long end = getCurrentTimeByChrono();
    cout << "Using gettimeofday in thread " << this_thread::get_id() << ": " << end - start << " ms\n";
}

void testChrono() {
    long long start = getCurrentTimeByChrono();
    for (int i = 0; i < TEST_TIMES; ++i) {
        getCurrentTimeByChrono();
    }
    long long end = getCurrentTimeByChrono();
    cout << "Using chrono::high_resolution_clock in thread " << this_thread::get_id() << ": " << end - start << " ms\n";
}

int testMultiThread(int thread_num) {
    vector<thread> threads;

    for (int i = 0; i < thread_num; ++i) {
        threads.emplace_back(testChrono);
    }
    for (auto& t : threads) {
        t.join();
    }

    threads.clear();
    for (int i = 0; i < thread_num; ++i) {
        threads.emplace_back(testGetTimeOfDay);
    }
    for (auto& t : threads) {
        t.join();
    }

    threads.clear();
    for (int i = 0; i < thread_num; ++i) {
        threads.emplace_back(testClockGetTime);
    }
    for (auto& t : threads) {
        t.join();
    }

    return 0;
}



int main() {
    testMultiThread(1);
    testMultiThread(10);
}