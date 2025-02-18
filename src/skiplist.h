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

struct SkipListNode {
  std::string key_;                     // 节点存储的键
  std::string value_;                   // 节点存储的值
  std::vector<SkipListNode*> forward_;  // 多层前向指针

  SkipListNode(std::string key, std::string value, int level)
      : key_(std::move(key)),
        value_(std::move(value)),
        forward_(level, nullptr) {}
};

class SkipList {
 public:
  explicit SkipList(int max_level = 16, float prob = 0.5);

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

  void insert(std::string key, std::string value);

  void erase(const std::string& key);

  std::optional<std::string> get(const std::string& key) const;

  bool contains(const std::string& key) const;

  size_t get_size() const { return size_bytes_; }

  void print() const;

 private:
  size_t size_bytes_;
  int max_level_;
  int current_level_;
  float probability_;
  SkipListNode* header_;
  std::random_device rd_;
  std::mt19937 gen_;
  std::uniform_real_distribution<> dis_;

  int random_level();
};

#endif  // SKIPLIST_H
