// src/heap/BinomialHeap.cpp
#include "heap/BinomialHeap.h"
#include <algorithm>
#include <cmath> // <--- ADD THIS INCLUDE for log2
#include <utility>
#include <vector>

// --- Private Helper Method Implementations ---
// ... (rest of the file remains the same initially) ...

// --- Private Helper Method Implementations ---

void BinomialHeap::link(BinomialHeapNode *y, BinomialHeapNode *z) {
  y->parent = z;
  y->sibling = z->child;
  z->child = y;
  z->degree++;
}

void BinomialHeap::mergeRootLists(BinomialHeap &other_heap) {
  auto it1 = roots.begin();
  auto it2 = other_heap.roots.begin();
  std::list<BinomialHeapNode *> new_roots;

  while (it1 != roots.end() && it2 != other_heap.roots.end()) {
    if ((*it1)->degree <= (*it2)->degree) {
      new_roots.push_back(*it1);
      it1++;
    } else {
      new_roots.push_back(*it2);
      it2++;
    }
  }
  // Append remaining nodes
  while (it1 != roots.end()) {
    new_roots.push_back(*it1);
    it1++;
  }
  while (it2 != other_heap.roots.end()) {
    new_roots.push_back(*it2);
    it2++;
  }

  roots = std::move(new_roots); // Replace old root list
  other_heap.roots.clear();     // Other heap is now empty
}

void BinomialHeap::consolidate() {
  if (roots.size() <= 1) {
    return;
  }

  // Max possible degree can be estimated, log2(n). Using roots.size()+1 is
  // safe. Calculate a reasonable upper bound for degrees needed.
  int max_degree_possible = 0;
  if (!roots.empty()) {
    int n = getSize(); // Need getSize() to be somewhat reliable or estimate
                       // based on roots size
    max_degree_possible =
        static_cast<int>(log2(n > 0 ? n : 1)) + 2; // Add buffer
  }
  if (max_degree_possible < 5)
    max_degree_possible = 5; // Minimum size

  std::vector<BinomialHeapNode *> degree_table(max_degree_possible, nullptr);

  std::list<BinomialHeapNode *> new_root_list;
  auto it = roots.begin();

  while (it != roots.end()) {
    BinomialHeapNode *current = *it;
    int d = current->degree;

    // Check if another tree with the same degree exists in the table
    while (static_cast<size_t>(d) < degree_table.size() &&
           degree_table[d] != nullptr) {
      BinomialHeapNode *other = degree_table[d];

      // Ensure 'current' has the smaller key (or equal), swap pointers if
      // necessary Remember: Lower priority value means higher actual priority
      if (other->priority < current->priority) {
        std::swap(current, other);
      }

      // Now link 'other' under 'current'. 'other' must be removed from table.
      link(other, current);
      degree_table[d] = nullptr; // Clear the slot as we've merged from it
      d++; // Degree of 'current' increased, check next degree slot
    }

    // Place the resulting tree (potentially merged) in the table
    // Resize table if necessary (should be rare if initial estimate is good)
    if (static_cast<size_t>(d) >= degree_table.size()) {
      degree_table.resize(d + 1, nullptr);
    }
    degree_table[d] = current;

    it++; // Move to the next node in the original root list
  }

  // Rebuild the root list from the degree table
  roots.clear();
  for (BinomialHeapNode *node : degree_table) {
    if (node != nullptr) {
      // Reset parent pointer for root nodes before adding
      node->parent = nullptr;
      roots.push_back(node);
    }
  }
  // Ensure roots are sorted by degree (optional but conventional)
  roots.sort([](const BinomialHeapNode *a, const BinomialHeapNode *b) {
    return a->degree < b->degree;
  });
}

std::list<BinomialHeapNode *>::const_iterator
BinomialHeap::findMinNodeIterator() const {
  if (roots.empty()) {
    return roots.cend(); // Use const iterator end
  }
  auto min_it = roots.cbegin(); // Use const iterator begin
  for (auto it = std::next(roots.cbegin()); it != roots.cend(); ++it) {
    // Lower priority value means higher actual priority
    if ((*it)->priority < (*min_it)->priority) {
      min_it = it;
    }
  }
  return min_it; // Returns a const_iterator
}

void BinomialHeap::collectNodes(BinomialHeapNode *node,
                                std::vector<BinomialHeapNode *> &nodes_list) {
  if (node == nullptr)
    return;
  // Use iterative approach to avoid deep recursion stack
  std::vector<BinomialHeapNode *> q;
  q.push_back(node);
  while (!q.empty()) {
    BinomialHeapNode *current = q.back();
    q.pop_back();
    nodes_list.push_back(current);
    if (current->child)
      q.push_back(current->child);
    if (current->sibling)
      q.push_back(current->sibling); // Collect siblings as well
  }
  // The initial call should iterate through roots and call this for each root's
  // children/siblings. Let's refine clear() instead.
}

// --- Constructor / Destructor / Move Operations ---

BinomialHeap::BinomialHeap() = default; // Empty heap

BinomialHeap::~BinomialHeap() { clear(); }

void BinomialHeap::clear() {
  std::vector<BinomialHeapNode *> nodes_to_delete;
  std::vector<BinomialHeapNode *> q;

  // Start with all root nodes
  for (auto *root : roots) {
    if (root)
      q.push_back(root);
  }
  roots.clear(); // Clear the list itself

  // Iteratively traverse all reachable nodes
  while (!q.empty()) {
    BinomialHeapNode *current = q.back();
    q.pop_back();
    nodes_to_delete.push_back(current); // Mark for deletion

    // Add children and siblings to the queue
    if (current->child)
      q.push_back(current->child);
    if (current->sibling)
      q.push_back(current->sibling);
  }

  // Delete all collected nodes
  for (BinomialHeapNode *node : nodes_to_delete) {
    delete node;
  }
}

BinomialHeap::BinomialHeap(BinomialHeap &&other) noexcept
    : roots(std::move(other.roots)) {
  // 'other' is left in a valid, empty state by std::list move constructor
}

BinomialHeap &BinomialHeap::operator=(BinomialHeap &&other) noexcept {
  if (this != &other) {
    clear();                        // Delete existing nodes
    roots = std::move(other.roots); // Move ownership
  }
  return *this;
}

// --- Public Interface Method Implementations ---

bool BinomialHeap::isEmpty() const { return roots.empty(); }

void BinomialHeap::insert(PriorityType priority, PassengerIdType passengerId) {
  BinomialHeapNode *new_node = new BinomialHeapNode(priority, passengerId);
  BinomialHeap temp_heap;
  temp_heap.roots.push_back(new_node);
  mergeRootLists(
      temp_heap); // Merge the new node's heap (now empty) into this heap
  consolidate();
}

PassengerIdType BinomialHeap::findMinPassengerId() const {
  auto min_it = findMinNodeIterator();
  if (min_it == roots.end()) {
    throw std::runtime_error("Heap is empty");
  }
  return (*min_it)->passengerId;
}

PriorityType BinomialHeap::findMinPriority() const {
  auto min_it = findMinNodeIterator();
  if (min_it == roots.end()) {
    throw std::runtime_error("Heap is empty");
  }
  return (*min_it)->priority;
}

PassengerIdType BinomialHeap::extractMin() {
  auto min_it = findMinNodeIterator();
  if (min_it == roots.end()) {
    throw std::runtime_error("Cannot extract from empty heap");
  }

  BinomialHeapNode *min_node = *min_it;
  roots.erase(min_it); // Remove min node from root list

  // Create a temporary heap with the children of the min node
  BinomialHeap children_heap;
  BinomialHeapNode *child = min_node->child;
  while (child != nullptr) {
    BinomialHeapNode *next_sibling = child->sibling; // Store next sibling
    child->parent = nullptr;                         // Reset parent
    child->sibling = nullptr;                        // Isolate the child
    // Children list needs to be reversed when adding to roots
    children_heap.roots.push_front(child);
    child = next_sibling;
  }

  // Merge the children heap back into the main heap
  mergeRootLists(children_heap);
  consolidate();

  PassengerIdType minPassengerId = min_node->passengerId;
  delete min_node; // Free memory
  return minPassengerId;
}

int BinomialHeap::getSize() const {
  int count = 0;
  std::vector<BinomialHeapNode *> q;
  for (auto *root : roots) {
    if (root)
      q.push_back(root);
  }

  int head = 0;
  while (static_cast<decltype(q.size())>(head) < q.size()) {
    BinomialHeapNode *current = q[head++];
    count++;
    BinomialHeapNode *child = current->child;
    while (child != nullptr) {
      q.push_back(child);
      child = child->sibling;
    }
  }
  return count; // Returns total number of nodes
}

std::vector<std::pair<PriorityType, PassengerIdType>>
BinomialHeap::getWaitlistOrdered_destructive() {
  std::vector<std::pair<PriorityType, PassengerIdType>> waitlist;
  while (!isEmpty()) {
    PriorityType prio = findMinPriority();
    PassengerIdType passId = extractMin(); // This modifies the heap
    waitlist.push_back({prio, passId});
  }
  return waitlist; // Original heap is now empty
}
