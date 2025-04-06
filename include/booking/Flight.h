#pragma once // Header guard

#include "common/Types.h"
#include "core/Passenger.h"    // Needed for displayStatus signature
#include "heap/BinomialHeap.h" // Contains a BinomialHeap member
#include <map>                 // Needed for displayStatus signature
#include <string>
#include <vector>

class Flight {
private:
  std::string flightId;
  std::string origin;
  std::string destination;
  int capacity;
  std::vector<PassengerIdType> confirmedPassengers;
  BinomialHeap waitlistHeap; // Binomial Heap for the waitlist

public:
  Flight(std::string id, std::string orig, std::string dest, int cap);

  // Accessors
  std::string getFlightId() const;
  std::string getOrigin() const;
  std::string getDestination() const;
  int getCapacity() const;
  int getBookedCount() const;
  int getWaitlistCount() const; // Uses heap's getSize()

  // Core Operations
  bool addPassenger(
      PassengerIdType passengerId,
      PriorityType priority); // Returns true if confirmed, false if waitlisted
  bool cancelBooking(
      PassengerIdType passengerId); // Returns true if successful (handles
                                    // promotion from waitlist)

  // Display
  // Takes passengerDb to look up names
  void
  displayStatus(const std::map<PassengerIdType, Passenger> &passengerDb) const;

  // Allow read-only access to heap if needed externally (e.g., for advanced
  // display)
  const BinomialHeap &getWaitlistHeap() const;
};
