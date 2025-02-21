#include "skiplist.h"
#include "skiplist.cpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <gtest/gtest.h>
#include <iomanip>
#include <latch>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_set>
#include <utility>
#include <vector>

typedef std::string Key;
typedef std::string Value;

struct Comparator {
    int operator()(const Key &a, const Key &b) const {
        if (a < b) {
            return -1;
        } else if (a > b) {
            return +1;
        } else {
            return 0;
        }
    }
};

struct IntComparator {
    int operator()(const int &a, const int &b) const {
        if (a < b) {
            return -1;
        } else if (a > b) {
            return +1;
        } else {
            return 0;
        }
    }
};

struct DoubleComparator {
    int operator()(const double &a, const double &b) const {
        if (a < b) {
            return -1;
        } else if (a > b) {
            return +1;
        } else {
            return 0;
        }
    }
};

// 测试基本插入、查找和删除
TEST(SkipListTest, BasicOperations) {
    Comparator cmp;
    SkipList<Key, Value, Comparator> skipList(cmp);

    // 测试插入和查找
    skipList.insert("key1", "value1");
    EXPECT_EQ(skipList.get("key1").value(), "value1");

    // 测试更新
    skipList.insert("key1", "new_value");
    EXPECT_EQ(skipList.get("key1").value(), "new_value");

    // 测试删除
    skipList.erase("key1");
    EXPECT_FALSE(skipList.get("key1").has_value());

    // skipList.print();
}

TEST(SkipListTest, Template) {
    IntComparator cmp1;
    SkipList<int, int, IntComparator> skip_list1(cmp1);
    // 测试插入和查找
    for (int i = 0; i < 100; ++i) {
        skip_list1.insert(i, i);
        EXPECT_EQ(skip_list1.get(i).value(), i);
    }
    // skip_list1.print();

    DoubleComparator cmp2;
    SkipList<double, double, DoubleComparator> skip_list2(cmp2);
    // 测试插入和查找
    for (int i = 0; i < 100; ++i) {
        skip_list2.insert(i + i / 0.42, i + 1);
        EXPECT_EQ(skip_list2.get(i+ i/0.42).value(), i + 1);
    }
    // skip_list2.print();
}

// 测试迭代器
TEST(SkipListTest, Iterator) {
    Comparator cmp;
    SkipList<Key, Value, Comparator> skipList(cmp);
    skipList.insert("key1", "value1");
    skipList.insert("key2", "value2");
    skipList.insert("key3", "value3");

    // 测试迭代器
    // std::vector<std::pair<std::string, std::string> > result;
    // for (auto it = skipList.begin(); it != skipList.end(); ++it) {
    //     result.push_back(*it);
    // }

    // EXPECT_EQ(result.size(), 3);
    // EXPECT_EQ(result[0].first, "key1");
    // EXPECT_EQ(result[1].first, "key2");
    // EXPECT_EQ(result[2].first, "key3");
}

// 测试大量数据插入和查找
TEST(SkipListTest, LargeScaleInsertAndGet) {
    Comparator cmp;
    SkipList<Key, Value, Comparator> skipList(cmp);
    const int num_elements = 1000;

    // 插入大量数据
    for (int i = 0; i < num_elements; ++i) {
        std::string key = "key" + std::to_string(i);
        std::string value = "value" + std::to_string(i);
        skipList.insert(key, value);
    }

    // 验证插入的数据
    for (int i = 0; i < num_elements; ++i) {
        std::string key = "key" + std::to_string(i);
        std::string expected_value = "value" + std::to_string(i);
        EXPECT_EQ(skipList.get(key).value(), expected_value);
    }

    // skipList.print();
}

// 测试智能指针循环引用和内存泄漏
TEST(SkipListTest, VeryLargeScaleInsert) {
    Comparator cmp;
    SkipList<Key, Value, Comparator> skipList(cmp);
    const int num_elements = 5'000'000;

    // 插入大量数据
    for (int i = 0; i < num_elements; ++i) {
        std::string key = "key" + std::to_string(i);
        std::string value = "value" + std::to_string(i);
        skipList.insert(key, value);
    }
}

// 测试大量数据删除
TEST(SkipListTest, LargeScaleErase) {
    Comparator cmp;
    SkipList<Key, Value, Comparator> skipList(cmp);
    const int num_elements = 10000;

    // 插入大量数据
    for (int i = 0; i < num_elements; ++i) {
        std::string key = "key" + std::to_string(i);
        std::string value = "value" + std::to_string(i);
        skipList.insert(key, value);
    }

    // 删除所有数据
    for (int i = 0; i < num_elements; ++i) {
        std::string key = "key" + std::to_string(i);
        skipList.erase(key);
    }

    // 验证所有数据已被删除
    for (int i = 0; i < num_elements; ++i) {
        std::string key = "key" + std::to_string(i);
        EXPECT_FALSE(skipList.get(key).has_value());
    }
}

// 测试重复插入
TEST(SkipListTest, DuplicateInsert) {
    Comparator cmp;
    SkipList<Key, Value, Comparator> skipList(cmp);

    // 重复插入相同的key
    skipList.insert("key1", "value1");
    skipList.insert("key1", "value2");
    skipList.insert("key1", "value3");

    // 验证最后一次插入的值
    EXPECT_EQ(skipList.get("key1").value(), "value3");
}

// 测试空跳表
TEST(SkipListTest, EmptySkipList) {
    Comparator cmp;
    SkipList<Key, Value, Comparator> skipList(cmp);

    // 验证空跳表的查找和删除
    EXPECT_FALSE(skipList.get("nonexistent_key").has_value());
    skipList.erase("nonexistent_key"); // 删除不存在的key
}

// 测试随机插入和删除
TEST(SkipListTest, RandomInsertAnderase) {
    Comparator cmp;
    SkipList<Key, Value, Comparator> skipList(cmp);
    std::unordered_set<std::string> keys;
    const int num_operations = 10000;

    for (int i = 0; i < num_operations; ++i) {
        std::string key = "key" + std::to_string(rand() % 1000);
        std::string value = "value" + std::to_string(rand() % 1000);

        if (keys.find(key) == keys.end()) {
            // 插入新key
            skipList.insert(key, value);
            keys.insert(key);
        } else {
            // 删除已存在的key
            skipList.erase(key);
            keys.erase(key);
        }

        // 验证当前状态
        if (keys.find(key) != keys.end()) {
            EXPECT_EQ(skipList.get(key).value(), value);
        } else {
            EXPECT_FALSE(skipList.get(key).has_value());
        }
    }
}

// 测试内存大小跟踪
TEST(SkipListTest, MemorySizeTracking) {
    Comparator cmp;
    SkipList<Key, Value, Comparator> skipList(cmp);

    // 插入数据
    skipList.insert("key1", "value1");
    skipList.insert("key2", "value2");

    // 验证内存大小
    size_t expected_size = sizeof("key1") - 1 + sizeof("value1") - 1 +
                           sizeof("key2") - 1 + sizeof("value2") - 1;
    EXPECT_EQ(skipList.get_size(), expected_size);

    // 删除数据
    skipList.erase("key1");
    expected_size -= sizeof("key1") - 1 + sizeof("value1") - 1;
    EXPECT_EQ(skipList.get_size(), expected_size);

    // skipList.clear();
    // EXPECT_EQ(skipList.get_size(), 0);
}

// TEST(SkipListTest, IteratorPreffix) {
//     SkipList skipList;
//
//     // 插入一些测试数据
//     skipList.insert("apple", "0");
//     skipList.insert("apple2", "1");
//     skipList.insert("apricot", "2");
//     skipList.insert("banana", "3");
//     skipList.insert("berry", "4");
//     skipList.insert("cherry", "5");
//     skipList.insert("cherry2", "6");
//
//     // 测试前缀 "ap"
//     auto it = skipList.begin_preffix("ap");
//     EXPECT_EQ(it.get_key(), "apple");
//
//     // 测试前缀 "ba"
//     it = skipList.begin_preffix("ba");
//     EXPECT_EQ(it.get_key(), "banana");
//
//     // 测试前缀 "ch"
//     it = skipList.begin_preffix("ch");
//     EXPECT_EQ(it.get_key(), "cherry");
//
//     // 测试前缀 "z"
//     it = skipList.begin_preffix("z");
//     EXPECT_TRUE(it == skipList.end());
//
//     // 测试前缀 "berr"
//     it = skipList.begin_preffix("berr");
//     EXPECT_EQ(it.get_key(), "berry");
//
//     // 测试前缀 "a"
//     it = skipList.begin_preffix("a");
//     EXPECT_EQ(it.get_key(), "apple");
//
//     // 测试前缀结束位置
//     it = skipList.end_preffix("a");
//     EXPECT_EQ(it.get_key(), "banana");
//
//     it = skipList.end_preffix("cherry");
//     EXPECT_TRUE(it == skipList.end());
//
//     EXPECT_EQ(skipList.begin_preffix("not exist"),
//               skipList.end_preffix("not exist"));
// }

// TEST(SkipListTest, ItersPredicate_Base) {
//     SkipList skipList;
//     skipList.insert("prefix1", "value1");
//     skipList.insert("prefix2", "value2");
//     skipList.insert("prefix3", "value3");
//     skipList.insert("other", "value4");
//     skipList.insert("longerkey", "value5");
//     skipList.insert("averylongkey", "value6");
//     skipList.insert("medium", "value7");
//     skipList.insert("midway", "value8");
//     skipList.insert("midpoint", "value9");
//
//     // 测试前缀匹配
//     auto prefix_result =
//             skipList.iters_monotony_predicate([](const std::string &key) {
//                 auto match_str = key.substr(0, 3);
//                 if (match_str == "pre") {
//                     return 0;
//                 } else if (match_str < "pre") {
//                     return 1;
//                 }
//                 return -1;
//             });
//     ASSERT_TRUE(prefix_result.has_value());
//     auto [prefix_begin_iter, prefix_end_iter] = prefix_result.value();
//     EXPECT_EQ(prefix_begin_iter.get_key(), "prefix1");
//     EXPECT_TRUE(prefix_end_iter.is_end());
//
//     EXPECT_EQ(prefix_begin_iter.get_value(), "value1");
//     ++prefix_begin_iter;
//     EXPECT_EQ(prefix_begin_iter.get_value(), "value2");
//     ++prefix_begin_iter;
//     EXPECT_EQ(prefix_begin_iter.get_value(), "value3");
//
//     // 测试范围匹配
//     auto range = std::make_pair("l", "n"); // [l, n)
//     auto range_result =
//             skipList.iters_monotony_predicate([&range](const std::string &key) {
//                 if (key < range.first) {
//                     return 1;
//                 } else if (key >= range.second) {
//                     return -1;
//                 } else {
//                     return 0;
//                 }
//             });
//     ASSERT_TRUE(range_result.has_value());
//     auto [range_begin_iter, range_end_iter] = range_result.value();
//     EXPECT_EQ(range_end_iter.get_key(),
//               "other"); // end_iter 是开区间，所以指向 "prefix1"
//     EXPECT_EQ(range_begin_iter.get_key(), "longerkey");
//     ++range_begin_iter;
//     EXPECT_EQ(range_begin_iter.get_key(), "medium");
//     ++range_begin_iter;
//     EXPECT_EQ(range_begin_iter.get_key(), "midpoint");
//     ++range_begin_iter;
//     EXPECT_EQ(range_begin_iter.get_key(), "midway");
// }

// TEST(SkipListTest, ItersPredicate_Large) {
//     SkipList skipList;
//     int num = 10000;
//
//     for (int i = 0; i < num; ++i) {
//         std::ostringstream oss_key;
//         std::ostringstream oss_value;
//
//         // 设置数字为4位长度，不足的部分用前导零填充
//         oss_key << "key" << std::setw(4) << std::setfill('0') << i;
//         oss_value << "value" << std::setw(4) << std::setfill('0') << i;
//
//         std::string key = oss_key.str();
//         std::string value = oss_value.str();
//
//         skipList.insert(key, value);
//     }
//
//     skipList.erase("key1015");
//
//     auto result = skipList.iters_monotony_predicate([](const std::string &key) {
//         if (key < "key1010") {
//             return 1;
//         } else if (key >= "key1020") {
//             return -1;
//         } else {
//             return 0;
//         }
//     });
//
//     ASSERT_TRUE(result.has_value());
//     auto [range_begin_iter, range_end_iter] = result.value();
//     EXPECT_EQ(range_begin_iter.get_key(), "key1010");
//     EXPECT_EQ(range_end_iter.get_key(), "key1020");
//     for (int i = 0; i < 5; i++) {
//         range_begin_iter++;
//     }
//     EXPECT_EQ(range_begin_iter.get_key(), "key1016");
// }

// ! 现在的实现, 并发的锁由 SkipList 的上层 MemTable 实现, 因此不需要测试
// SkipList 的并发性
// // 测试跳表的并发性能
// TEST(SkipListTest, ConcurrentOperations) {
//   SkipList skipList;
//   const int num_readers = 4;       // 读线程数
//   const int num_writers = 2;       // 写线程数
//   const int num_operations = 1000; // 每个线程的操作数

//   // 用于同步所有线程的开始
//   std::atomic<bool> start{false};
//   // 用于等待所有线程完成
//   std::latch completion_latch((num_readers + num_writers));

//   // 记录写入的键，用于验证
//   std::vector<std::string> inserted_keys;
//   std::mutex keys_mutex;

//   // 写线程函数
//   auto writer_func = [&](int thread_id) {
//     // 等待开始信号
//     while (!start) {
//       std::this_thread::yield();
//     }

//     // 执行写操作
//     for (int i = 0; i < num_operations; ++i) {
//       std::string key =
//           "key_" + std::to_string(thread_id) + "_" + std::to_string(i);
//       std::string value =
//           "value_" + std::to_string(thread_id) + "_" + std::to_string(i);

//       if (i % 2 == 0) {
//         // 插入操作
//         skipList.insert(key, value);
//         {
//           std::lock_guard<std::mutex> lock(keys_mutex);
//           inserted_keys.push_back(key);
//         }
//       } else {
//         // 删除操作
//         skipList.erase(key);
//       }

//       // 随机休眠一小段时间，模拟实际工作负载
//       std::this_thread::sleep_for(std::chrono::microseconds(rand() % 100));
//     }

//     completion_latch.count_down();
//   };

//   // 读线程函数
//   auto reader_func = [&](int thread_id) {
//     // 等待开始信号
//     while (!start) {
//       std::this_thread::yield();
//     }

//     int found_count = 0;
//     // 执行读操作
//     for (int i = 0; i < num_operations; ++i) {
//       // 随机选择一个已插入的key进行查询
//       std::string key_to_find;
//       {
//         std::lock_guard<std::mutex> lock(keys_mutex);
//         if (!inserted_keys.empty()) {
//           key_to_find = inserted_keys[rand() % inserted_keys.size()];
//         }
//       }

//       if (!key_to_find.empty()) {
//         auto result = skipList.get(key_to_find);
//         if (result.has_value()) {
//           found_count++;
//         }
//       }

//       // 每隔一段时间进行一次遍历操作
//       if (i % 100 == 0) {
//         std::vector<std::pair<std::string, std::string>> items;
//         for (auto it = skipList.begin(); it != skipList.end(); ++it) {
//           items.push_back(*it);
//         }
//       }

//       std::this_thread::sleep_for(std::chrono::microseconds(rand() % 50));
//     }

//     completion_latch.count_down();
//   };

//   // 创建并启动写线程
//   std::vector<std::thread> writers;
//   for (int i = 0; i < num_writers; ++i) {
//     writers.emplace_back(writer_func, i);
//   }

//   // 创建并启动读线程
//   std::vector<std::thread> readers;
//   for (int i = 0; i < num_readers; ++i) {
//     readers.emplace_back(reader_func, i);
//   }

//   // 给线程一点时间进入等待状态
//   std::this_thread::sleep_for(std::chrono::milliseconds(100));

//   // 记录开始时间
//   auto start_time = std::chrono::high_resolution_clock::now();

//   // 发送开始信号
//   start = true;

//   // 等待所有线程完成
//   completion_latch.wait();

//   // 记录结束时间
//   auto end_time = std::chrono::high_resolution_clock::now();
//   auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
//       end_time - start_time);

//   // 等待所有线程结束
//   for (auto &w : writers) {
//     w.join();
//   }
//   for (auto &r : readers) {
//     r.join();
//   }

//   // 验证跳表的最终状态
//   size_t final_size = 0;
//   for (auto it = skipList.begin(); it != skipList.end(); ++it) {
//     final_size++;
//   }

//   //   std::cout << "Concurrent test completed in " << duration.count()
//   //             << "ms\nFinal skiplist size: " << final_size << std::endl;

//   // 基本正确性检查
//   EXPECT_GT(final_size, 0); // 跳表不应该为空
//   EXPECT_LE(final_size,
//             num_writers * num_operations); // 跳表大小不应超过最大可能值
// }

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
