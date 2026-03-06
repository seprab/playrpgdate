//
// Created by Sergio Prada on 1/03/26.
//

#ifndef CARDOBLAST_CIRCULARBUFFER_H
#define CARDOBLAST_CIRCULARBUFFER_H

#include <cstddef>  // For size_t
#include <stdexcept>
#include <vector>

template <typename T>
class CircularBuffer {
 public:
  // Constructor to initialize the buffer with a fixed capacity
  CircularBuffer(size_t capacity)
      : m_data(capacity), m_capacity(capacity), m_head(0), m_tail(0), m_count(0) {}

  // Destructor to clean up any dynamically allocated items if necessary
  ~CircularBuffer() {
    for (size_t i = 0; i < m_capacity; ++i) {
      if (m_data[i] != nullptr) {
        delete m_data[i];
        m_data[i] = nullptr;
      }
    }
  }

  // Adds an item to the circular buffer
  void add(T* item) {
    if (isFull()) {
      // Overwrite the oldest item if the buffer is full
      delete m_data[m_head];  // Free the memory of the overwritten item
      m_head = (m_head + 1) % m_capacity;
    } else {
      m_count++;  // Only increment the count if the buffer isn't full
    }

    // Insert the item at the tail and advance the tail pointer
    m_data[m_tail] = item;
    m_tail = (m_tail + 1) % m_capacity;
  }

  // Removes and returns the item at the head of the circular buffer
  T* remove() {
    if (isEmpty()) {
      return nullptr;
    }

    // Get the item at the head
    T* item = m_data[m_head];
    m_data[m_head] = nullptr;  // Set the removed spot to nullptr
    m_head = (m_head + 1) % m_capacity;
    m_count--;  // Decrease the item count

    return item;
  }

  // Returns the item at the head without removing it
  T* peek() const {
    if (isEmpty()) {
      return nullptr;
    }
    return m_data[m_head];
  }

  // Checks if the buffer is full
  bool isFull() const { return m_count == m_capacity; }

  // Checks if the buffer is empty
  bool isEmpty() const { return m_count == 0; }

  // Returns the current number of elements in the buffer
  size_t size() const { return m_count; }

  // Returns the total capacity of the buffer
  size_t capacity() const { return m_capacity; }

 private:
  std::vector<T*> m_data;  // Storage for buffer elements (pointers)
  size_t m_capacity;       // Maximum number of elements
  size_t m_head;           // Index of the oldest element (head)
  size_t m_tail;           // Index for the next insertion (tail)
  size_t m_count;          // Current number of elements in the buffer
};

#endif //CARDOBLAST_CIRCULARBUFFER_H