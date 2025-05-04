// Code modified by Bernardo Cohen
// Original code by Antoine Savine
/*
Written by Antoine Savine in 2018

This code is the strict IP of Antoine Savine

License to use and alter this code for personal and commercial applications
is freely granted to any person or company who purchased a copy of the book

Modern Computational Finance: AAD and Parallel Simulations
Antoine Savine
Wiley, 2018

As long as this comment is preserved at the top of the file
*/

#pragma once
#ifndef BLOCKLIST_HPP
#define BLOCKLIST_HPP

#include <array>
#include <cstring>
#include <iterator>
#include <list>

using namespace std;

template <class T, size_t block_size> class blocklist {

  list<array<T, block_size>> data;
  using list_iter = decltype(data.begin());
  using block_iter = decltype(data.front().begin());

  list_iter current_block;

  list_iter last_block;

  block_iter next_space;

  block_iter last_space;

  list_iter marked_block;
  block_iter marked_space;

  void newblock() {
    data.emplace_back();
    current_block = last_block = prev(data.end());
    next_space = current_block->begin();
    last_space = current_block->end();
  }

  void nextblock() {
    if (current_block == last_block) {
      newblock();
    } else {
      ++current_block;
      next_space = current_block->begin();
      last_space = current_block->end();
    }
  }

public:
  blocklist() { newblock(); }

  void clear() {
    data.clear();
    newblock();
  }

  void rewind() {
    current_block = data.begin();
    next_space = current_block->begin();
    last_space = current_block->end();
  }

  void myMemset(unsigned char value = 0) {
    for (auto &arr : data) {
      memset(&arr[0], value, block_size * sizeof(T));
    }
  }

  template <typename... Args> T *emplace_back(Args &&...args) {
    if (next_space == last_space) {
      nextblock();
    }

    T *emplaced = new (&*next_space) T(std::forward<Args>(args)...);
    ++next_space;
    return emplaced;
  }

  T *emplace_back() {
    if (next_space == last_space) {
      nextblock();
    }

    auto old_next = next_space;
    ++next_space;
    return &*old_next;
  }

  template <size_t n> T *emplace_back_multi() {
    if (distance(next_space, last_space) < n) {
      nextblock();
    }

    auto old_next = next_space;
    next_space += n;
    return &*old_next;
  }

  T *emplace_back_multi(size_t n) {
    if (distance(next_space, last_space) < n) {
      nextblock();
    }

    auto old_next = next_space;
    next_space += n;
    return &*old_next;
  }

  void setmark() {
    if (next_space == last_space) {
      nextblock();
    }

    marked_block = current_block;
    marked_space = next_space;
  }

  void rewind_to_mark() {
    current_block = marked_block;
    next_space = marked_space;
    last_space = current_block->end();
  }

  class iterator {
    list_iter current_block;
    block_iter current_space;
    block_iter first_space;
    block_iter last_space;

  public:
    using difference_type = ptrdiff_t;
    using reference = T &;
    using pointer = T *;
    using value_type = T;
    using iterator_category = bidirectional_iterator_tag;

    iterator() = default;

    iterator(list_iter cb, block_iter cs, block_iter fs, block_iter ls)
        : current_block(cb), current_space(cs), first_space(fs),
          last_space(ls) {}

    iterator &operator++() {
      ++current_space;
      if (current_space == last_space) {
        ++current_block;
        first_space = current_block->begin();
        last_space = current_block->end();
        current_space = first_space;
      }
      return *this;
    }

    iterator &operator--() {
      if (current_space == first_space) {
        --current_block;
        first_space = current_block->begin();
        last_space = current_block->end();
        current_space = last_space;
      }
      --current_space;
      return *this;
    }

    T &operator*() { return *current_space; }
    const T &operator*() const { return *current_space; }
    T *operator->() { return &*current_space; }
    const T *operator->() const { return &*current_space; }

    bool operator==(const iterator &rhs) const {
      return (current_block == rhs.current_block &&
              current_space == rhs.current_space);
    }

    bool operator!=(const iterator &rhs) const {
      return (current_block != rhs.current_block ||
              current_space != rhs.current_space);
    }
  };

  iterator begin() {
    return iterator(data.begin(), data.begin()->begin(), data.begin()->begin(),
                    data.begin()->end());
  }

  iterator end() {
    return iterator(current_block, next_space, current_block->begin(),
                    current_block->end());
  }

  iterator mark() {
    return iterator(marked_block, marked_space, marked_block->begin(),
                    marked_block->end());
  }

  iterator find(const T *const element) {
    iterator it = begin();
    iterator b = begin();

    while (it != b) {
      --it;
      if (&*it == element)
        return it;
    }

    if (&*it == element)
      return it;

    return end();
  }
};

#endif // BLOCKLIST_HPP