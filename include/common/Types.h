#pragma once // Header guard

#include <limits>

// Define common types used throughout the project
using PriorityType = int; // Lower value means HIGHER priority
using PassengerIdType = int;

const PriorityType MAX_PRIORITY = std::numeric_limits<PriorityType>::max();
const PriorityType MIN_PRIORITY =
    std::numeric_limits<PriorityType>::min(); // Useful for decreaseKey/delete
const PassengerIdType INVALID_PASSENGER_ID = -1;
