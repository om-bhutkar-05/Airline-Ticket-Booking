#include "booking/BookingSystem.h" // Include the main system class header

int main() {
  // Create the booking system object (loads sample data in constructor)
  BookingSystem airlineSystem;

  // Run the main application loop (TUI)
  airlineSystem.run();

  return 0;
}
