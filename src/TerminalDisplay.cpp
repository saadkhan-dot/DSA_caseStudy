#include "TerminalDisplay.h"
#include <iostream>
#include <iomanip>

TerminalDisplay::TerminalDisplay() {}

void TerminalDisplay::clearScreen() const {
    // ANSI escape: clear entire screen + move cursor to home (1,1)
    std::cout << "\033[2J\033[H";
}

void TerminalDisplay::moveCursor(int row, int col) const {
    std::cout << "\033[" << row << ";" << col << "H";
}

void TerminalDisplay::hideCursor() const {
    std::cout << "\033[?25l";
}

void TerminalDisplay::showCursor() const {
    std::cout << "\033[?25h";
}

void TerminalDisplay::drawDivider(int width, char ch) const {
    std::cout << Color::DIM << std::string(width, ch) << Color::RESET << "\n";
}

std::string TerminalDisplay::colorize(const std::string& text, const std::string& colorCode) {
    return colorCode + text + Color::RESET;
}

void TerminalDisplay::pushLog(const std::string& line) {
    actionLog.push_back(line);
    while (actionLog.size() > MAX_LOG_LINES) {
        actionLog.pop_front();
    }
}

void TerminalDisplay::renderFrame(const std::string& header,
                                   const std::vector<std::string>& queueLines,
                                   const std::vector<std::string>& logLines,
                                   const std::string& inputPrompt) const {
    clearScreen();

    const int width = 78;

    // ---- HEADER ----
    std::cout << Color::BOLD << Color::CYAN
              << "================= " << header << " =================" 
              << Color::RESET << "\n\n";

    // ---- QUEUE STATUS PANEL (pinned top) ----
    std::cout << Color::BOLD << Color::YELLOW << "[ QUEUE STATUS ]" << Color::RESET << "\n";
    drawDivider(width);
    if (queueLines.empty()) {
        std::cout << Color::DIM << "  (queue empty)" << Color::RESET << "\n";
    } else {
        for (const auto& q : queueLines) std::cout << "  " << q << "\n";
    }
    drawDivider(width);
    std::cout << "\n";

    // ---- ACTION LOG / ROUTE MAP PANEL (middle) ----
    std::cout << Color::BOLD << Color::BLUE << "[ ACTION LOG / ROUTE MAP ]" << Color::RESET << "\n";
    drawDivider(width);
    for (const auto& l : logLines) {
        std::cout << "  " << l << "\n";
    }
    drawDivider(width);
    std::cout << "\n";

    // ---- INPUT PROMPT (pinned bottom) ----
    std::cout << Color::BOLD << Color::GREEN << inputPrompt << Color::RESET;
    std::cout.flush();
}
