#ifndef TERMINAL_DISPLAY_H
#define TERMINAL_DISPLAY_H

#include <string>
#include <vector>
#include <deque>

// ===================== ANSI COLOR CODES =====================
namespace Color {
    const std::string RESET   = "\033[0m";
    const std::string RED     = "\033[1;31m";
    const std::string GREEN   = "\033[1;32m";
    const std::string YELLOW  = "\033[1;33m";
    const std::string BLUE    = "\033[1;34m";
    const std::string MAGENTA = "\033[1;35m";
    const std::string CYAN    = "\033[1;36m";
    const std::string WHITE   = "\033[1;37m";
    const std::string BOLD    = "\033[1m";
    const std::string DIM     = "\033[2m";
    const std::string BG_RED  = "\033[41m";
    const std::string BG_GREEN= "\033[42m";
}

// ===================== TERMINAL DISPLAY ENGINE =====================
// Handles screen clearing, cursor positioning, and structured
// "viewport" style redraw of the TUI (Queue Status / Logs / Input)
class TerminalDisplay {
public:
    TerminalDisplay();

    void clearScreen() const;
    void moveCursor(int row, int col) const;
    void hideCursor() const;
    void showCursor() const;
    void clearLogs() { actionLog.clear(); }

    // Draws a horizontal divider line of given width
    void drawDivider(int width, char ch = '-') const;

    // Renders the full TUI frame: header, queue panel, graph/log panel, input bar
    void renderFrame(const std::string& header,
                      const std::vector<std::string>& queueLines,
                      const std::vector<std::string>& logLines,
                      const std::string& inputPrompt) const;

    // Pushes a log line into the rolling action log buffer
    void pushLog(const std::string& line);

    const std::deque<std::string>& getLogs() const { return actionLog; }

    static std::string colorize(const std::string& text, const std::string& colorCode);

private:
    std::deque<std::string> actionLog;
    static const size_t MAX_LOG_LINES = 15;
};

#endif // TERMINAL_DISPLAY_H
