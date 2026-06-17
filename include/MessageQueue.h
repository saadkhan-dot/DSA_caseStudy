#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <string>
#include <queue>
#include <vector>
#include <stdexcept>

// ===================== MESSAGE STRUCT =====================
struct Message {
    std::string msgId;          // e.g. "MSG-00231"
    std::string origin;         // source node, e.g. "STATION_ZULU_9"
    std::string destination;    // target node, e.g. "ECHO_RELAY_3"
    std::string payload;        // encrypted payload string
    long timestamp;             // unix-style timestamp (for binary search)
    int priority;               // 1 = routine, 5 = critical

    std::string toString() const;
};

// ===================== MESSAGE QUEUE =====================
// Encapsulates a bounded FIFO queue of intercepted/encrypted messages.
// Processes messages strictly in arrival order (Queue ADT).
class MessageQueue {
public:
    explicit MessageQueue(size_t capacity = 50);

    // Returns false (instead of throwing/crashing) if the queue is full.
    bool enqueue(const Message& msg);

    // Returns false if queue is empty; result written into 'out'.
    bool dequeue(Message& out);

    bool isFull() const;
    bool isEmpty() const;
    size_t size() const;
    size_t getCapacity() const;

    // Returns a snapshot of current queue contents (front to back)
    std::vector<Message> snapshot() const;

private:
    std::queue<Message> internalQueue;
    size_t capacity;
};

#endif // MESSAGE_QUEUE_H
