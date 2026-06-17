#include "HistoricalDatabase.h"
#include <algorithm>
#include <cmath>

void HistoricalDatabase::loadRecords(const std::vector<Message>& recs) {
    records = recs;
    std::sort(records.begin(), records.end(),
              [](const Message& a, const Message& b) {
                  return a.timestamp < b.timestamp;
              });
}

bool HistoricalDatabase::searchByTimestamp(long timestamp, Message& result, std::vector<std::string>& steps) const {
    int lo = 0, hi = static_cast<int>(records.size()) - 1;
    int stepNum = 1;

    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        long midTS = records[mid].timestamp;

        std::ostringstream oss;
        oss << "  Step " << stepNum << " | Index " << mid
            << " | TS: " << midTS;

        if (midTS == timestamp) {
            oss << " | >>> MATCH FOUND <<<";
            steps.push_back(oss.str());
            result = records[mid];
            return true;
        } else if (midTS < timestamp) {
            oss << " | Too LOW  --> go RIGHT";
            steps.push_back(oss.str());
            lo = mid + 1;
        } else {
            oss << " | Too HIGH --> go LEFT";
            steps.push_back(oss.str());
            hi = mid - 1;
        }
        stepNum++;
    }
    return false;
}

int HistoricalDatabase::findClosest(long timestamp) const {
    if (records.empty()) return -1;
    int lo = 0, hi = static_cast<int>(records.size()) - 1;
    if (timestamp <= records[lo].timestamp) return lo;
    if (timestamp >= records[hi].timestamp) return hi;

    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (records[mid].timestamp == timestamp) return mid;
        else if (records[mid].timestamp < timestamp) lo = mid + 1;
        else hi = mid - 1;
    }
    // lo and hi have crossed; pick whichever is closer
    if (lo >= static_cast<int>(records.size())) return hi;
    if (hi < 0) return lo;
    long diffLo = std::abs(records[lo].timestamp - timestamp);
    long diffHi = std::abs(records[hi].timestamp - timestamp);
    return (diffLo < diffHi) ? lo : hi;
}

size_t HistoricalDatabase::size() const {
    return records.size();
}

const std::vector<Message>& HistoricalDatabase::getRecords() const {
    return records;
}
