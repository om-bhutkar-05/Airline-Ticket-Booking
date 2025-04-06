#include "booking/BookingSystem.h"
#include "booking/Flight.h" // Include full definitions now
#include "core/Passenger.h" // Include full definitions now
#include <cstdlib>          // For system()
#include <iomanip>          // For std::setw, std::left
#include <iostream>
#include <limits>    // For std::numeric_limits
#include <stdexcept> // For error handling (optional)

// --- Constructor ---
BookingSystem::BookingSystem() : nextPassengerId(1), nextBookingPriority(1) {
  loadSampleData();
}

// --- Private TUI Helper Method Implementations ---

void BookingSystem::clearScreen() {
#ifdef _WIN32
  system("cls");
#else
  // Assume POSIX
  system("clear");
#endif
}

void BookingSystem::pressEnterToContinue() {
  std::cout << "\nPress Enter to continue...";
  // Clear the input buffer before waiting
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  std::cin.get();
}

void BookingSystem::displayMainMenu() {
  clearScreen();
  std::cout << "========================================\n";
  std::cout << "   Airline Ticket Booking System (Modular)\n";
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

void BookingSystem::listAllFlights() {
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
              << std::endl; // Divider line
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

void BookingSystem::viewFlightDetails() {
  clearScreen();
  std::cout << "--- View Flight Details ---\n";
  std::string flightId;
  std::cout << "Enter Flight ID: ";
  std::cin >> flightId;
  // Clear buffer after reading string/number before potential getline
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

  auto it = flights.find(flightId);
  if (it != flights.end()) {
    // Pass the passenger map to the display function
    it->second.displayStatus(passengers);
  } else {
    std::cout << "Flight ID '" << flightId << "' not found.\n";
  }
  pressEnterToContinue();
}

void BookingSystem::addNewPassenger() {
  clearScreen();
  std::cout << "--- Add New Passenger ---\n";
  std::string name;
  std::cout << "Enter passenger name: ";
  // Use getline to read names with spaces
  std::getline(std::cin, name); // Assumes previous newline was consumed

  if (name.empty()) {
    std::cout << "Passenger name cannot be empty.\n";
  } else {
    PassengerIdType newId = nextPassengerId++;
    passengers.emplace(newId, Passenger(newId, name));
    std::cout << "Passenger '" << name << "' added with ID: " << newId
              << std::endl;
  }
  pressEnterToContinue();
}

void BookingSystem::bookTicket() {
  clearScreen();
  std::cout << "--- Book Ticket ---\n";
  PassengerIdType passengerId = INVALID_PASSENGER_ID;
  std::string flightId;

  std::cout << "Enter Passenger ID: ";
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
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                  '\n'); // Consume newline

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

void BookingSystem::cancelBooking() {
  clearScreen();
  std::cout << "--- Cancel Booking ---\n";
  PassengerIdType passengerId = INVALID_PASSENGER_ID;
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
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                  '\n'); // Consume newline

  auto flightIt = flights.find(flightId);
  if (flightIt == flights.end()) {
    std::cout << "Flight ID '" << flightId << "' not found.\n";
  } else {
    flightIt->second.cancelBooking(passengerId);
  }
  pressEnterToContinue();
}

void BookingSystem::addNewFlight() {
  clearScreen();
  std::cout << "--- Add New Flight (Admin) ---\n";
  std::string id, origin, dest;
  int capacity = -1;

  std::cout << "Enter new Flight ID: ";
  std::cin >> id;
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                  '\n'); // Consume newline
                         // Check if ID already exists
  if (flights.count(id)) {
    std::cout << "Flight ID '" << id << "' already exists.\n";
    pressEnterToContinue();
    return;
  }

  std::cout << "Enter Origin: ";
  std::getline(std::cin,
               origin); // Use getline for potentially multi-word names
  std::cout << "Enter Destination: ";
  std::getline(std::cin, dest);

  std::cout << "Enter Capacity: ";
  while (!(std::cin >> capacity) ||
         capacity < 0) { // Allow 0 capacity? Let's allow >=0
    std::cout << "Invalid capacity. Please enter a non-negative number: ";
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                  '\n'); // Consume newline

  // Use emplace for efficiency
  flights.emplace(std::piecewise_construct, std::forward_as_tuple(id),
                  std::forward_as_tuple(id, origin, dest, capacity));

  std::cout << "Flight " << id << " added successfully.\n";
  pressEnterToContinue();
}

void BookingSystem::loadSampleData() {
  // Add sample passengers
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
  passengers.emplace(nextPassengerId, Passenger(nextPassengerId, "Frank"));
  nextPassengerId++;

  // Add sample flights using emplace
  // Use piecewise_construct to construct Flight in place
  flights.emplace(
      std::piecewise_construct, std::forward_as_tuple("AI101"),
      std::forward_as_tuple("AI101", "Delhi", "Mumbai",
                            2)); // Small capacity for testing waitlist

  flights.emplace(std::piecewise_construct, std::forward_as_tuple("BA202"),
                  std::forward_as_tuple("BA202", "London", "NewYork", 250));

  flights.emplace(std::piecewise_construct, std::forward_as_tuple("LH303"),
                  std::forward_as_tuple("LH303", "Frankfurt", "Tokyo", 3));

  // Pre-book some passengers to test waitlist, ensuring flights exist
  if (flights.count("AI101")) {
    flights.at("AI101").addPassenger(1, nextBookingPriority++); // Alice
    flights.at("AI101").addPassenger(2, nextBookingPriority++); // Bob
    flights.at("AI101").addPassenger(
        3, nextBookingPriority++); // Charlie to waitlist
    flights.at("AI101").addPassenger(
        4, nextBookingPriority++); // David to waitlist
  }
  if (flights.count("LH303")) {
    flights.at("LH303").addPassenger(5, nextBookingPriority++); // Eve
  }
  // Clear the console messages generated during setup
  // Note: suppressing cout during setup might be better
  // std::cout.setstate(std::ios_base::failbit); // Suppress
  // ... setup calls ...
  // std::cout.clear(); // Restore
  std::cout << "\nSample data loaded.\n"; // Indicate setup complete
}

// --- Public Run Method ---

void BookingSystem::run() {
  int choice = -1;
  do {
    displayMainMenu();
    // Input validation loop
    while (!(std::cin >> choice)) {
      std::cout << "Invalid input. Please enter a number: ";
      std::cin.clear(); // Clear error flags
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                      '\n'); // Discard bad input
    }
    // Consume the newline character left by operator>>
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

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
