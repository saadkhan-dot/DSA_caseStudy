# Intelligence Communication Network

## 2.1 Project Title

**Intelligence Communication Network: Secure Message Routing, Archive Retrieval, and Network Failure Recovery using Data Structures & Algorithms**

---

# 2.2 Problem Statement

Modern intelligence agencies and secure communication networks handle large volumes of encrypted messages that must be processed, routed, archived, and recovered efficiently. The system must support message transmission, route discovery, failure recovery, historical record retrieval, and network monitoring while maintaining performance and reliability.

The challenge is to design a communication network simulator that demonstrates the practical use of fundamental Data Structures and Algorithms (DSA) concepts such as Queues, Graphs, DFS, Binary Search, and Stacks.

---

# 2.3 Objectives

The objectives of this project are:

* Simulate secure communication between intelligence offices.
* Process intercepted encrypted messages efficiently.
* Find optimal communication routes between offices.
* Discover backup routes during network failures.
* Retrieve archived communication records quickly.
* Support rollback and recovery of network configurations.
* Visualize the communication topology through a terminal-based interface.
* Demonstrate practical applications of DSA concepts.

---

# 2.4 System Overview / Architecture

The system consists of five major modules:

```text
                    +----------------+
                    |  messages.json |
                    +--------+-------+
                             |
                             v
                    +----------------+
                    |   JsonLoader   |
                    +--------+-------+
                             |
         +-------------------+-------------------+
         |                                       |
         v                                       v
+--------------------+              +----------------------+
| HistoricalDatabase |              |    MessageQueue      |
+---------+----------+              +----------+-----------+
          |                                    |
          |                                    |
          v                                    v
+---------------------------------------------------------+
|                    NetworkGraph                         |
|      (Routing, DFS, Dijkstra, Rollback Stack)           |
+-------------------------+-------------------------------+
                          |
                          v
                 +----------------+
                 |TerminalDisplay |
                 +----------------+
```

### Module Description

**JsonLoader**

* Loads and validates communication records from JSON files.

**MessageQueue**

* Stores intercepted messages using Queue.

**HistoricalDatabase**

* Stores archived records sorted by timestamp.

**NetworkGraph**

* Represents communication offices and links.

**TerminalDisplay**

* Provides terminal-based user interface.

---

# 2.5 Data Structures and Algorithms Used

## Queue

**Implementation:**

```cpp
std::queue<Message>
```

**Purpose:**
Processes encrypted messages in FIFO order.

**Operations:**

* enqueue()
* dequeue()
* front()

---

## Graph

**Implementation:**

```cpp
unordered_map<string, vector<Edge>>
```

**Purpose:**
Represents communication offices and communication links.

---

## Depth First Search (DFS)

**Implementation:**

```cpp
dfsBackupChannels()
```

**Purpose:**
Finds backup communication routes during failures.

---

## Dijkstra's Algorithm

**Implementation:**

```cpp
findShortestPath()
```

**Purpose:**
Calculates the shortest communication path between offices.

---

## Binary Search

**Implementation:**

```cpp
searchByTimestamp()
```

**Purpose:**
Efficient retrieval of archived communication records.

---

## Stack

**Implementation:**

```cpp
std::stack<LinkAction>
```

**Purpose:**
Supports rollback and recovery operations.

---

## Additional Data Structures

### Vector

```cpp
std::vector<Message>
```

Used for archive storage and path storage.

### Deque

```cpp
std::deque<std::string>
```

Used for action log management.

### Set

```cpp
std::set<std::string>
```

Used to track visited nodes during DFS.

### Priority Queue

```cpp
std::priority_queue
```

Used by Dijkstra's Algorithm.

---

# 2.6 Implementation Approach

### Step 1: Load Communication Records

* JSON file is parsed using JsonLoader.
* Invalid records are skipped.
* Valid records are converted into Message objects.

### Step 2: Store Historical Records

* Records are stored in HistoricalDatabase.
* Records are sorted by timestamp.

### Step 3: Build Communication Network

* Communication offices are represented as nodes.
* Communication links are represented as weighted edges.

### Step 4: Message Processing

* New intercepted messages are added to MessageQueue.
* Messages are dispatched in FIFO order.

### Step 5: Route Discovery

* Dijkstra Algorithm calculates shortest routes.
* DFS identifies backup communication channels.

### Step 6: Failure Recovery

* Communication links can be severed.
* Rollback Stack restores recently severed links.

### Step 7: User Interface

* TerminalDisplay renders network information.
* Logs and queue status are displayed dynamically.

---

# 2.7 Time and Space Complexity Analysis

| Operation             | Algorithm/Data Structure | Time Complexity  |
| --------------------- | ------------------------ | ---------------- |
| Enqueue Message       | Queue                    | O(1)             |
| Dequeue Message       | Queue                    | O(1)             |
| Archive Search        | Binary Search            | O(log n)         |
| Closest Record Search | Binary Search            | O(log n)         |
| DFS Backup Route      | DFS                      | O(V + E)         |
| Shortest Path         | Dijkstra                 | O((V + E) log V) |
| Rollback Operation    | Stack                    | O(1)             |
| Add Link              | Graph                    | O(1)             |
| Restore Link          | Graph                    | O(E)             |

### Space Complexity

| Structure           | Space Complexity |
| ------------------- | ---------------- |
| Queue               | O(n)             |
| Graph               | O(V + E)         |
| Historical Database | O(n)             |
| Rollback Stack      | O(n)             |

---

# 2.8 Execution Steps

### Compile

```bash
g++ *.cpp -o communication_network
```

### Run

```bash
./communication_network
```

### Available Commands

```text
1. INTERCEPT
2. DISPATCH
3. PATHFIND
4. BLACKOUT
5. RESTORE
6. GHOSTPATH
7. ARCHIVES
8. GRIDMAP
9. ROLLBACK
0. DISCONNECT
```

---

# 2.9 Sample Inputs and Outputs

## Example 1: Path Finding

### Input

```text
PATHFIND
Source: Mumbai
Destination: Moscow
```

### Output

```text
Shortest Route:

Mumbai
London
Moscow

Total Cost: 25 ms
```

---

## Example 2: Archive Search

### Input

```text
ARCHIVES
Timestamp: 1715000000
```

### Output

```text
Step 1 | Index 5 | TS: 1714000000 | Too LOW --> go RIGHT
Step 2 | Index 8 | TS: 1715000000 | MATCH FOUND

Record Retrieved Successfully
```

---

## Example 3: Rollback

### Input

```text
BLACKOUT Mumbai-Berlin
ROLLBACK
```

### Output

```text
Rollback Successful

Communication Link Restored:
Mumbai <-> Berlin
```

---

# 2.10 Screenshots

Add screenshots of:

1. Main Menu
2. Queue Status
3. Shortest Path Output
4. DFS Backup Route Output
5. Archive Search Output
6. GRIDMAP Network View
7. Rollback Operation
8. Communication Link Failure Simulation

Example:

```md
![Main Menu](screenshots/main_menu.png)
<img width="537" height="268" alt="Screenshot 2026-06-18 at 8 08 29 AM" src="https://github.com/user-attachments/assets/f9f8bd8d-5729-4735-892e-b759d0a13bf8" />


![Grid Map](screenshots/gridmap.png)

![Archive Search](screenshots/archive_search.png)
```

---

# 2.11 Results and Observations

* Queue successfully maintained FIFO message processing.
* Graph efficiently represented communication offices and links.
* Dijkstra Algorithm correctly calculated shortest communication routes.
* DFS successfully discovered backup communication channels.
* Binary Search significantly improved archive retrieval performance.
* Stack provided efficient rollback and recovery functionality.
* Terminal-based interface enabled clear visualization of network activity.

---

# 2.12 Conclusion

The Intelligence Communication Network successfully demonstrates the practical application of fundamental Data Structures and Algorithms in a realistic communication environment. The project integrates Queues, Graphs, DFS, Dijkstra’s Algorithm, Binary Search, and Stacks to manage message processing, routing, archive retrieval, and failure recovery.

The system provides an efficient and scalable solution for secure communication simulation while highlighting the importance of choosing appropriate data structures and algorithms for real-world applications.
