#ifndef AIRLINE_BOOKING_SYSTEM_H
#define AIRLINE_BOOKING_SYSTEM_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

// Priority levels for tickets
enum class TicketPriority { ECONOMY = 0, BUSINESS = 1, FIRST_CLASS = 2 };

// Simple date and passenger structures
struct Date { int day, month, year; };
struct Passenger { std::string id, name; std::string contact; };

// Flight class - basic flight information
class Flight {
private:
    std::string flightNumber;
    std::string route; // departure-arrival
    Date departureDate;
    int availableSeats;
    double basePrice;
    
public:
    Flight(const std::string& number, const std::string& route, 
           const Date& date, int seats, double price);
    
    std::string getFlightNumber() const;
    bool bookSeat();
    void cancelSeat();
};

// Ticket class - will be used in priority queue
class Ticket {
private:
    std::string ticketID;
    std::string flightNumber;
    std::string passengerID;
    TicketPriority priority;
    bool confirmed;
    
public:
    Ticket(const std::string& id, const std::string& flight, 
           const std::string& passenger, TicketPriority prio);
    
    TicketPriority getPriority() const;
    
    // For priority queue comparison
    bool operator<(const Ticket& other) const;
};

// Binary Heap implementation for Priority Queue
template <typename T>
class PriorityQueue {
private:
    std::vector<T> heap;
    
    void heapifyUp(int index);
    void heapifyDown(int index);
    int parent(int i) const { return (i - 1) / 2; }
    int leftChild(int i) const { return 2 * i + 1; }
    int rightChild(int i) const { return 2 * i + 2; }
    
public:
    void push(const T& item);
    T pop();
    T& top();
    bool empty() const;
    size_t size() const;
};

// Booking System class
class BookingSystem {
private:
    std::unordered_map<std::string, Flight> flights;
    std::unordered_map<std::string, Passenger> passengers;
    std::unordered_map<std::string, Ticket> tickets;
    
    // Priority Queue for processing ticket bookings
    PriorityQueue<Ticket> bookingQueue;
    
public:
    void addFlight(const Flight& flight);
    std::string bookTicket(const std::string& flightNumber, 
                          const std::string& passengerID,
                          TicketPriority priority);
    bool cancelTicket(const std::string& ticketID);
    void processBookingQueue();
};

// Template implementation for basic PriorityQueue operations
template <typename T>
void PriorityQueue<T>::push(const T& item) {
    heap.push_back(item);
    heapifyUp(heap.size() - 1);
}

template <typename T>
T PriorityQueue<T>::pop() {
    T root = heap.front();
    heap[0] = heap.back();
    heap.pop_back();
    if (!empty()) heapifyDown(0);
    return root;
}

template <typename T>
T& PriorityQueue<T>::top() { return heap.front(); }

template <typename T>
bool PriorityQueue<T>::empty() const { return heap.empty(); }

template <typename T>
size_t PriorityQueue<T>::size() const { return heap.size(); }

template <typename T>
void PriorityQueue<T>::heapifyUp(int index) {
    if (index && heap[parent(index)] < heap[index]) {
        std::swap(heap[index], heap[parent(index)]);
        heapifyUp(parent(index));
    }
}

template <typename T>
void PriorityQueue<T>::heapifyDown(int index) {
    int largest = index;
    int left = leftChild(index);
    int right = rightChild(index);
    
    if (left < size() && heap[largest] < heap[left])
        largest = left;
    
    if (right < size() && heap[largest] < heap[right])
        largest = right;
    
    if (largest != index) {
        std::swap(heap[index], heap[largest]);
        heapifyDown(largest);
    }
}

#endif // AIRLINE_BOOKING_SYSTEM_H