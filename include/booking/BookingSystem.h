// include/booking/BookingSystem.h
#pragma once

#include "common/Types.h"
#include <map>
#include <string>
#include <vector>
// REMOVE Forward declarations:
// class Flight;
// struct Passenger;

// ADD Includes for full definitions:
#include "booking/Flight.h"
#include "core/Passenger.h"

class BookingSystem {
private:
  // These map declarations now have the full type information for Flight and
  // Passenger
  std::map<std::string, Flight> flights;
  std::map<PassengerIdType, Passenger> passengers;
  PassengerIdType nextPassengerId;
  PriorityType nextBookingPriority;

  // --- Private TUI Helper Methods ---
  void clearScreen();
  void pressEnterToContinue();
  void displayMainMenu();
  void listAllFlights();
  void viewFlightDetails();
  void addNewPassenger();
  void bookTicket();
  void cancelBooking();
  void addNewFlight();
  void loadSampleData();

public:
  BookingSystem();
  void run();
};
