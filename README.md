# C++ Airline Booking System with Binomial Heap Waitlists

## Overview

This project implements a simple console-based Airline Ticket Booking System in C++. Its primary focus is to demonstrate the implementation and application of a **Binomial Heap** data structure used as a priority queue for managing flight waitlists.

When a flight reaches its capacity, passengers attempting to book are added to a waitlist specific to that flight. The waitlist is managed by a Binomial Heap, ensuring that passengers are prioritized based on their booking attempt sequence (first-come, first-served for the waitlist). When a confirmed passenger cancels, the highest priority passenger from the waitlist is automatically promoted to a confirmed seat.

The application provides a Text-based User Interface (TUI) for interaction via the console.

## Features

- **Flight Management:**
  - Add new flights with ID, origin, destination, and capacity.
  - List all available flights with their current status (booked, capacity, waitlist count).
  - View detailed status for a specific flight, including:
    - Confirmed passenger list (ID and Name).
    - Waitlist size.
    - Details of the next passenger on the waitlist (ID, Name, Priority).
- **Passenger Management:**
  - Add new passengers to the system (assigned a unique ID).
- **Booking & Cancellation:**
  - Book tickets for a specific passenger on a specific flight.
    - Confirmation if seats are available.
    - Automatic addition to the flight's waitlist (managed by Binomial Heap) if the flight is full. Priority is assigned based on booking attempt order (lower number = higher priority).
  - Cancel tickets for confirmed passengers.
    - If the waitlist for that flight is not empty, the highest priority passenger is automatically promoted from the waitlist to a confirmed seat.
- **Binomial Heap Waitlist:**
  - Each flight maintains its own independent waitlist using a custom Binomial Heap implementation.
  - Supports core priority queue operations: `insert`, `extractMin`, `findMin`.

## Project Structure

The project is organized into separate header and source files for better modularity:

```
airline_booking/
├── include/ # Header files (.h)
│ ├── common/
│ │ └── Types.h # Common type definitions (PriorityType, etc.)
│ ├── core/
│ │ └── Passenger.h # Passenger struct definition
│ ├── heap/
│ │ ├── BinomialHeap.h # BinomialHeap class declaration
│ │ └── BinomialHeapNode.h # BinomialHeapNode struct definition
│ └── booking/
│ ├── BookingSystem.h # BookingSystem class declaration (TUI manager)
│ └── Flight.h # Flight class declaration
├── src/ # Source files (.cpp)
│ ├── heap/
│ │ └── BinomialHeap.cpp # BinomialHeap method implementations
│ └── booking/
│ ├── BookingSystem.cpp # BookingSystem method implementations
│ └── Flight.cpp # Flight method implementations
├── main.cpp # Main application entry point
└── Makefile # Build instructions (for Make utility)
```

## Core Data Structures

- **`Passenger`**: Simple struct holding passenger `id` and `name`.
- **`BinomialHeapNode`**: Represents a node within the Binomial Heap, storing priority, passenger ID, degree, and pointers (parent, child, sibling).
- **`BinomialHeap`**: The core data structure implementation. It acts as a min-priority queue (lower priority value means higher actual priority). It manages `BinomialHeapNode`s and provides operations like `insert`, `extractMin`, `findMin`, `isEmpty`, `getSize`. Each `Flight` instance contains one `BinomialHeap`.
- **`Flight`**: Represents a flight with details (ID, origin, destination, capacity). It holds a list of confirmed passenger IDs and a `BinomialHeap` instance to manage its waitlist.
- **`BookingSystem`**: The main application class. It manages collections of `Flight` and `Passenger` objects and controls the main Text User Interface (TUI) loop.

## Binomial Heap Implementation Details

- **Priority:** Uses an integer (`PriorityType`). In this system, a lower value indicates higher priority, simulating an earlier booking attempt time via a simple incrementing counter (`nextBookingPriority` in `BookingSystem`).
- **Operations:** Implements the essential heap operations required for the waitlist functionality:
  - `insert(priority, passengerId)`: Adds a passenger to the heap.
  - `extractMin()`: Removes and returns the passenger ID with the highest priority (lowest value).
  - `findMinPassengerId()`, `findMinPriority()`: Returns details of the highest priority passenger without removing them (these are `const` methods).
  - `isEmpty()`, `getSize()`, `clear()`: Utility methods.
  - Internal helpers: `link`, `mergeRootLists`, `consolidate`, `findMinNodeIterator`.
- **Memory:** Nodes are dynamically allocated (`new`) and deallocated (`delete`) within the heap implementation (primarily in `insert`, `extractMin`, and `clear`/destructor).

## How to Build and Run

### Prerequisites

- A C++ compiler supporting C++11 or later (e.g., `g++`, `clang++`).
- The `make` build utility.

### Build Instructions

1.  Clone or download the project source code.
2.  Open a terminal or command prompt.
3.  Navigate to the root directory of the project (the `airline_booking` folder containing the `Makefile`).
4.  Run the `make` command:
    ```bash
    make
    ```
    This will compile all the necessary source files and create an executable file named `airline_booking` (or `airline_booking.exe` on Windows).

### Run Instructions

1.  After successful compilation, run the executable from the same directory:
    ```bash
    ./airline_booking
    ```
    (or `.\airline_booking.exe` on Windows PowerShell, or `airline_booking.exe` in Windows Command Prompt)
2.  The console-based menu will appear, allowing you to interact with the system.

## Usage

Follow the on-screen menu prompts:

1.  **List Flights (1):** See a summary of all flights, including booked counts and waitlist sizes.
2.  **View Flight Details (2):** Enter a Flight ID to see confirmed passengers and the next person on the waitlist (if any).
3.  **Add Passenger (3):** Create a new passenger record. Note the assigned Passenger ID for booking.
4.  **Book Ticket (4):** Enter a valid Passenger ID and Flight ID. The system will confirm the booking or add the passenger to the waitlist.
5.  **Cancel Booking (5):** Enter the Passenger ID and Flight ID for a _confirmed_ booking. If successful and the waitlist is populated, the next passenger will be promoted. (Cancellation from the waitlist itself is not implemented in this version).
6.  **Add Flight (6):** Add a new flight route to the system.
7.  **Exit (0):** Terminate the application.

_Example Workflow:_ Add a few passengers. Add a flight with low capacity (e.g., 2). Book tickets for 3 different passengers on that flight – the first two get confirmed, the third goes to the waitlist. View the flight details to see the waitlist status. Cancel one of the confirmed bookings. View the details again to see the waitlisted passenger promoted.

## Potential Improvements / Future Work

- **Non-destructive Waitlist View:** Implement a way to display the entire ordered waitlist without modifying the heap (e.g., heap copy or iterator).
- **Waitlist Cancellation:** Implement `delete` or `decreaseKey` operations in the Binomial Heap to allow removal of passengers directly from the waitlist.
- **Data Persistence:** Save and load flight and passenger data to/from files (e.g., CSV, JSON, binary) so state is maintained between runs.
- **Advanced Priority:** Implement more complex priority schemes (e.g., using actual timestamps, considering frequent flyer status, fare class).
- **Robust Error Handling:** Add more comprehensive checks for invalid inputs and edge cases.
- **Unit Testing:** Create unit tests, especially for the `BinomialHeap` class, to ensure correctness.
- **Memory Management:** Consider using smart pointers (`std::unique_ptr`, `std::shared_ptr`) for node management within the heap, although manual management is common for learning purposes.
- **GUI:** Develop a graphical user interface (using libraries like Qt, wxWidgets) or a web interface (requiring a C++ web framework and front-end code).

---
