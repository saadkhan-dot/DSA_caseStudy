#include "MessageQueue.h"
#include <sstream>
#include <queue>
#include <ctime>

std::string Message::toString() const {
    // Convert timestamp to readable date
    time_t t = (time_t)timestamp;
    struct tm* tm_info = localtime(&t);
    char timebuf[30];
    strftime(timebuf, sizeof(timebuf), "%Y-%m-%d | %H:%M:%S", tm_info);

    // Convert priority number to word
    std::string level;
    switch(priority) {
        case 1: level = "ROUTINE";  break;
        case 2: level = "LOW";      break;
        case 3: level = "MODERATE"; break;
        case 4: level = "ALERT";    break;
        case 5: level = "CRITICAL"; break;
        default: level = "UNKNOWN"; break;
    }

    std::ostringstream oss;
    oss << "\n             [" << msgId << "]\n"
        << "             FROM  : " << origin << "\n"
        << "             TO    : " << destination << "\n"
        << "             TIME  : " << timebuf << "\n"
        << "             LEVEL : " << level << "\n"
        << "             MSG   : " << payload;
    return oss.str();
}
MessageQueue::MessageQueue(size_t cap) : capacity(cap) {}

bool MessageQueue::enqueue(const Message& msg) {
    if (isFull()) {
        // Graceful handling of queue overflow -- do NOT crash.
        return false;
    }
    internalQueue.push(msg);
    return true;
}

bool MessageQueue::dequeue(Message& out) {
    if (isEmpty()) {
        return false;
    }
    out = internalQueue.front();
    internalQueue.pop();
    return true;
}

bool MessageQueue::isFull() const {
    return internalQueue.size() >= capacity;
}

bool MessageQueue::isEmpty() const {
    return internalQueue.empty();
}

size_t MessageQueue::size() const {
    return internalQueue.size();
}

size_t MessageQueue::getCapacity() const {
    return capacity;
}

std::vector<Message> MessageQueue::snapshot() const {
    std::vector<Message> result;
    std::queue<Message> copy = internalQueue; // copy to avoid mutating original
    while (!copy.empty()) {
        result.push_back(copy.front());
        copy.pop();
    }
    return result;
}

//snapshot() creates a copy of the queue and stores every message from that copy into a vector.

// Then it returns the vector.