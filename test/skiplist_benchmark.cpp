//
// Created by Koschei on 2025/2/16.
//

#include "skiplist.h"
#include "skiplist.cpp"

#include <benchmark/benchmark.h>

const int benchInitSize = 10000;
const int benchBatchSize = 10;

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

template<typename Key, typename Value, class Comparator>
std::unique_ptr<SkipList<Key, Value, Comparator>> MakeSkipListN(int n) {
    Comparator cmp;
    auto sl = std::make_unique<SkipList<Key, Value, Comparator>>(cmp);
    for (int i = 0; i < n; ++i) {
        sl->insert(std::to_string(i), std::to_string(i));
    }
    return sl;
}

std::map<std::string, std::string> MakeMapN(int n) {
    std::map<std::string, std::string> m;
    for (auto i = 0; i < n; ++i) {
        m[std::to_string(i)] = std::to_string(i);
    }
    return m;
}

void BenchmarkSkipList_Insert(benchmark::State &state) {
    auto start = benchInitSize;
    auto sl = MakeSkipListN<std::string, std::string, Comparator>(start);
    for (auto _: state) {
        for (auto i = 0; i < benchBatchSize; i++) {
            sl->insert(std::to_string(start + i), std::to_string(i));
        }
        start += benchBatchSize;
    }
}

BENCHMARK(BenchmarkSkipList_Insert);

void BenchmarkSkipList_Insert_Dup(benchmark::State &state) {
    auto start = benchInitSize;
    auto sl = MakeSkipListN<std::string, std::string, Comparator>(start);
    for (auto _: state) {
        for (auto i = 0; i < benchBatchSize; i++) {
            sl->insert(std::to_string(i), std::to_string(i));
        }
        start += benchBatchSize;
    }
}

BENCHMARK(BenchmarkSkipList_Insert_Dup);

void BenchmarkSkipList_Erase(benchmark::State &state) {
    auto start = benchInitSize;
    auto sl = MakeSkipListN<std::string, std::string, Comparator>(start);
    for (auto _: state) {
        for (auto i = 0; i < benchBatchSize; i++) {
            sl->erase(std::to_string(start - i));
        }
        start -= benchBatchSize;
    }
}

BENCHMARK(BenchmarkSkipList_Erase);

void BenchmarkMapSkipList_Find(benchmark::State &state) {
    auto sl = MakeSkipListN<std::string, std::string, Comparator>(benchInitSize);
    for (auto _: state) {
        for (auto i = 0; i < benchBatchSize; i++) {
            auto v = sl->get(std::to_string(i));
            (void) v;
        }
    }
}

BENCHMARK(BenchmarkMapSkipList_Find);

void BenchmarkMap_Insert(benchmark::State &state) {
    auto start = benchInitSize;
    auto m = MakeMapN(benchInitSize);
    for (auto _: state) {
        for (auto i = 0; i < benchBatchSize; i++) {
            m[std::to_string(start + i)] = std::to_string(i);
        }
        start += benchBatchSize;
    }
}

BENCHMARK(BenchmarkMap_Insert);

void BenchmarkMap_Insert_Dup(benchmark::State &state) {
    auto start = benchInitSize;
    auto m = MakeMapN(benchInitSize);
    for (auto _: state) {
        for (auto i = 0; i < benchBatchSize; i++) {
            m[std::to_string(i)] = std::to_string(i);
        }
        start += benchBatchSize;
    }
}

BENCHMARK(BenchmarkMap_Insert_Dup);

void BenchmarkMap_Erase(benchmark::State &state) {
    auto start = benchInitSize;
    auto m = MakeMapN(benchInitSize);
    for (auto _: state) {
        for (auto i = 0; i < benchBatchSize; i++) {
            m.erase(std::to_string(start - i));
        }
        start -= benchBatchSize;
    }
}

BENCHMARK(BenchmarkMap_Erase);

void BenchmarkMap_Find(benchmark::State &state) {
    auto m = MakeMapN(benchInitSize);
    for (auto _: state) {
        for (auto i = 0; i < benchBatchSize; i++) {
            auto v = m[std::to_string(i)];
            (void) v;
        }
    }
}

BENCHMARK(BenchmarkMap_Find);

BENCHMARK_MAIN();
