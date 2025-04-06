#pragma once // Header guard

#include "common/Types.h" // Include common type definitions

// Forward declaration (optional, as it's self-referential)
// struct BinomialHeapNode;

struct BinomialHeapNode {
  PriorityType priority;       // Priority key
  PassengerIdType passengerId; // Data (Passenger ID)
  int degree;
  BinomialHeapNode *parent;
  BinomialHeapNode *child;
  BinomialHeapNode *sibling;

  BinomialHeapNode(PriorityType p, PassengerIdType data)
      : priority(p), passengerId(data), degree(0), parent(nullptr),
        child(nullptr), sibling(nullptr) {}

  // Prevent copying nodes directly - manage through heap operations
  BinomialHeapNode(const BinomialHeapNode &) = delete;
  BinomialHeapNode &operator=(const BinomialHeapNode &) = delete;
};
