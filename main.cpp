#include <iostream>
#include <string>
#include <ctime>
#include <fstream>
#include <sys/stat.h>

// DevLog — Developer Journal CLI
// Day 3: write entry to logs/ as a dated .txt file

struct Entry {
    std::string date;
    std::string worked_on;
    std::string learned;
    std::string blocked;
    std::string tags;
    int mood;
};

std::string getToday() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buf[20];
    strftime(buf, sizeof(buf), "%d-%m-%y", ltm);
    return std::string(buf);
}

// same as getToday() but YYYY-MM-DD so files sort correctly
std::string getTodayForFilename() {
    time_t now = time(0); 
    tm* ltm = localtime(&now);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d", ltm);
    return std::string(buf);
}

std::string ask(const std::string& question) {
    std::cout << "  \033[34m" << question << "\033[0m\n";
    std::cout << "  › ";
    std::string answer;
    std::getline(std::cin, answer);
    std::cout << "\n";
    return answer;
}

void printBanner() {
    std::cout << R"(
  +----------------------------------+
  |                                  |
  |   >> DEVLOG                      |
  |   // developer journal           |
  |   ## v0.1 | FOSS | CLI           |
  |                                  |
  +----------------------------------+

  type './devlog help' to get started

)";
}

void printHelp() {
    std::cout << "  Usage: ./devlog <command>\n\n";
    std::cout << "  Commands:\n";
    std::cout << "    new      ->  Log today's dev session\n";
    std::cout << "    list     ->  View all past entries\n";
    std::cout << "    search   ->  Search entries by keyword\n";
    std::cout << "    report   ->  Generate HTML report\n";
    std::cout << "    week     ->  Show this week's summary\n";
    std::cout << "    edit     ->  Edit the last entry\n";
    std::cout << "    help     ->  Show this help message\n";
    std::cout << "\n";
    std::cout << "  Example: ./devlog new\n\n";
}

void createLogsFolder() {
    mkdir("logs", 0777);
}

void newEntry() {
    auto today = getToday();
    auto todayFilename = getTodayForFilename();

    std::cout << "\n  \033[33m--- DEV LOG | " << today << " ---\033[0m\n\n";

    Entry e;
    e.date = today;
    e.worked_on = ask("What did you work on today?");
    e.learned = ask("What did you learn?");
    e.blocked = ask("What blocked you? (type 'nothing' if nothing)");
    e.tags = ask("Tags — space separated (e.g. cpp networking debug):");

    std::cout << "  \033[34mMood? [1-5]:\033[0m\n";
    std::cout << "  › ";
    std::string moodStr;
    std::getline(std::cin, moodStr);
    std::cout << "\n";

    // clamp to 1-5
    e.mood = 3; // default
    if (!moodStr.empty()) {
        try {
            int m = std::stoi(moodStr);
            if (m >= 1 && m <= 5) e.mood = m;
        } catch (...) {
        }
    }

    // show preview
    std::cout << "  ──────────────────────────────────────────\n";
    std::cout << "  \033[33mEntry Preview — " << today << "\033[0m\n";
    std::cout << "  ──────────────────────────────────────────\n";
    std::cout << "  Worked on: " << e.worked_on << "\n";
    std::cout << "  Learned: " << e.learned << "\n";
    std::cout << "  Blocked: " << e.blocked << "\n";
    std::cout << "  Tags: " << e.tags << "\n";
    std::cout << "  Mood: " << e.mood << "/5\n";
    std::cout << "  ──────────────────────────────────────────\n\n";

    // save to file
    createLogsFolder();
    std::string filename = "logs/" + todayFilename + ".txt";
    std::ofstream file(filename);

    if (file.is_open()) {
        file << "Date: " << e.date << "\n";
        file << "Worked on: " << e.worked_on << "\n";
        file << "Learned: " << e.learned << "\n";
        file << "Blocked: " << e.blocked << "\n";
        file << "Tags: " << e.tags << "\n";
        file << "Mood: " << e.mood << "/5\n";
        std::cout << "  \033[32m✓ Entry saved to " << filename << "\033[0m\n\n";
    } else {
        std::cout << "  \033[31m✗ Could not save entry.\033[0m\n\n";
    }
}

int main(int argc, char* argv[]) {

    printBanner();

    if (argc < 2) {
        printHelp();
        return 0;
    }

    std::string command = argv[1];

    if (command == "help") {
        printHelp();
    }
    else if (command == "new") {
        newEntry();
    }
    else if (command == "list") {
        std::cout << "  [list] Coming on Day 7!\n\n";
    }
    else if (command == "report") {
        std::cout << "  [report] Coming on Day 15!\n\n";
    }
    else if (command == "search") {
        std::cout << "  [search] Coming on Day 11!\n\n";
    }
    else if (command == "week") {
        std::cout << "  [week] Coming on Day 21!\n\n";
    }
    else if (command == "edit") {
        std::cout << "  [edit] Coming on Day 20!\n\n";
    }
    else {
        std::cout << "  Unknown command: \"" << command << "\"\n";
        std::cout << "  Run './devlog help' to see available commands.\n\n";
    }

    return 0;
}