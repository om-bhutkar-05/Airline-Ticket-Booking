// include/heap/BinomialHeap.h

#pragma once // Header guard

#include "common/Types.h"
#include "heap/BinomialHeapNode.h" // Node definition
#include <list>
#include <stdexcept> // For runtime_error
#include <utility>   // For std::pair
#include <vector>

class BinomialHeap {
private:
  std::list<BinomialHeapNode *> roots; // Root list

  // --- Private Helper Methods ---
  // ADD DECLARATION HERE:
  void link(BinomialHeapNode *y, BinomialHeapNode *z);

  void mergeRootLists(BinomialHeap &other_heap);
  void consolidate();
  // Make findMinNodeIterator const as it doesn't modify state
  std::list<BinomialHeapNode *>::const_iterator findMinNodeIterator() const;
  void collectNodes(
      BinomialHeapNode *node,
      std::vector<BinomialHeapNode *> &nodes_list); // For destructor/clear

public:
  BinomialHeap();
  ~BinomialHeap();

  // Rule of 5/3: Prevent copying/assignment, allow moving
  BinomialHeap(const BinomialHeap &) = delete;
  BinomialHeap &operator=(const BinomialHeap &) = delete;
  BinomialHeap(BinomialHeap &&other) noexcept;
  BinomialHeap &operator=(BinomialHeap &&other) noexcept;

  // --- Public Interface ---
  bool isEmpty() const;
  void insert(PriorityType priority, PassengerIdType passengerId);
  // ADD CONST HERE
  PassengerIdType findMinPassengerId() const; // Throws if empty
  // ADD CONST HERE
  PriorityType findMinPriority() const; // Throws if empty
  PassengerIdType extractMin();         // Throws if empty
  int getSize() const;
  void clear();

  std::vector<std::pair<PriorityType, PassengerIdType>>
  getWaitlistOrdered_destructive();
};
