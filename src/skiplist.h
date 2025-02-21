//
// Created by Koschei on 2025/2/12.
//

#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <__random/random_device.h>
#include <fmt/format.h>

#include <iostream>
#include <memory>
#include <random>
#include <utility>
#include <vector>

template <typename Key, typename Value>
struct SkipListNode {
  Key key_;                             // 节点存储的键
  Value value_;                         // 节点存储的值
  std::vector<SkipListNode*> forward_;  // 多层前向指针

  SkipListNode(Key key, Value value, int level)
      : key_(std::move(key)),
        value_(std::move(value)),
        forward_(level, nullptr) {}
};

template <typename Key, typename Value, class Comparator>
class SkipList {
 public:
  explicit SkipList(Comparator cmp, int max_level = 16, float prob = 0.5);

  SkipList(const SkipList&) = delete;

  SkipList& operator=(const SkipList&) = delete;

  ~SkipList() {
    auto current = header_->forward_[0];
    while (current != nullptr) {
      auto next = current->forward_[0];
      delete current;
      current = next;
    }
    delete header_;
  }

  void insert(Key key, Value value);

  void erase(const Key& key);

  std::optional<Key> get(const Key& key) const;

  bool contains(const Key& key) const;

  size_t get_size() const { return size_bytes_; }

  void print() const;

 private:
  size_t size_bytes_;
  int max_level_;
  int current_level_;
  float probability_;
  SkipListNode<Key, Value>* header_;
  std::random_device rd_;
  std::mt19937 gen_;
  std::uniform_real_distribution<> dis_;

  Comparator const compare_;

  int random_level();
};

#endif  // SKIPLIST_H
