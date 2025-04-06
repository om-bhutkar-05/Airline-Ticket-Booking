#include "booking/Flight.h"
#include "core/Passenger.h" // Include Passenger definition
#include <algorithm>        // For std::find, std::find_if
#include <iomanip>          // For std::setw
#include <iostream>

Flight::Flight(std::string id, std::string orig, std::string dest, int cap)
    : flightId(std::move(id)), origin(std::move(orig)),
      destination(std::move(dest)),
      capacity(cap >= 0 ? cap : 0) // Ensure non-negative capacity
                                   // waitlistHeap is default constructed
{}

// --- Accessors ---
std::string Flight::getFlightId() const { return flightId; }
std::string Flight::getOrigin() const { return origin; }
std::string Flight::getDestination() const { return destination; }
int Flight::getCapacity() const { return capacity; }
int Flight::getBookedCount() const { return confirmedPassengers.size(); }
int Flight::getWaitlistCount() const { return waitlistHeap.getSize(); }
const BinomialHeap &Flight::getWaitlistHeap() const { return waitlistHeap; }

// --- Core Operations ---

bool Flight::addPassenger(PassengerIdType passengerId, PriorityType priority) {
  // Basic check if already confirmed (more robust check might be needed for
  // waitlist)
  if (std::find(confirmedPassengers.begin(), confirmedPassengers.end(),
                passengerId) != confirmedPassengers.end()) {
    std::cout << "Passenger " << passengerId
              << " is already confirmed on flight " << flightId << "."
              << std::endl;
    return true; // Already confirmed
  }
  // Add check for waitlist if necessary (requires search capability in heap or
  // separate tracking)

  if (confirmedPassengers.size() < static_cast<size_t>(capacity)) {
    confirmedPassengers.push_back(passengerId);
    std::cout << "Booking confirmed for Passenger " << passengerId
              << " on flight " << flightId << "." << std::endl;
    return true;
  } else {
    waitlistHeap.insert(priority, passengerId);
    std::cout << "Flight " << flightId << " is full. Passenger " << passengerId
              << " added to waitlist (Priority: " << priority << ")."
              << std::endl;
    return false;
  }
}

bool Flight::cancelBooking(PassengerIdType passengerId) {
  // Check confirmed list
  auto it = std::find(confirmedPassengers.begin(), confirmedPassengers.end(),
                      passengerId);
  if (it != confirmedPassengers.end()) {
    confirmedPassengers.erase(it);
    std::cout << "Booking cancelled for Passenger " << passengerId
              << " on flight " << flightId << "." << std::endl;

    // Process waitlist if space opened up and waitlist is not empty
    if (!waitlistHeap.isEmpty()) {
      try {
        PassengerIdType promotedPassengerId = waitlistHeap.extractMin();
        confirmedPassengers.push_back(promotedPassengerId);
        std::cout << "Passenger " << promotedPassengerId
                  << " moved from waitlist to confirmed seat on flight "
                  << flightId << "." << std::endl;
      } catch (const std::runtime_error &e) {
        // Should not happen if isEmpty is false, but good practice
        std::cerr << "Error processing waitlist after cancellation: "
                  << e.what() << std::endl;
      }
    }
    return true;
  }

  // Waitlist cancellation is not implemented here (requires search/delete in
  // heap)
  std::cout << "Passenger " << passengerId
            << " not found in confirmed bookings for flight " << flightId
            << ". Waitlist cancellation not supported in this version."
            << std::endl;
  return false;
}

// --- Display ---

void Flight::displayStatus(
    const std::map<PassengerIdType, Passenger> &passengerDb) const {
  std::cout << "----------------------------------------\n";
  std::cout << " Flight Status: " << flightId << " (" << origin << " -> "
            << destination << ")\n";
  std::cout << "----------------------------------------\n";
  std::cout << " Capacity: " << capacity << "\n";
  std::cout << " Booked:   " << getBookedCount() << "\n";
  std::cout << " Waitlist: " << getWaitlistCount() << "\n";

  std::cout << "\n--- Confirmed Passengers ---\n";
  if (confirmedPassengers.empty()) {
    std::cout << " None\n";
  } else {
    for (const auto &pId : confirmedPassengers) {
      auto pIt = passengerDb.find(pId);
      std::cout << " ID: " << std::setw(4) << pId << ", Name: ";
      if (pIt != passengerDb.end()) {
        std::cout << pIt->second.name << "\n";
      } else {
        std::cout << "<Unknown Passenger>\n";
      }
    }
  }

  std::cout << "\n--- Waitlist (" << getWaitlistCount() << " waiting) ---\n";
  if (waitlistHeap.isEmpty()) {
    std::cout << " Empty\n";
  } else {
    // Displaying the top person on waitlist
    try {
      PassengerIdType nextPassId = waitlistHeap.findMinPassengerId();
      PriorityType nextPrio = waitlistHeap.findMinPriority();
      auto pIt = passengerDb.find(nextPassId);
      std::cout << " Next: ID: " << std::setw(4) << nextPassId << ", Name: ";
      if (pIt != passengerDb.end()) {
        std::cout << pIt->second.name;
      } else {
        std::cout << "<Unknown Passenger>";
      }
      std::cout << " (Priority: " << nextPrio << ")\n";
    } catch (const std::runtime_error &e) {
      std::cerr << " Could not retrieve next passenger from waitlist: "
                << e.what() << std::endl;
    }
    // Note: Displaying the full ordered waitlist requires a non-destructive
    // method or copying the heap.
  }
  std::cout << "----------------------------------------\n";
}
