#include "JsonLoader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cctype>

namespace {

    // Skip whitespace
    void skipWS(const std::string& s, size_t& i) {
        while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) i++;
    }

    // Parses a JSON string literal starting at s[i] == '"'
    std::string parseString(const std::string& s, size_t& i) {
        std::string out;
        if (s[i] != '"') throw std::runtime_error("Expected string");
        i++; // skip opening quote
        while (i < s.size() && s[i] != '"') {
            if (s[i] == '\\' && i + 1 < s.size()) {
                i++; // skip escape char
                switch (s[i]) {
                    case 'n': out += '\n'; break;
                    case 't': out += '\t'; break;
                    case '"': out += '"'; break;
                    case '\\': out += '\\'; break;
                    default: out += s[i]; break;
                }
            } else {
                out += s[i];
            }
            i++;
        }
        i++; // skip closing quote
        return out;
    }

    // Parses a JSON number starting at s[i]
    long parseNumber(const std::string& s, size_t& i) {
        size_t start = i;
        if (s[i] == '-') i++;
        while (i < s.size() && (std::isdigit(static_cast<unsigned char>(s[i])) || s[i] == '.')) i++;
        return std::stol(s.substr(start, i - start));
    }

    // Parses one flat JSON object: { "key": "value", "key2": 123, ... }
    // Returns field map.
    bool parseObject(const std::string& s, size_t& i, Message& msg) {
        skipWS(s, i);
        if (i >= s.size() || s[i] != '{') return false;
        i++; // skip {

        while (true) {
            skipWS(s, i);
            if (i >= s.size()) return false;
            if (s[i] == '}') { i++; break; }
            if (s[i] == ',') { i++; continue; }

            // key
            std::string key = parseString(s, i);
            skipWS(s, i);
            if (s[i] != ':') return false;
            i++; // skip :
            skipWS(s, i);

            // value
            if (s[i] == '"') {
                std::string val = parseString(s, i);
                if (key == "msgId") msg.msgId = val;
                else if (key == "origin") msg.origin = val;
                else if (key == "destination") msg.destination = val;
                else if (key == "payload") msg.payload = val;
            } else if (std::isdigit(static_cast<unsigned char>(s[i])) || s[i] == '-') {
                long val = parseNumber(s, i);
                if (key == "timestamp") msg.timestamp = val;
                else if (key == "priority") msg.priority = static_cast<int>(val);
            } else {
                // Unknown/unsupported value type (null, bool, nested) -- skip token
                while (i < s.size() && s[i] != ',' && s[i] != '}') i++;
            }
        }
        return true;
    }
}

std::vector<Message> JsonLoader::loadMessages(const std::string& filepath, int& skippedRecords) {
    skippedRecords = 0;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filepath);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    std::vector<Message> messages;
    size_t i = 0;
    skipWS(content, i);
    if (i >= content.size() || content[i] != '[') {
        throw std::runtime_error("Expected JSON array at top level");
    }
    i++; // skip [

    while (true) {
        skipWS(content, i);
        if (i >= content.size()) break;
        if (content[i] == ']') { i++; break; }
        if (content[i] == ',') { i++; continue; }

        Message msg;
        msg.timestamp = 0;
        msg.priority = 1;
        size_t before = i;
        try {
            if (!parseObject(content, i, msg)) {
                skippedRecords++;
                // attempt recovery: advance past this token
                if (i == before) i++;
                continue;
            }
            // basic validation -- required fields present
            if (msg.msgId.empty() || msg.origin.empty() || msg.destination.empty()) {
                skippedRecords++;
                continue;
            }
            messages.push_back(msg);
        } catch (...) {
            skippedRecords++;
            if (i == before) i++; // avoid infinite loop on bad token
        }
    }

    return messages;
}
