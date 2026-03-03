#include <iostream>
#include <string>
#include <ctime>
#include <fstream>
#include <sys/stat.h>

// DevLog — Developer Journal CLI
// Day 4: empty validation, entry count, mood stars, timestamp

struct Entry {
    std::string date;
    std::string time;
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

std::string getTodayForFilename() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d", ltm);
    return std::string(buf);
}

// NEW 1 — gets current time as HH:MM
std::string getTime() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buf[10];
    strftime(buf, sizeof(buf), "%H:%M", ltm);
    return std::string(buf);
}

// NEW 2 — counts total entries saved
int countEntries() {
    int count = 0;
    std::ifstream indexFile("logs/count.txt");
    if (indexFile.is_open()) {
        indexFile >> count;
        indexFile.close();
    }
    count++;
    std::ofstream outFile("logs/count.txt");
    outFile << count;
    outFile.close();
    return count;
}

// NEW 3 — converts mood number to stars
std::string moodToStars(int mood) {
    std::string stars = "";
    for (int i = 1; i <= 5; i++) {
        if (i <= mood) stars += "*";
        else           stars += "-";
    }
    return stars;
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
  |   ## v0.4 | FOSS | CLI           |
  |                                  |
  +----------------------------------+

  type './devlog help' to get started

)";
}

void printHelp() {
    std::cout << "  Usage: ./devlog <command>\n\n";
    std::cout << "  Commands:\n";
    std::cout << "    new      ->  Log today's dev session\n";
    std::cout << "    read     ->  Read entry by date (YYYY-MM-DD)\n";
    std::cout << "    list     ->  View all past entries\n";
    std::cout << "    search   ->  Search entries by keyword\n";
    std::cout << "    report   ->  Generate HTML report\n";
    std::cout << "    week     ->  Show this week's summary\n";
    std::cout << "    edit     ->  Edit the last entry\n";
    std::cout << "    help     ->  Show this help message\n";
    std::cout << "\n";
    std::cout << "  Examples:\n";
    std::cout << "    ./devlog new\n";
    std::cout << "    ./devlog read 2026-03-01\n\n";
}

void createLogsFolder() {
    mkdir("logs", 0777);
}

void readCommand(const std::string& date) {
    std::string filename = "logs/" + date + ".json";
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cout << "  \033[31mx No entry found for: " << date << "\033[0m\n";
        std::cout << "  Make sure the date format is YYYY-MM-DD (e.g. 2026-03-01)\n\n";
        return;
    }

    std::cout << "\n  \033[33m--- ENTRY | " << date << " ---\033[0m\n\n";
    std::string line;
    while (std::getline(file, line)) {
        std::cout << "  " << line << "\n";
    }
    file.close();
    std::cout << "\n";
}

void newEntry() {
    auto today         = getToday();
    auto todayFilename = getTodayForFilename();
    auto currentTime   = getTime();

    // warn if entry already exists for today
    std::string filename = "logs/" + todayFilename + ".json";
    std::ifstream checkFile(filename);
    if (checkFile.is_open()) {
        checkFile.close();
        std::cout << "  \033[33m! Entry already exists for today.\033[0m\n";
        std::cout << "  Overwrite it? (y/n): ";
        std::string answer;
        std::getline(std::cin, answer);
        std::cout << "\n";
        if (answer != "y" && answer != "Y") {
            std::cout << "  Entry kept unchanged.\n\n";
            return;
        }
    }

    std::cout << "\n  \033[33m--- DEV LOG | " << today << " | " << currentTime << " ---\033[0m\n\n";

    Entry e;
    e.date      = today;
    e.time      = currentTime;
    e.worked_on = ask("What did you work on today?");

    // NEW 4 — validate worked_on is not empty
    if (e.worked_on.empty()) {
        std::cout << "  \033[31m✗ 'Worked on' cannot be empty.\033[0m\n\n";
        return;
    }

    e.learned   = ask("What did you learn?");
    e.blocked   = ask("What blocked you? (type 'nothing' if nothing)");
    e.tags      = ask("Tags — space separated (e.g. cpp networking debug):");

    std::cout << "  \033[34mMood? [1-5]:\033[0m\n";
    std::cout << "  › ";
    std::string moodStr;
    std::getline(std::cin, moodStr);
    std::cout << "\n";

    e.mood = 3;
    if (!moodStr.empty()) {
        try {
            int m = std::stoi(moodStr);
            if (m >= 1 && m <= 5) e.mood = m;
        } catch (...) {}
    }

    std::cout << "  ──────────────────────────────────────────\n";
    std::cout << "  \033[33mEntry Preview — " << today << "\033[0m\n";
    std::cout << "  ──────────────────────────────────────────\n";
    std::cout << "  Worked on : " << e.worked_on << "\n";
    std::cout << "  Learned   : " << e.learned   << "\n";
    std::cout << "  Blocked   : " << e.blocked   << "\n";
    std::cout << "  Tags      : " << e.tags      << "\n";
    std::cout << "  Mood      : " << moodToStars(e.mood) << " (" << e.mood << "/5)\n";
    std::cout << "  ──────────────────────────────────────────\n\n";

    createLogsFolder();
    std::ofstream file(filename);

    if (file.is_open()) {
        file << "{\n";
        file << "  \"date\": \""      << e.date      << "\",\n";
        file << "  \"time\": \""      << e.time      << "\",\n";
        file << "  \"worked_on\": \"" << e.worked_on << "\",\n";
        file << "  \"learned\": \""   << e.learned   << "\",\n";
        file << "  \"blocked\": \""   << e.blocked   << "\",\n";
        file << "  \"tags\": \""      << e.tags      << "\",\n";
        file << "  \"mood\": "        << e.mood      << "\n";
        file << "}\n";
        file.close();
        int total = countEntries();
        std::cout << "  \033[32m✓ Entry saved to " << filename << "\033[0m\n";
        std::cout << "  \033[32m✓ Total entries : " << total   << "\033[0m\n\n";
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
    } else if (command == "new") {
        newEntry();
    } else if (command == "read") {
        if (argc < 3) {
            std::cout << "  \033[31mx Please provide a date.\033[0m\n";
            std::cout << "  Usage: ./devlog read 2026-03-01\n\n";
        } else {
            readCommand(argv[2]);
        }
    } else if (command == "list") {
        std::cout << "  [list] Coming on Day 7!\n\n";
    } else if (command == "report") {
        std::cout << "  [report] Coming on Day 15!\n\n";
    } else if (command == "search") {
        std::cout << "  [search] Coming on Day 11!\n\n";
    } else if (command == "week") {
        std::cout << "  [week] Coming on Day 21!\n\n";
    } else if (command == "edit") {
        std::cout << "  [edit] Coming on Day 20!\n\n";
    } else {
        std::cout << "  Unknown command: \"" << command << "\"\n";
        std::cout << "  Run './devlog help' to see available commands.\n\n";
    }

    return 0;
}
