#pragma once // Header guard

#include "common/Types.h" // Include common type definitions
#include <string>

struct Passenger {
  PassengerIdType id;
  std::string name;

  // Default constructor for map usage
  Passenger() : id(INVALID_PASSENGER_ID), name("Unknown") {}

  Passenger(PassengerIdType _id, std::string _name)
      : id(_id), name(std::move(_name)) {}
};
