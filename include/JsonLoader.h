#ifndef JSON_LOADER_H
#define JSON_LOADER_H

#include <string>
#include <vector>
#include "MessageQueue.h"

// ===================== JSON LOADER =====================
// Minimal, dependency-free parser for our specific data format:
// an array of flat JSON objects with string/number fields.
// NOT a general-purpose JSON parser -- intentionally narrow in scope.
namespace JsonLoader {

    // Loads messages from a JSON file at the given path.
    // Throws std::runtime_error on file I/O failure.
    // Malformed individual records are skipped (with a warning count).
    std::vector<Message> loadMessages(const std::string& filepath, int& skippedRecords);

}

#endif // JSON_LOADER_H
