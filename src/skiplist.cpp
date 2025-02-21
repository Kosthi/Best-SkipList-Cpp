//
// Created by Koschei on 2025/2/12.
//

#include "skiplist.h"

#include <fmt/base.h>
#include <fmt/format.h>

#include <sstream>
#include <unordered_set>

// 通用版本：将 Key 转换为字符串后计算长度
template <typename Key>
size_t get_key_length(const Key& key) {
  std::ostringstream oss;
  oss << key;
  return oss.str().size();
}

// 特化版本：若 Key 是 std::string，直接返回长度
template <>
size_t get_key_length<std::string>(const std::string& key) {
  return key.size();
}

template <typename Key, typename Value, class Comparator>
SkipList<Key, Value, Comparator>::SkipList(Comparator cmp, int max_level,
                                           float prob)
    : size_bytes_(0),
      max_level_(max_level),
      current_level_(1),
      probability_(prob),
      gen_(rd_()),
      dis_(0.0, 1.0),
      compare_(cmp) {
  header_ = new SkipListNode<Key, Value>({}, {}, max_level_);
}

template <typename Key, typename Value, class Comparator>
int SkipList<Key, Value, Comparator>::random_level() {
  int level = 1;
  // 通过"抛硬币"的方式随机生成层数：
  // - 每次有50%的概率增加一层
  // - 确保层数分布为：第1层100%，第2层50%，第3层25%，以此类推
  // - 层数范围限制在[1, max_level]之间，避免浪费内存
  while (dis_(gen_) < probability_ && level < max_level_) {
    ++level;
  }
  return level;
}

template <typename Key, typename Value, class Comparator>
void SkipList<Key, Value, Comparator>::insert(Key key, Value value) {
  // 保存搜索过程中经过的节点
  std::vector<SkipListNode<Key, Value>*> update(max_level_, nullptr);
  auto current = header_;  // 不能使用引用，引用就把 header 改了
  for (int level = current_level_ - 1; level >= 0; --level) {
    while (current->forward_[level] &&
           compare_(current->forward_[level]->key_, key) < 0) {
      current = current->forward_[level];
    }
    update[level] = current;
  }
  current = current->forward_[0];
  if (current && compare_(current->key_, key) == 0) {
    current->value_ = std::move(value);
  } else {
    int new_level = random_level();
    if (new_level > current_level_) {
      for (int level = current_level_; level < new_level; ++level) {
        update[level] = header_;
      }
      current_level_ = new_level;
    }
    size_bytes_ += get_key_length(key) + get_key_length(value);
    auto new_node =
        new SkipListNode(std::move(key), std::move(value), max_level_);
    for (int level = 0; level < new_level; ++level) {
      new_node->forward_[level] = update[level]->forward_[level];
      update[level]->forward_[level] = new_node;
    }
  }
}

template <typename Key, typename Value, class Comparator>
void SkipList<Key, Value, Comparator>::erase(const Key& key) {
  // 保存搜索过程中经过的节点
  std::vector<SkipListNode<Key, Value>*> update(max_level_, nullptr);
  auto current = header_;
  for (int level = current_level_ - 1; level >= 0; --level) {
    while (current->forward_[level] &&
           compare_(current->forward_[level]->key_, key) < 0) {
      current = current->forward_[level];
    }
    update[level] = current;  // 都是 < key 或 nullptr
  }
  // auto &prev = current->forward_[0];
  if (current->forward_[0] && compare_(current->forward_[0]->key_, key) == 0) {
    current = current->forward_[0];  // 下一个节点可能大于等于
    // key，等于的话就是我要找的了
    size_bytes_ -=
        get_key_length(current->key_) + get_key_length(current->value_);
    for (int level = 0; level < current_level_; ++level) {
      // 必须是需要删除的目标节点，否则会误删无关节点
      if (update[level]->forward_[level] &&
          compare_(update[level]->forward_[level]->key_, key) == 0) {
        update[level]->forward_[level] = current->forward_[level];
      }
    }
    // 删除节点可能导致层级降低
    while (current_level_ > 1 &&
           header_->forward_[current_level_ - 1] == nullptr) {
      --current_level_;
    }
  }
}

template <typename Key, typename Value, class Comparator>
std::optional<Key> SkipList<Key, Value, Comparator>::get(const Key& key) const {
  auto current = header_;
  for (int level = current_level_ - 1; level >= 0; --level) {
    while (current->forward_[level] &&
           compare_(current->forward_[level]->key_, key) < 0) {
      current = current->forward_[level];
    }
  }
  current = current->forward_[0];
  if (current && compare_(current->key_, key) == 0) {
    return current->value_;
  }
  return {};
}

template <typename Key, typename Value, class Comparator>
bool SkipList<Key, Value, Comparator>::contains(const Key& key) const {
  return get(key).has_value();
}

template <typename Key, typename Value, class Comparator>
void SkipList<Key, Value, Comparator>::print() const {
  // 获取底层所有节点并计算最大键长
  std::vector<SkipListNode<Key, Value>*> nodes;
  auto current = header_->forward_[0];
  size_t max_key_length = 0;
  while (current != nullptr) {
    nodes.emplace_back(current);
    // 计算 Key 长度
    auto length = get_key_length(current->key_);
    if (length > max_key_length) {
      max_key_length = length;
    }
    current = current->forward_[0];
  }

  if (nodes.empty()) {
    fmt::print("Skip list is empty.\n");
    return;
  }

  const int node_width =
      static_cast<int>(max_key_length) + 4;  // 键宽 + 箭头宽度

  // 预计算每层的节点
  std::vector<std::unordered_set<Key> > level_keys(current_level_);
  for (int level = 0; level < current_level_; ++level) {
    auto node = header_->forward_[level];
    while (node != nullptr) {
      level_keys[level].emplace(node->key_);
      node = node->forward_[level];
    }
  }

  // 打印每一层
  for (int level = current_level_ - 1; level >= 0; --level) {
    fmt::print("Level{:2}: ", level);

    fmt::memory_buffer buffer;
    int loss = 0;
    int last_pos = -1;  // 上一个存在节点的位置

    for (int j = 0; j < nodes.size(); ++j) {
      const auto& key = nodes[j]->key_;
      if (level_keys[level].count(key)) {
        if (last_pos == -1) {
          // 填充起始空格
          if (j > 0) {
            fmt::format_to(std::back_inserter(buffer), "{:{}}", "",
                           j * node_width - loss);
          }
          fmt::format_to(std::back_inserter(buffer), "{}", key);
        } else {
          // 计算间隔并填充箭头
          int gap = j - last_pos - 1;
          if (gap > 0) {
            fmt::format_to(std::back_inserter(buffer), " {:-^{}}",
                           "",                      // 内容为空字符串
                           gap * node_width - loss  // 总宽度
            );
            fmt::format_to(std::back_inserter(buffer), "-> {}", key);
          } else {
            fmt::format_to(std::back_inserter(buffer), " -> {}", key);
          }
        }
        loss = 0;
        last_pos = j;
      } else {
        loss += max_key_length - get_key_length(key);
      }
    }
    // 打印当前层
    fmt::print("{}\n", fmt::to_string(buffer));
  }
}
