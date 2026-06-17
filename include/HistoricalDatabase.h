#ifndef HISTORICAL_DATABASE_H
#define HISTORICAL_DATABASE_H

#include <string>
#include <sstream>
#include <vector>
#include "MessageQueue.h" // for Message struct

// ===================== HISTORICAL DATABASE =====================
// Stores archived intelligence messages sorted by timestamp.
// Provides O(log n) lookup via Binary Search.
class HistoricalDatabase {
public:
    void loadRecords(const std::vector<Message>& records);

    // Searches for an exact timestamp. Returns true if found, fills 'result'.
  bool searchByTimestamp(long timestamp, Message& result, std::vector<std::string>& steps) const;

    // Returns index of closest record (for "near this time" queries), or -1 if empty.
    int findClosest(long timestamp) const;

    size_t size() const;
    const std::vector<Message>& getRecords() const;

private:
    std::vector<Message> records; // kept sorted by timestamp
};

#endif // HISTORICAL_DATABASE_H
