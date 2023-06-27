#include <iostream>
#include <vector>
#include <chrono>
#include <typeinfo>
#include <map>

using namespace std;
using namespace std::chrono;

template <typename T>
void test_vec(const T& t, int add_times = 1000 * 1000) {
  cout << "*** vector type: " << typeid(T).name() << "\n";

  // 测试emplace_back
  vector<T> v1;
  auto start1 = high_resolution_clock::now();  // 记录开始时间
  for (int i = 0; i < add_times; i++) {
      v1.emplace_back(t);
  }
  auto end1 = high_resolution_clock::now();  // 记录结束时间
  auto duration1 = duration_cast<milliseconds>(end1 - start1);
  cout << "emplace_back: " << duration1.count() << " milliseconds" << "\n";
  
  // 测试push_back
  vector<T> v2;

  auto start2 = high_resolution_clock::now();  // 记录开始时间
  for (int i = 0; i < add_times; i++) {
      v2.push_back(T(t));
  }
  auto end2 = high_resolution_clock::now();  // 记录结束时间
  auto duration2 = duration_cast<milliseconds>(end2 - start2);
  cout << "push_back: " << duration2.count() << " milliseconds" << "\n";
  cout << "\n";
}

template <typename T>
void test_map(const T& t, int add_times = 1000 * 1000) {
    std::cout << "*** map type: " << typeid(T).name() << "\n";

    // 测试 emplace
    std::map<T, T> m2;
    auto start2 = std::chrono::high_resolution_clock::now();   // 记录开始时间
    for (int i = 0; i < add_times; ++i) {
        m2.emplace(i, i);
    }
    auto end2 = std::chrono::high_resolution_clock::now();   // 记录结束时间
    auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2);
    std::cout << "emplace: " << duration2.count() << " milliseconds" << "\n";

    // 测试 insert
    std::map<T, T> m1;
    auto start1 = std::chrono::high_resolution_clock::now();   // 记录开始时间
    for (int i = 0; i < add_times; ++i) {
        m1.insert(std::make_pair(i, i));
    }
    auto end1 = std::chrono::high_resolution_clock::now();   // 记录结束时间
    auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1);
    std::cout << "insert: " << duration1.count() << " milliseconds" << "\n";
    cout << "\n";
}

template <>
void test_map<std::string>(const std::string& t, int add_times) {
    std::cout << "*** map type: " << typeid(std::string).name() << "\n";

    // 测试 emplace
    std::map<std::string, std::string> m2;
    auto start2 = std::chrono::high_resolution_clock::now();   // 记录开始时间
    for (int i = 0; i < add_times; ++i) {
        m2.emplace(std::to_string(i), std::to_string(i));
    }
    auto end2 = std::chrono::high_resolution_clock::now();   // 记录结束时间
    auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2);
    std::cout << "emplace: " << duration2.count() << " milliseconds" << "\n";

    // 测试 insert
    std::map<std::string, std::string> m1;
    auto start1 = std::chrono::high_resolution_clock::now();   // 记录开始时间
    for (int i = 0; i < add_times; ++i) {
        m1.insert(std::make_pair(std::to_string(i), std::to_string(i)));
    }
    auto end1 = std::chrono::high_resolution_clock::now();   // 记录结束时间
    auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1);
    std::cout << "insert: " << duration1.count() << " milliseconds" << "\n";
    cout << "\n";
}

int main() {
    std::string str(100, 'a'); ;
    test_vec(1);
    test_vec(str);

    test_map(1);
    test_map(str);

    return 0;
}