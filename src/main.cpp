#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>
#include <ctime>
#include <unordered_map>
#include <cctype>

#include "TerminalDisplay.h"
#include "MessageQueue.h"
#include "NetworkGraph.h"
#include "HistoricalDatabase.h"
#include "JsonLoader.h"

// ===================== MORSE CODE ENCODER =====================
static std::string toMorse(const std::string& text) {
    static const std::unordered_map<char, std::string> morse = {
        {'A',".-"},  {'B',"-..."},{'C',"-.-."},{'D',"-.." },{'E',"."},
        {'F',"..-."},{'G',"--."},  {'H',"...."},{'I',".."},  {'J',".---"},
        {'K',"-.-"}, {'L',".-.."},{'M',"--"},  {'N',"-."},  {'O',"---"},
        {'P',".--."},{'Q',"--.-"},{'R',".-."},  {'S',"..."},{'T',"-"},
        {'U',"..-"}, {'V',"...-"},{'W',".--"}, {'X',"-..-"},{'Y',"-.--"},
        {'Z',"--.."},{'0',"-----"},{'1',".----"},{'2',"..---"},
        {'3',"...--"},{'4',"....-"},{'5',"....."},{'6',"-...."},
        {'7',"--..."},{'8',"---.."},{'9',".----."},
        {'-',"-....-"},{'_',"..--.-"}
    };
    std::string result;
    for (char c : text) {
        c = std::toupper(static_cast<unsigned char>(c));
        if (c == ' ') { result += "/ "; continue; }
        auto it = morse.find(c);
        if (it != morse.end()) result += it->second + " ";
        else result += "? ";
    }
    return result;
}

// ===================== NETWORK TOPOLOGY =====================
static NetworkGraph buildNetworkTopology() {
    NetworkGraph g;
    g.addEdge("HUB_MUMBAI_PRIME",    "HUB_DELHI_CTR",        90);
    g.addEdge("HUB_MUMBAI_PRIME",    "HUB_LONDON_VAUXHALL",   8);
    g.addEdge("HUB_MUMBAI_PRIME",    "NODE_BERLIN_TANGO",    15);
    g.addEdge("HUB_DELHI_CTR",       "NODE_OTTAWA_BRAVO",    20);
    g.addEdge("HUB_DELHI_CTR",       "GHOST_NODE_VESPER",    35);
    g.addEdge("NODE_BERLIN_TANGO",   "OUTPOST_KARACHI_4",    55);
    g.addEdge("OUTPOST_KARACHI_4",   "HUB_TELAVIV_9",        22);
    g.addEdge("HUB_TELAVIV_9",       "OUTPOST_NAIROBI_2",    40);
    g.addEdge("NODE_SINGAPORE_7",    "NODE_SEOUL_VECTOR",    18);
    g.addEdge("NODE_SEOUL_VECTOR",   "NODE_SYDNEY_K9",       60);
    g.addEdge("NODE_SINGAPORE_7",    "OUTPOST_KARACHI_4",    45);
    g.addEdge("SAT_LINK_CHENNAI",    "SAT_LINK_KOLKATA",      5);
    g.addEdge("SAT_LINK_CHENNAI",    "HUB_MUMBAI_PRIME",     12);
    g.addEdge("SAT_LINK_KOLKATA",    "NODE_SINGAPORE_7",     30);
    g.addEdge("RELAY_JAIPUR",        "STATION_PUNE_9",       10);
    g.addEdge("RELAY_HYDERABAD",     "STATION_AHMEDABAD_4",  14);
    g.addEdge("STATION_PUNE_9",      "STATION_NAGPUR_2",     25);
    g.addEdge("STATION_NAGPUR_2",    "STATION_BHOPAL_7",     19);
    g.addEdge("ECHO_RELAY_MOSCOW",   "ECHO_RELAY_BEIJING",    9);
    g.addEdge("ECHO_RELAY_MOSCOW",   "RELAY_JAIPUR",         28);
    g.addEdge("ECHO_RELAY_BEIJING",  "RELAY_HYDERABAD",      33);
    g.addEdge("GHOST_NODE_VESPER",   "GHOST_NODE_CIPHER",    50);
    g.addEdge("GHOST_NODE_CIPHER",   "OUTPOST_REYKJAVIK",    70);
    g.addEdge("OUTPOST_REYKJAVIK",   "HUB_MUMBAI_PRIME",     24);
    g.addEdge("NODE_OTTAWA_BRAVO",   "STATION_BHOPAL_7",     65);
    g.addEdge("HUB_LONDON_VAUXHALL", "ECHO_RELAY_MOSCOW",    17);
    g.addEdge("NODE_SYDNEY_K9",      "OUTPOST_NAIROBI_2",    80);
    return g;
}

// ===================== INPUT HELPERS =====================
static bool readLine(std::string& out) {
    if (!std::getline(std::cin, out)) return false;
    return true;
}

static bool safeParseLong(const std::string& s, long& out, TerminalDisplay& disp) {
    try {
        size_t pos;
        out = std::stol(s, &pos);
        if (pos != s.size()) throw std::invalid_argument("trailing chars");
        return true;
    } catch (...) {
        disp.pushLog(TerminalDisplay::colorize(
            "[INPUT REJECTED] TIMESTAMP FORMAT INVALID. EXPECTED EPOCH INTEGER, RECEIVED: '" + s + "'",
            Color::RED));
        return false;
    }
}

// ===================== MAIN =====================
int main() {
    TerminalDisplay display;
    MessageQueue msgQueue(10);
    NetworkGraph network = buildNetworkTopology();
    HistoricalDatabase historyDB;

    int skipped = 0;
    std::vector<Message> allMessages;
    try {
        allMessages = JsonLoader::loadMessages("data/intel_messages.json", skipped);
        historyDB.loadRecords(allMessages);
    } catch (const std::exception& e) {
        display.pushLog(TerminalDisplay::colorize(
            std::string("[SYSTEM WARNING] Failed to load archive: ") + e.what(), Color::RED));
    }

    if (skipped > 0) {
        display.pushLog(TerminalDisplay::colorize(
            "[SYSTEM WARNING] Skipped " + std::to_string(skipped) + " malformed records.",
            Color::YELLOW));
    }

    display.pushLog(TerminalDisplay::colorize(
        "CLASSIFIED ARCHIVE ONLINE // " + std::to_string(historyDB.size()) + " ENCRYPTED RECORDS DECRYPTED.",
        Color::GREEN));
    display.pushLog("ALL STATIONS NOMINAL. GLOBAL COMM GRID ACTIVE. AWAITING OPERATOR CLEARANCE.");

    bool running = true;
    while (running) {

        // Build queue panel
        std::vector<std::string> queueLines;
        auto snap = msgQueue.snapshot();
        queueLines.push_back("LIVE INTERCEPTS: " + std::to_string(msgQueue.size()) +
                              " / " + std::to_string(msgQueue.getCapacity()) + " SLOTS OCCUPIED");
        for (size_t i = 0; i < snap.size() && i < 5; i++) {
            std::string color = (snap[i].priority >= 4) ? Color::RED : Color::YELLOW;
            queueLines.push_back(TerminalDisplay::colorize(
                "#" + std::to_string(i+1) + " " + snap[i].toString(), color));
        }
        if (snap.size() > 5) {
            queueLines.push_back(TerminalDisplay::colorize(
                "... +" + std::to_string(snap.size() - 5) + " more transmissions standing by.", Color::DIM));
        }

        std::string menu =
            "  OPERATOR COMMAND INTERFACE // CLEARANCE LEVEL: EYES ONLY\n"
            "  -------------------------------------------------------\n"
            "  [1]  INTERCEPT  -- Compose & queue a new transmission\n"
            "  [2]  DISPATCH   -- Decrypt and forward next message\n"
            "  [3]  PATHFIND   -- Compute optimal secure route (Dijkstra)\n"
            "  [4]  BLACKOUT   -- Sever a communication link\n"
            "  [5]  RESTORE    -- Reestablish severed channel\n"
            "  [6]  GHOSTPATH  -- DFS emergency backup routing\n" 
            "  [7]  ARCHIVES   -- Retrieve classified record by timestamp\n"
            "  [8]  GRIDMAP    -- Display live network topology\n"
            "  [9]  ROLLBACK   -- Undo last configuration\n"
            "  [0]  DISCONNECT -- Terminate session\n"
            "  >> ";

        display.renderFrame("// IICN CLASSIFIED OPS TERMINAL // SECTOR 7 // EYES ONLY //",
                             queueLines,
                             std::vector<std::string>(display.getLogs().begin(), display.getLogs().end()),
                             menu);

        std::string choice;
        if (!readLine(choice)) break;

        // ---- OPTION 1: COMPOSE CUSTOM MESSAGE ----
        if (choice == "1") {
            static int customMsgCounter = 1;
            std::string origin, dest, payload, priorityInput;

            display.renderFrame("// COMPOSE TRANSMISSION //", queueLines,
                std::vector<std::string>(display.getLogs().begin(), display.getLogs().end()),
                "ORIGIN STATION >> ");
            readLine(origin);

            display.renderFrame("// COMPOSE TRANSMISSION //", queueLines,
                std::vector<std::string>(display.getLogs().begin(), display.getLogs().end()),
                "TARGET STATION >> ");
            readLine(dest);

            display.renderFrame("// COMPOSE TRANSMISSION //", queueLines,
                std::vector<std::string>(display.getLogs().begin(), display.getLogs().end()),
                "YOUR MESSAGE   >> ");
            readLine(payload);

            display.renderFrame("// COMPOSE TRANSMISSION //", queueLines,
                std::vector<std::string>(display.getLogs().begin(), display.getLogs().end()),
                "PRIORITY (1-5) >> ");
            readLine(priorityInput);

            int priority = 1;
            try {
                priority = std::stoi(priorityInput);
                if (priority < 1 || priority > 5) throw std::invalid_argument("out of range");
            } catch (...) {
                display.pushLog(TerminalDisplay::colorize(
                    "[INPUT REJECTED] Invalid priority. Defaulting to 1 (ROUTINE).", Color::YELLOW));
                priority = 1;
            }

            if (!network.hasNode(origin) || !network.hasNode(dest)) {
                display.pushLog(TerminalDisplay::colorize(
                    "[UNRECOGNIZED] Station not found in grid. Transmission aborted.", Color::RED));
            } else if (origin == dest) {
                display.pushLog(TerminalDisplay::colorize(
                    "[INVALID ROUTE] Origin and destination cannot be the same station.", Color::RED));
            } else if (payload.empty()) {
                display.pushLog(TerminalDisplay::colorize(
                    "[INPUT REJECTED] Empty message. Transmission aborted.", Color::RED));
            } else {
                Message msg;
                msg.msgId       = "MSG-" + std::string(5 - std::to_string(customMsgCounter).size(), '0')
                                  + std::to_string(customMsgCounter);
                msg.origin      = origin;
                msg.destination = dest;
                msg.payload     = payload;
                msg.timestamp   = (long)time(nullptr);
                msg.priority    = priority;

                bool ok = msgQueue.enqueue(msg);
                if (ok) {
                    display.pushLog(TerminalDisplay::colorize(
                        "[INTERCEPTED] " + msg.msgId + " // " + origin + " --> " + dest, Color::GREEN));
                    display.pushLog(TerminalDisplay::colorize(
                        "[QUEUED] " + payload, Color::YELLOW));
                    customMsgCounter++;
                } else {
                    display.pushLog(TerminalDisplay::colorize(
                        "[BUFFER CRITICAL] INTERCEPT BUFFER FULL. DISPATCH A MESSAGE BEFORE PROCEEDING.", Color::RED));
                }
            }
        }

        // ---- OPTION 2: DISPATCH MESSAGE ----
        else if (choice == "2") {
            Message m;
            if (msgQueue.dequeue(m)) {
                display.pushLog(TerminalDisplay::colorize(
                    "[DISPATCHED] TRANSMISSION CLEARED // " + m.toString(), Color::CYAN));
                display.pushLog(TerminalDisplay::colorize(
                    "[MORSE ENCODED] " + toMorse(m.msgId), Color::MAGENTA));
            } else {
                display.pushLog(TerminalDisplay::colorize(
                    "[BUFFER EMPTY] NO ACTIVE TRANSMISSIONS IN QUEUE. INTERCEPT FIRST.", Color::RED));
            }
        }

        // ---- OPTION 3: PATHFIND (DIJKSTRA) ----
        else if (choice == "3") {
            display.renderFrame("PATHFINDING // DIJKSTRA OPTIMAL ROUTE PROTOCOL", queueLines,
                std::vector<std::string>(display.getLogs().begin(), display.getLogs().end()),
                "ORIGIN STATION DESIGNATOR >> ");
            std::string src, dest;
            readLine(src);

            display.renderFrame("PATHFINDING // DIJKSTRA OPTIMAL ROUTE PROTOCOL", queueLines,
                std::vector<std::string>(display.getLogs().begin(), display.getLogs().end()),
                "TARGET STATION DESIGNATOR >> ");
            readLine(dest);

            if (!network.hasNode(src) || !network.hasNode(dest)) {
                display.pushLog(TerminalDisplay::colorize(
                    "[UNRECOGNIZED] STATION DESIGNATOR NOT FOUND IN GRID: '" + src + "' / '" + dest + "'",
                    Color::RED));
            } else {
                int cost;
                auto path = network.findShortestPath(src, dest, cost);
                if (path.empty()) {
                    display.pushLog(TerminalDisplay::colorize(
                        "[ROUTE FAILURE] ALL CHANNELS BETWEEN " + src + " AND " + dest + " ARE COMPROMISED.",
                        Color::RED));
                } else {
                    std::ostringstream oss;
                    for (size_t i = 0; i < path.size(); i++) {
                        oss << path[i];
                        if (i + 1 < path.size()) oss << " -> ";
                    }
                    display.pushLog(TerminalDisplay::colorize(
                        "[SECURE ROUTE ESTABLISHED] LATENCY: " + std::to_string(cost) + "ms // PATH: " + oss.str(),
                        Color::GREEN));
                }
            }
        }

        // ---- OPTION 4: BLACKOUT (SEVER LINK) ----
        else if (choice == "4") {
            display.renderFrame("BLACKOUT PROTOCOL // LINK TERMINATION", queueLines,
                std::vector<std::string>(display.getLogs().begin(), display.getLogs().end()),
                "STATION ALPHA DESIGNATOR >> ");
            std::string a, b;
            readLine(a);

            display.renderFrame("BLACKOUT PROTOCOL // LINK TERMINATION", queueLines,
                std::vector<std::string>(display.getLogs().begin(), display.getLogs().end()),
                "STATION BRAVO DESIGNATOR >> ");
            readLine(b);

            if (network.severLink(a, b)) {
                display.pushLog(TerminalDisplay::colorize(
                    "[BLACKOUT CONFIRMED] CHANNEL DESTROYED // " + a + " <=X=> " + b + " // LINK IS DEAD",
                    Color::RED));
            } else {
                display.pushLog(TerminalDisplay::colorize(
                    "[INVALID TARGET] NO DIRECT CHANNEL EXISTS BETWEEN '" + a + "' AND '" + b + "'",
                    Color::RED));
            }
        }

        // ---- OPTION 5: RESTORE LINK ----
        else if (choice == "5") {
            display.renderFrame("CHANNEL REACTIVATION // LINK RESTORATION PROTOCOL", queueLines,
                std::vector<std::string>(display.getLogs().begin(), display.getLogs().end()),
                "STATION ALPHA DESIGNATOR >> ");
            std::string a, b;
            readLine(a);

            display.renderFrame("CHANNEL REACTIVATION // LINK RESTORATION PROTOCOL", queueLines,
                std::vector<std::string>(display.getLogs().begin(), display.getLogs().end()),
                "STATION BRAVO DESIGNATOR >> ");
            readLine(b);

            if (network.restoreLink(a, b)) {
                display.pushLog(TerminalDisplay::colorize(
                    "[CHANNEL LIVE] SECURE LINK REESTABLISHED // " + a + " <===> " + b,
                    Color::GREEN));
            } else {
                display.pushLog(TerminalDisplay::colorize(
                    "[INVALID TARGET] NO DORMANT CHANNEL FOUND BETWEEN '" + a + "' AND '" + b + "'",
                    Color::RED));
            }
        }

        // ---- OPTION 6: GHOSTPATH (DFS) ----
        else if (choice == "6") {
            display.renderFrame("GHOSTPATH // EMERGENCY REROUTING PROTOCOL", queueLines,
                std::vector<std::string>(display.getLogs().begin(), display.getLogs().end()),
                "ORIGIN STATION DESIGNATOR >> ");
            std::string src, dest;
            readLine(src);

            display.renderFrame("GHOSTPATH // EMERGENCY REROUTING PROTOCOL", queueLines,
                std::vector<std::string>(display.getLogs().begin(), display.getLogs().end()),
                "TARGET STATION DESIGNATOR >> ");
            readLine(dest);

            if (!network.hasNode(src) || !network.hasNode(dest)) {
                display.pushLog(TerminalDisplay::colorize(
                    "[UNRECOGNIZED] STATION DESIGNATOR NOT FOUND IN GRID: '" + src + "' / '" + dest + "'",
                    Color::RED));
            } else {
                std::vector<std::string> order;
                auto path = network.dfsBackupChannels(src, dest, order);

                std::ostringstream oss;
                for (size_t i = 0; i < order.size(); i++) {
                    oss << order[i];
                    if (i + 1 < order.size()) oss << " -> ";
                }
                display.pushLog(TerminalDisplay::colorize(
                    "[SCANNING GRID] PROBING CHANNELS: " + oss.str(), Color::YELLOW));

                if (path.empty()) {
                    display.pushLog(TerminalDisplay::colorize(
                        "[NETWORK DARK] ALL BACKUP CHANNELS EXHAUSTED. STATION IS ISOLATED.",
                        Color::RED));
                } else {
                    std::ostringstream pathStr;
                    for (size_t i = 0; i < path.size(); i++) {
                        pathStr << path[i];
                        if (i + 1 < path.size()) pathStr << " -> ";
                    }
                    display.pushLog(TerminalDisplay::colorize(
                        "[GHOSTPATH SECURED] COVERT ROUTE ONLINE // " + pathStr.str(),
                        Color::GREEN));
                }
            }
        }

        // ---- OPTION 7: ARCHIVES (BINARY SEARCH) ----
        else if (choice == "7") {
    display.renderFrame("ARCHIVES // CLASSIFIED RECORD RETRIEVAL", queueLines,
        std::vector<std::string>(display.getLogs().begin(), display.getLogs().end()),
        "ENTER TRANSMISSION TIMESTAMP (EPOCH) >> ");
    std::string tsInput;
    readLine(tsInput);

    long ts;
    if (safeParseLong(tsInput, ts, display)) {
        Message result;
        std::vector<std::string> steps;

        display.pushLog(TerminalDisplay::colorize(
            "[BINARY SEARCH INITIATED] Scanning " +
            std::to_string(historyDB.size()) + " classified records...",
            Color::CYAN));

        if (historyDB.searchByTimestamp(ts, result, steps)) {
            for (const auto& s : steps) {
                display.pushLog(TerminalDisplay::colorize(s, Color::YELLOW));
            }
            display.pushLog(TerminalDisplay::colorize(
                "[RECORD DECLASSIFIED] Found in " +
                std::to_string(steps.size()) + " comparisons out of " +
                std::to_string(historyDB.size()) + " records // " +
                result.toString(), Color::GREEN));
        } else {
            for (const auto& s : steps) {
                display.pushLog(TerminalDisplay::colorize(s, Color::YELLOW));
            }
            int idx = historyDB.findClosest(ts);
            if (idx >= 0) {
                const auto& rec = historyDB.getRecords()[idx];
                display.pushLog(TerminalDisplay::colorize(
                    "[PARTIAL MATCH] No exact record found after " +
                    std::to_string(steps.size()) + " comparisons. Nearest intercept // " +
                    rec.toString(), Color::YELLOW));
            } else {
                display.pushLog(TerminalDisplay::colorize(
                    "[ARCHIVE OFFLINE] CLASSIFIED DATABASE RETURNED NO RECORDS.",
                    Color::RED));
            }
        }
    }
}

        // ---- OPTION 8: GRIDMAP ----
        else if (choice == "8") {
            display.clearLogs();
            std::istringstream table(network.renderASCIITable());
            display.pushLog(">>> GLOBAL COMM GRID // LIVE TOPOLOGY SNAPSHOT <<<");
            std::string line;
            while (std::getline(table, line)) {
                std::string colored = line;
                if (line.find("SEVERED") != std::string::npos) {
                    colored = TerminalDisplay::colorize(line, Color::RED);
                } else if (line.find("ACTIVE") != std::string::npos) {
                    colored = TerminalDisplay::colorize(line, Color::GREEN);
                }
                display.pushLog(colored);
            }
        }

        else if (choice == "9") {
            if(network.rollbackLastAction()) {
                display.pushLog(
                    TerminalDisplay::colorize(
                        "[ROLLBACK COMPLETE] MOST RECENT CONFIGURATION RESTORED.",
                        Color::GREEN
                    )
                );
            } else {
                display.pushLog(
                    TerminalDisplay::colorize(
                        "[ROLLBACK FAILED] STACK EMPTY.",
                        Color::YELLOW
                    )
                );
            }
        }

        // ---- OPTION 0: EXIT ----
        else if (choice == "0") {
            running = false;
        }

        // ---- UNKNOWN INPUT ----
        else {
            display.pushLog(TerminalDisplay::colorize(
                "[SYSTEM WARNING] Unrecognized command: '" + choice + "'", Color::RED));
        }
    }

    display.clearScreen();
    std::cout << Color::CYAN
              << "SESSION TERMINATED. CHANNEL ENCRYPTED. GOODBYE, OPERATOR.\n"
              << Color::RESET;
    return 0;
}
