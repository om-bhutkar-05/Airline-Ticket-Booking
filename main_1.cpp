#include <algorithm>
#include <cstdlib> // For system("cls") or system("clear")
#include <iomanip> // For std::setw
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

// --- Configuration ---
// Lower priority value means HIGHER priority (e.g., earlier booking time)
using PriorityType = int;
using PassengerIdType = int;
const PriorityType INVALID_PRIORITY = std::numeric_limits<PriorityType>::max();

// --- Basic Data Structures ---

struct Passenger {
  PassengerIdType id;
  std::string name;

  // Default constructor for map usage
  Passenger() : id(-1), name("Unknown") {}

  Passenger(PassengerIdType _id, std::string _name)
      : id(_id), name(std::move(_name)) {}
};

// --- Binomial Heap Implementation ---

struct BinomialHeapNode {
  PriorityType priority;       // Priority key (e.g., booking sequence number)
  PassengerIdType passengerId; // Data
  int degree;
  BinomialHeapNode *parent;
  BinomialHeapNode *child;
  BinomialHeapNode *sibling;

  BinomialHeapNode(PriorityType p, PassengerIdType data)
      : priority(p), passengerId(data), degree(0), parent(nullptr),
        child(nullptr), sibling(nullptr) {}
};

class BinomialHeap {
private:
  std::list<BinomialHeapNode *> roots; // Using list for easier merging/splicing

  // Links two binomial trees of the same degree, making y the child of z
  void link(BinomialHeapNode *y, BinomialHeapNode *z) {
    y->parent = z;
    y->sibling = z->child;
    z->child = y;
    z->degree++;
  }

  // Merges the root lists of two heaps (does not handle trees of same degree
  // yet) Modifies the current heap's root list
  void mergeRootLists(BinomialHeap &other_heap) {
    auto it1 = roots.begin();
    auto it2 = other_heap.roots.begin();

    while (it1 != roots.end() && it2 != other_heap.roots.end()) {
      // Insert node from other_heap before it1 if its degree is smaller
      if ((*it1)->degree > (*it2)->degree) {
        it1 = roots.insert(it1, *it2);
        it2++;
      } else {
        it1++;
      }
    }
    // Append remaining nodes from other_heap
    while (it2 != other_heap.roots.end()) {
      roots.push_back(*it2);
      it2++;
    }
    other_heap.roots.clear(); // Other heap is now empty
  }

  // Consolidates trees of the same degree after merging
  void consolidate() {
    if (roots.empty() || roots.size() == 1) {
      return;
    }

    // Max possible degree is log2(n), size+1 provides safe upper bound
    std::vector<BinomialHeapNode *> degree_table(roots.size() + 1, nullptr);

    auto it = roots.begin();
    while (it != roots.end()) {
      BinomialHeapNode *current = *it;
      int d = current->degree;

      // Check if another tree with the same degree exists
      while (d < degree_table.size() && degree_table[d] != nullptr) {
        BinomialHeapNode *other = degree_table[d];

        // Ensure 'current' has the smaller key (or equal), swap if necessary
        // Smaller priority value means higher actual priority
        if (other->priority < current->priority) {
          std::swap(current, other);
        }

        // Link 'other' under 'current'
        // Need to remove 'other' from the root list before linking
        bool found = false;
        for (auto rm_it = roots.begin(); rm_it != roots.end(); ++rm_it) {
          if (*rm_it == other) {
            it = roots.erase(rm_it); // erase returns iterator to next element
            found = true;
            break;
          }
        }
        if (!found) {
          // This case should ideally not happen if logic is correct
          // If 'other' was already processed and removed in a previous step
          // we need to handle this. Resetting iterator might be needed.
          // For simplicity, let's assume 'other' is always found in the current
          // root list state. A more robust implementation might need careful
          // iterator handling.
          it = roots.begin(); // Reset iterator as a safe fallback
        }

        link(other, current);
        degree_table[d] = nullptr; // Clear the slot
        d++;                       // Move to check the next degree

        // Adjust iterator if it pointed to the removed 'other'
        if (it != roots.end() && *it == other) {
          it = roots.begin(); // Simple reset, might re-process some nodes
        } else if (it == roots.end() && !roots.empty()) {
          it = roots.begin(); // Reset if erase removed the last element
        }
      }

      // Place the (potentially merged) tree in the table
      if (d < degree_table.size()) {
        degree_table[d] = current;
      } else {
        // Need to resize if degree exceeds current table size
        degree_table.resize(d + 1, nullptr);
        degree_table[d] = current;
      }

      // Only advance iterator if we didn't erase the element it points to
      // This consolidation logic can be tricky with list iterators.
      // A simpler approach might be to rebuild the root list from degree_table
      // at the end.
      if (it != roots.end()) { // Check if iterator is still valid
        // Advance only if the current node was NOT the one erased ('other')
        // If 'current' was swapped with 'other', 'it' might now point to the
        // original 'other' which might be somewhere else or removed. Let's
        // rebuild the list for simplicity/robustness:
        goto next_node; // Using goto for clarity in this specific complex loop
                        // structure
      }
    next_node:; // Label for jump
    }

    // Rebuild root list from degree table (simpler approach)
    roots.clear();
    for (BinomialHeapNode *node : degree_table) {
      if (node != nullptr) {
        // Reset parent pointer for root nodes
        node->parent = nullptr;
        roots.push_back(node);
      }
    }
    // Ensure roots are sorted by degree (optional but conventional)
    roots.sort([](const BinomialHeapNode *a, const BinomialHeapNode *b) {
      return a->degree < b->degree;
    });
  }

  // Finds the node with the minimum priority in the root list
  std::list<BinomialHeapNode *>::iterator findMinNodeIterator() {
    if (roots.empty()) {
      return roots.end();
    }
    auto min_it = roots.begin();
    for (auto it = std::next(roots.begin()); it != roots.end(); ++it) {
      if ((*it)->priority < (*min_it)->priority) {
        min_it = it;
      }
    }
    return min_it;
  }

public:
  BinomialHeap() = default;

  // Disable copy constructor and assignment operator for simplicity
  // Managing deep copies of nodes and pointers is complex
  BinomialHeap(const BinomialHeap &) = delete;
  BinomialHeap &operator=(const BinomialHeap &) = delete;

  // Move constructor (optional but good practice)
  BinomialHeap(BinomialHeap &&other) noexcept : roots(std::move(other.roots)) {}

  // Move assignment operator (optional but good practice)
  BinomialHeap &operator=(BinomialHeap &&other) noexcept {
    if (this != &other) {
      // Clean up existing resources if necessary (though nodes are owned
      // elsewhere or deleted explicitly)
      clear(); // Need a clear method to delete nodes
      roots = std::move(other.roots);
    }
    return *this;
  }

  ~BinomialHeap() { clear(); }

  void clear() {
    // Recursively delete all nodes
    std::vector<BinomialHeapNode *> nodes_to_delete;
    for (BinomialHeapNode *root : roots) {
      collectNodes(root, nodes_to_delete);
    }
    for (BinomialHeapNode *node : nodes_to_delete) {
      delete node;
    }
    roots.clear();
  }

  void collectNodes(BinomialHeapNode *node,
                    std::vector<BinomialHeapNode *> &nodes_list) {
    if (node == nullptr)
      return;
    nodes_list.push_back(node);
    collectNodes(node->child, nodes_list);
    collectNodes(node->sibling, nodes_list);
  }

  bool isEmpty() const { return roots.empty(); }

  void insert(PriorityType priority, PassengerIdType passengerId) {
    BinomialHeapNode *new_node = new BinomialHeapNode(priority, passengerId);
    BinomialHeap temp_heap;
    temp_heap.roots.push_back(new_node);
    mergeRootLists(temp_heap); // Merge the new node's heap into this heap
    consolidate();
  }

  // Returns the passenger ID with the highest priority (lowest value) without
  // removing it
  PassengerIdType findMinPassengerId() {
    auto min_it = findMinNodeIterator();
    if (min_it == roots.end()) {
      throw std::runtime_error("Heap is empty");
    }
    return (*min_it)->passengerId;
  }

  // Returns the minimum priority value without removing node
  PriorityType findMinPriority() {
    auto min_it = findMinNodeIterator();
    if (min_it == roots.end()) {
      throw std::runtime_error("Heap is empty");
    }
    return (*min_it)->priority;
  }

  // Removes and returns the passenger ID with the highest priority (lowest
  // value)
  PassengerIdType extractMin() {
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
      // Reverse the list of children before adding to root list
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

  // Get waitlist contents (destroys heap - use only for temporary display or
  // make a copy)
  // Note: This is destructive. A non-destructive version would need copy or
  // traversal.
  std::vector<std::pair<PriorityType, PassengerIdType>>
  getWaitlistOrdered_destructive() {
    std::vector<std::pair<PriorityType, PassengerIdType>> waitlist;
    while (!isEmpty()) {
      PriorityType prio = findMinPriority();
      PassengerIdType passId = extractMin();
      waitlist.push_back({prio, passId});
    }
    return waitlist;
  }

  int getSize() const {
    // Note: This isn't efficient. Calculating size requires traversing.
    // It's better to maintain a size counter if needed frequently.
    // For now, just count roots for a rough idea (incorrect for total nodes)
    // Let's return 0 if not tracking size properly. For waitlist size:
    // To get actual size, would need traversal or a size member updated on
    // insert/extract
    int count = 0;
    std::vector<BinomialHeapNode *> nodes_to_visit;
    for (auto *root : roots)
      nodes_to_visit.push_back(root);

    int head = 0;
    while (head < nodes_to_visit.size()) {
      BinomialHeapNode *current = nodes_to_visit[head++];
      count++;
      BinomialHeapNode *child = current->child;
      while (child != nullptr) {
        nodes_to_visit.push_back(child);
        child = child->sibling;
      }
    }
    return count; // Returns total number of nodes (passengers on waitlist)
  }
};

// --- Flight Class ---

class Flight {
private:
  std::string flightId;
  std::string origin;
  std::string destination;
  int capacity;
  std::vector<PassengerIdType> confirmedPassengers;
  BinomialHeap waitlistHeap; // Binomial Heap for the waitlist

public:
  Flight(std::string id, std::string orig, std::string dest, int cap)
      : flightId(std::move(id)), origin(std::move(orig)),
        destination(std::move(dest)), capacity(cap) {}

  std::string getFlightId() const { return flightId; }
  std::string getOrigin() const { return origin; }
  std::string getDestination() const { return destination; }
  int getCapacity() const { return capacity; }
  int getBookedCount() const { return confirmedPassengers.size(); }
  int getWaitlistCount() const {
    return waitlistHeap.getSize(); /* Adjust if getSize is implemented fully */
  }

  // Attempts to add a passenger, returns true if confirmed, false if waitlisted
  bool addPassenger(PassengerIdType passengerId, PriorityType priority) {
    // Check if already booked or waitlisted (simple check)
    for (const auto &p_id : confirmedPassengers)
      if (p_id == passengerId)
        return true; // Already confirmed
    // Need a way to check waitlist without destruction - skip for now for
    // simplicity

    if (confirmedPassengers.size() < capacity) {
      confirmedPassengers.push_back(passengerId);
      std::cout << "Booking confirmed for Passenger " << passengerId
                << " on flight " << flightId << "." << std::endl;
      return true;
    } else {
      waitlistHeap.insert(priority, passengerId);
      std::cout << "Flight " << flightId << " is full. Passenger "
                << passengerId << " added to waitlist (Priority: " << priority
                << ")." << std::endl;
      return false;
    }
  }

  // Attempts to cancel a booking, returns true if successful
  bool cancelBooking(PassengerIdType passengerId) {
    // Check confirmed list
    auto it = std::find(confirmedPassengers.begin(), confirmedPassengers.end(),
                        passengerId);
    if (it != confirmedPassengers.end()) {
      confirmedPassengers.erase(it);
      std::cout << "Booking cancelled for Passenger " << passengerId
                << " on flight " << flightId << "." << std::endl;

      // Process waitlist if space opened up and waitlist is not empty
      if (!waitlistHeap.isEmpty()) {
        PassengerIdType promotedPassengerId = waitlistHeap.extractMin();
        confirmedPassengers.push_back(promotedPassengerId);
        std::cout << "Passenger " << promotedPassengerId
                  << " moved from waitlist to confirmed seat on flight "
                  << flightId << "." << std::endl;
      }
      return true;
    }

    // Check waitlist (more complex - requires finding and removing arbitrary
    // node) For simplicity, we only handle cancellation for confirmed
    // passengers here. Removing from waitlist would require
    // find+delete/decreaseKey operation in Binomial Heap.
    std::cout << "Passenger " << passengerId
              << " not found in confirmed bookings for flight " << flightId
              << ". Waitlist cancellation not implemented." << std::endl;
    return false;
  }

  void
  displayStatus(const std::map<PassengerIdType, Passenger> &passengerDb) const {
    std::cout << "----------------------------------------\n";
    std::cout << " Flight Status: " << flightId << " (" << origin << " -> "
              << destination << ")\n";
    std::cout << "----------------------------------------\n";
    std::cout << " Capacity: " << capacity << "\n";
    std::cout << " Booked:   " << confirmedPassengers.size() << "\n";
    std::cout << " Waitlist: " << getWaitlistCount()
              << "\n"; // Use the size calculation

    std::cout << "\n--- Confirmed Passengers ---\n";
    if (confirmedPassengers.empty()) {
      std::cout << " None\n";
    } else {
      for (const auto &pId : confirmedPassengers) {
        auto pIt = passengerDb.find(pId);
        if (pIt != passengerDb.end()) {
          std::cout << " ID: " << pId << ", Name: " << pIt->second.name << "\n";
        } else {
          std::cout << " ID: " << pId << ", Name: <Unknown>\n";
        }
      }
    }

    std::cout << "\n--- Waitlist (Top Priority First) ---\n";
    if (waitlistHeap.isEmpty()) {
      std::cout << " Empty\n";
    } else {
      // Displaying waitlist requires extracting or copying. Use a temporary
      // copy. This is complex to implement correctly without full copy support.
      // As a simple placeholder, just show the count.
      // A real implementation might show the top N passengers.
      std::cout << " Contains " << getWaitlistCount() << " passenger(s).\n";
      try {
        std::cout << " Next passenger on waitlist (ID): "
                  << waitlistHeap.findMinPassengerId()
                  << " (Priority: " << waitlistHeap.findMinPriority() << ")\n";
      } catch (const std::runtime_error &e) {
        // Should not happen if isEmpty() is false, but good practice
      }
      // Note: Displaying the full ordered waitlist without destruction needs
      // more heap features.
    }
    std::cout << "----------------------------------------\n";
  }

  // Provide access for waitlist display if needed (use carefully)
  const BinomialHeap &getWaitlistHeap() const { return waitlistHeap; }
  // BinomialHeap& getWaitlistHeapNonConst() { return waitlistHeap; } // If
  // modification needed
};

// --- Booking System Class (TUI) ---

class BookingSystem {
private:
  std::map<std::string, Flight> flights;
  std::map<PassengerIdType, Passenger> passengers;
  PassengerIdType nextPassengerId = 1;
  PriorityType nextBookingPriority = 1; // Lower value = higher priority

  void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    // Assume POSIX
    system("clear");
#endif
  }

  void pressEnterToContinue() {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                    '\n'); // Clear buffer before waiting
    std::cin.get();
  }

  void displayMainMenu() {
    clearScreen();
    std::cout << "========================================\n";
    std::cout << "   Airline Ticket Booking System (v0.1) \n";
    std::cout << "   (Using Binomial Heap Waitlists)      \n";
    std::cout << "========================================\n";
    std::cout << "1. List All Flights\n";
    std::cout << "2. View Flight Details\n";
    std::cout << "3. Add New Passenger\n";
    std::cout << "4. Book Ticket\n";
    std::cout << "5. Cancel Booking\n";
    std::cout << "6. Add New Flight (Admin)\n";
    std::cout << "0. Exit\n";
    std::cout << "----------------------------------------\n";
    std::cout << "Enter your choice: ";
  }

  void listAllFlights() {
    clearScreen();
    std::cout << "--- Available Flights ---\n";
    if (flights.empty()) {
      std::cout << "No flights available.\n";
    } else {
      std::cout << std::left << std::setw(10) << "Flight ID" << std::setw(15)
                << "Origin" << std::setw(15) << "Destination" << std::setw(10)
                << "Booked" << std::setw(10) << "Capacity" << std::setw(10)
                << "Waitlist" << std::endl;
      std::cout << std::setw(70) << std::setfill('-') << "" << std::setfill(' ')
                << std::endl;
      for (const auto &pair : flights) {
        const Flight &f = pair.second;
        std::cout << std::left << std::setw(10) << f.getFlightId()
                  << std::setw(15) << f.getOrigin() << std::setw(15)
                  << f.getDestination() << std::setw(10) << f.getBookedCount()
                  << std::setw(10) << f.getCapacity() << std::setw(10)
                  << f.getWaitlistCount() << std::endl;
      }
    }
    pressEnterToContinue();
  }

  void viewFlightDetails() {
    clearScreen();
    std::cout << "--- View Flight Details ---\n";
    std::string flightId;
    std::cout << "Enter Flight ID: ";
    std::cin >> flightId;

    auto it = flights.find(flightId);
    if (it != flights.end()) {
      it->second.displayStatus(passengers);
    } else {
      std::cout << "Flight ID '" << flightId << "' not found.\n";
    }
    pressEnterToContinue();
  }

  void addNewPassenger() {
    clearScreen();
    std::cout << "--- Add New Passenger ---\n";
    std::string name;
    std::cout << "Enter passenger name: ";
    // Handle potential leftover newline from previous input
    if (std::cin.peek() == '\n')
      std::cin.ignore();
    std::getline(std::cin, name);

    PassengerIdType newId = nextPassengerId++;
    passengers.emplace(newId, Passenger(newId, name));
    std::cout << "Passenger '" << name << "' added with ID: " << newId
              << std::endl;
    pressEnterToContinue();
  }

  void bookTicket() {
    clearScreen();
    std::cout << "--- Book Ticket ---\n";
    PassengerIdType passengerId;
    std::string flightId;

    std::cout << "Enter Passenger ID: ";
    while (!(std::cin >> passengerId) ||
           passengers.find(passengerId) == passengers.end()) {
      std::cout << "Invalid Passenger ID. Please try again: ";
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                    '\n'); // Consume newline

    std::cout << "Enter Flight ID: ";
    std::cin >> flightId;

    auto flightIt = flights.find(flightId);
    if (flightIt == flights.end()) {
      std::cout << "Flight ID '" << flightId << "' not found.\n";
    } else {
      // Assign priority based on booking sequence
      PriorityType currentPriority = nextBookingPriority++;
      flightIt->second.addPassenger(passengerId, currentPriority);
    }
    pressEnterToContinue();
  }

  void cancelBooking() {
    clearScreen();
    std::cout << "--- Cancel Booking ---\n";
    PassengerIdType passengerId;
    std::string flightId;

    std::cout << "Enter Passenger ID to cancel booking for: ";
    while (!(std::cin >> passengerId) ||
           passengers.find(passengerId) == passengers.end()) {
      std::cout << "Invalid or unknown Passenger ID. Please try again: ";
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                    '\n'); // Consume newline

    std::cout << "Enter Flight ID: ";
    std::cin >> flightId;

    auto flightIt = flights.find(flightId);
    if (flightIt == flights.end()) {
      std::cout << "Flight ID '" << flightId << "' not found.\n";
    } else {
      flightIt->second.cancelBooking(passengerId);
    }
    pressEnterToContinue();
  }

  void addNewFlight() {
    clearScreen();
    std::cout << "--- Add New Flight (Admin) ---\n";
    std::string id, origin, dest;
    int capacity;

    std::cout << "Enter new Flight ID: ";
    std::cin >> id;
    // Check if ID already exists
    if (flights.count(id)) {
      std::cout << "Flight ID already exists.\n";
      pressEnterToContinue();
      return;
    }

    std::cout << "Enter Origin: ";
    std::cin >> origin;
    std::cout << "Enter Destination: ";
    std::cin >> dest;
    std::cout << "Enter Capacity: ";
    while (!(std::cin >> capacity) || capacity <= 0) {
      std::cout << "Invalid capacity. Please enter a positive number: ";
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                    '\n'); // Consume newline

    flights.emplace(id, Flight(id, origin, dest, capacity));
    std::cout << "Flight " << id << " added successfully.\n";
    pressEnterToContinue();
  }

public:
  BookingSystem() {
    // Add some sample data
    passengers.emplace(nextPassengerId, Passenger(nextPassengerId, "Alice"));
    nextPassengerId++;
    passengers.emplace(nextPassengerId, Passenger(nextPassengerId, "Bob"));
    nextPassengerId++;
    passengers.emplace(nextPassengerId, Passenger(nextPassengerId, "Charlie"));
    nextPassengerId++;
    passengers.emplace(nextPassengerId, Passenger(nextPassengerId, "David"));
    nextPassengerId++;
    passengers.emplace(nextPassengerId, Passenger(nextPassengerId, "Eve"));
    nextPassengerId++;

    flights.emplace("AI101", Flight("AI101", "Delhi", "Mumbai",
                                    2)); // Small capacity for testing waitlist
    flights.emplace("BA202", Flight("BA202", "London", "NewYork", 250));
    flights.emplace("LH303",
                    Flight("LH303", "Frankfurt", "Tokyo", 3)); // Small capacity

    // Pre-book some passengers to test waitlist
    flights.at("AI101").addPassenger(1, nextBookingPriority++); // Alice
    flights.at("AI101").addPassenger(2, nextBookingPriority++); // Bob
    flights.at("AI101").addPassenger(
        3, nextBookingPriority++); // Charlie to waitlist
    flights.at("AI101").addPassenger(
        4, nextBookingPriority++); // David to waitlist

    flights.at("LH303").addPassenger(5, nextBookingPriority++); // Eve
  }

  void run() {
    int choice;
    do {
      displayMainMenu();
      while (!(std::cin >> choice)) {
        std::cout << "Invalid input. Please enter a number: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      }
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                      '\n'); // Consume potential newline

      switch (choice) {
      case 1:
        listAllFlights();
        break;
      case 2:
        viewFlightDetails();
        break;
      case 3:
        addNewPassenger();
        break;
      case 4:
        bookTicket();
        break;
      case 5:
        cancelBooking();
        break;
      case 6:
        addNewFlight();
        break;
      case 0:
        std::cout << "Exiting system. Goodbye!\n";
        break;
      default:
        std::cout << "Invalid choice. Please try again.\n";
        pressEnterToContinue();
        break;
      }
    } while (choice != 0);
  }
};

// --- Main Function ---

int main() {
  BookingSystem airlineSystem;
  airlineSystem.run();
  return 0;
}
