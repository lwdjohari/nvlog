#define CATCH_CONFIG_MAIN
#include "nvlog/concurrent_queue.h"

#include <catch2/catch_all.hpp>
#include <thread>
#include <vector>

// Explanation
// #Single-threaded Operations:
//
// Enqueue and dequeue operations are tested to ensure basic functionality.
// Test ensures the queue correctly reports its size and emptiness.
//
// # Multi-threaded Operations:
//
// - Multiple producer threads enqueue items into the queue.
// - Multiple consumer threads dequeue items from the queue.
// - Atomic counter is used to ensure the total number of dequeued items matches
// the number enqueued.
// - Tests for thread safety and correctness in a concurrent environment.
//
// # Timeout Behavior:
//
// Tests the WaitAndDequeue function with a timeout to ensure it correctly
// handles waiting and timing out when the queue is empty.
//
// Clearing the Queue:
//
// Enqueue multiple items, clear the queue, and check if the queue is empty.
// Wait and Dequeue:
//
// Tests WaitAndDequeue to ensure it correctly waits for an item to be enqueued
// and then dequeues it.
//
// Complexity Analysis
// Single-threaded Operations: O(1) for enqueue and dequeue.
// Multi-threaded Operations:
// Enqueue: O(1) per item, with mutex locking.
// Dequeue: O(1) per item, with mutex locking and condition variable
// wait/signaling. Thread creation and joining: O(n) for n threads. Timeout
// Behavior: O(1) for checking the timeout condition. Clearing the Queue: O(n)
// for n items in the queue (linear time to clear all items). Wait and Dequeue:
// O(1) for condition variable wait and mutex locking/unlocking.

// Define the test case for the ConcurrentQueue
TEST_CASE("ConcurrentQueue Test") {
  nvlog::ConcurrentQueue<int> queue;

  SECTION("Single-threaded enqueue and dequeue") {
    REQUIRE(queue.Empty());
    queue.Enqueue(1);
    REQUIRE(queue.Size() == 1);
    int value;
    REQUIRE(queue.TryDequeue(value));
    REQUIRE(value == 1);
    REQUIRE(queue.Empty());
  }

  SECTION("Multi-threaded enqueue and dequeue") {
    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;
    const int num_threads = 10;
    const int items_per_thread = 100;
    std::atomic<int> total_dequeued(0);

    // Producer threads
    for (int i = 0; i < num_threads; ++i) {
      producers.emplace_back([&queue, items_per_thread, i]() {
        for (int j = 0; j < items_per_thread; ++j) {
          queue.Enqueue(i * items_per_thread + j);
        }
      });
    }

    // Consumer threads
    for (int i = 0; i < num_threads; ++i) {
      consumers.emplace_back([&queue, items_per_thread, &total_dequeued]() {
        for (int j = 0; j < items_per_thread; ++j) {
          int value;
          while (!queue.TryDequeue(value)) {
            std::this_thread::yield();
          }
          ++total_dequeued;
        }
      });
    }

    for (auto& producer : producers) {
      producer.join();
    }

    for (auto& consumer : consumers) {
      consumer.join();
    }

    REQUIRE(total_dequeued == num_threads * items_per_thread);
    REQUIRE(queue.Empty());
  }

  SECTION("Timeout behavior") {
    int value;
    REQUIRE_FALSE(queue.WaitAndDequeue(value, std::chrono::seconds(1)));
  }

  SECTION("Clearing the queue") {
    queue.Enqueue(1);
    queue.Enqueue(2);
    REQUIRE(queue.Size() == 2);
    queue.Clear();
    REQUIRE(queue.Empty());
  }

  SECTION("Wait and dequeue") {
    std::thread producer([&queue]() {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      queue.Enqueue(42);
    });

    int value;
    queue.WaitAndDequeue(value);
    REQUIRE(value == 42);

    producer.join();
  }
}
