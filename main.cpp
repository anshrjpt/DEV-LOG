#include <iostream>
#include <string>
#include <ctime>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <map>

#ifdef _WIN32
    #include <direct.h>
    #include <windows.h>
    #define MAKE_DIR(path) _mkdir(path)
#else
    #include <sys/stat.h>
    #include <dirent.h>
    #define MAKE_DIR(path) mkdir(path, 0777)
#endif

// DevLog -- Developer Journal CLI

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
    strftime(buf, sizeof(buf), "%Y-%m-%d", ltm);
    return std::string(buf);
}

std::string addDays(const std::string& date, int days) {
    struct tm t = {};
    int y, m, d;
    sscanf(date.c_str(), "%d-%d-%d", &y, &m, &d);
    t.tm_year = y - 1900;
    t.tm_mon  = m - 1;
    t.tm_mday = d + days;
    mktime(&t);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d", &t);
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
    std::cout << "  > ";
    std::string answer;
    std::getline(std::cin, answer);
    std::cout << "\n";
    return answer;
}

std::string toLower(const std::string& s) {
    std::string result = s;
    for (char& c : result) c = tolower(c);
    return result;
}

bool entryMatches(const Entry& e, const std::string& keyword) {
    std::string kw = toLower(keyword);
    if (toLower(e.worked_on).find(kw) != std::string::npos) return true;
    if (toLower(e.learned).find(kw)   != std::string::npos) return true;
    if (toLower(e.blocked).find(kw)   != std::string::npos) return true;
    if (toLower(e.tags).find(kw)      != std::string::npos) return true;
    if (toLower(e.date).find(kw)      != std::string::npos) return true;
    return false;
}

// NEW -- checks streak and mood, prints warnings after banner
void checkReminders() {
    // --- Streak reminder ---
    std::string yesterday = addDays(getToday(), -1);
    std::string yFile = "logs/" + yesterday + ".json";
    std::string todayFile = "logs/" + getToday() + ".json";

    std::ifstream yCheck(yFile);
    std::ifstream tCheck(todayFile);

    bool loggedYesterday = yCheck.is_open();
    bool loggedToday     = tCheck.is_open();
    yCheck.close();
    tCheck.close();

    if (!loggedToday && !loggedYesterday) {
        std::cout << "  \033[31m[!] You missed yesterday! Streak at risk.\033[0m\n";
        std::cout << "  Run './devlog new' to log today and keep going.\n\n";
    } else if (!loggedToday) {
        std::cout << "  \033[33m[!] You haven't logged today yet.\033[0m\n";
        std::cout << "  Run './devlog new' before the day ends!\n\n";
    }

    // --- Mood warning ---
    // read last 3 entries and check if avg is below 2.5
    std::vector<std::string> files;

#ifdef _WIN32
    WIN32_FIND_DATAA ffd;
    HANDLE hFind = FindFirstFileA("logs\\*.json", &ffd);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            files.push_back("logs/" + std::string(ffd.cFileName));
        } while (FindNextFileA(hFind, &ffd) != 0);
        FindClose(hFind);
    }
#else
    DIR* dir = opendir("logs");
    if (dir) {
        struct dirent* ent;
        while ((ent = readdir(dir)) != nullptr) {
            std::string name = ent->d_name;
            if (name.size() > 5 && name.substr(name.size() - 5) == ".json") {
                files.push_back("logs/" + name);
            }
        }
        closedir(dir);
    }
#endif

    if (files.size() >= 3) {
        std::sort(files.begin(), files.end());

        // take last 3 only
        int total = 0;
        int count = 0;
        for (int i = (int)files.size() - 3; i < (int)files.size(); i++) {
            std::ifstream f(files[i]);
            if (!f.is_open()) continue;
            std::string json((std::istreambuf_iterator<char>(f)),
                              std::istreambuf_iterator<char>());
            f.close();

            // parse mood manually
            std::string key = "\"mood\"";
            size_t kp = json.find(key);
            if (kp == std::string::npos) continue;
            size_t cp = json.find(":", kp);
            if (cp == std::string::npos) continue;
            size_t ns = json.find_first_not_of(" \n", cp + 1);
            if (ns == std::string::npos) continue;

            int mood = std::stoi(json.substr(ns));
            total += mood;
            count++;
        }

        if (count > 0) {
            double avg = (double)total / count;
            if (avg < 2.5) {
                std::cout << "  \033[31m[!] Mood alert: your last 3 entries avg " 
                          << std::fixed << std::setprecision(1) << avg << "/5\033[0m\n";
                std::cout << "  Take a break if you need one. You got this.\n\n";
            }
        }
    }
}

void printBanner() {
    std::cout << "\n";
    std::cout << "  +----------------------------------+\n";
    std::cout << "  |                                  |\n";
    std::cout << "  |   >> DEVLOG                      |\n";
    std::cout << "  |   // developer journal           |\n";
    std::cout << "  |   ## v0.9 | FOSS | CLI           |\n";
    std::cout << "  |                                  |\n";
    std::cout << "  +----------------------------------+\n";
    std::cout << "\n";
    std::cout << "  type './devlog help' to get started\n\n";
}

void printHelp() {
    std::cout << "  Usage: ./devlog <command>\n\n";
    std::cout << "  Commands:\n";
    std::cout << "    new      ->  Log today's dev session\n";
    std::cout << "    read     ->  Read entry by date (YYYY-MM-DD)\n";
    std::cout << "    list     ->  View all past entries\n";
    std::cout << "    search   ->  Search entries by keyword\n";
    std::cout << "    stats    ->  Show coding stats dashboard\n";
    std::cout << "    week     ->  Show this week's summary\n";
    std::cout << "    report   ->  Generate HTML report\n";
    std::cout << "    edit     ->  Edit the last entry\n";
    std::cout << "    help     ->  Show this help message\n";
    std::cout << "\n";
    std::cout << "  Examples:\n";
    std::cout << "    ./devlog new\n";
    std::cout << "    ./devlog read 2026-03-01\n";
    std::cout << "    ./devlog search cpp\n\n";
}

void createLogsFolder() {
    MAKE_DIR("logs");
}

std::string parseField(const std::string& json, const std::string& key) {
    std::string searchKey = "\"" + key + "\"";
    size_t keyPos = json.find(searchKey);
    if (keyPos == std::string::npos) return "";

    size_t colonPos = json.find(":", keyPos);
    if (colonPos == std::string::npos) return "";

    size_t quoteStart = json.find("\"", colonPos);
    if (quoteStart == std::string::npos) return "";

    size_t quoteEnd = json.find("\"", quoteStart + 1);
    if (quoteEnd == std::string::npos) return "";

    return json.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
}

int parseMood(const std::string& json) {
    std::string searchKey = "\"mood\"";
    size_t keyPos = json.find(searchKey);
    if (keyPos == std::string::npos) return 0;

    size_t colonPos = json.find(":", keyPos);
    if (colonPos == std::string::npos) return 0;

    size_t numStart = json.find_first_not_of(" \n", colonPos + 1);
    if (numStart == std::string::npos) return 0;

    return std::stoi(json.substr(numStart));
}

Entry readEntry(const std::string& filename) {
    Entry e;
    e.mood = 0;

    std::ifstream file(filename);
    if (!file.is_open()) return e;

    std::string json((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
    file.close();

    e.date      = parseField(json, "date");
    e.worked_on = parseField(json, "worked_on");
    e.learned   = parseField(json, "learned");
    e.blocked   = parseField(json, "blocked");
    e.tags      = parseField(json, "tags");
    e.mood      = parseMood(json);

    return e;
}

void readCommand(const std::string& date) {
    std::string filename = "logs/" + date + ".json";
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cout << "  \033[31m[x] No entry found for: " << date << "\033[0m\n";
        std::cout << "  Make sure the date format is YYYY-MM-DD (e.g. 2026-03-01)\n\n";
        return;
    }

    std::string json((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
    file.close();

    std::string displayDate = parseField(json, "date");
    std::string workedOn    = parseField(json, "worked_on");
    std::string learned     = parseField(json, "learned");
    std::string blocked     = parseField(json, "blocked");
    std::string tags        = parseField(json, "tags");
    int mood                = parseMood(json);

    std::cout << "\n  \033[33m--- ENTRY | " << date << " ---\033[0m\n\n";
    std::cout << "  Date      : " << displayDate << "\n";
    std::cout << "  Worked on : " << workedOn    << "\n";
    std::cout << "  Learned   : " << learned     << "\n";
    std::cout << "  Blocked   : " << blocked     << "\n";
    std::cout << "  Tags      : " << tags        << "\n";
    std::cout << "  Mood      : " << mood        << "/5\n";
    std::cout << "\n";
}

void newEntry() {
    auto today = getToday();
    auto currentTime = getTime();
    std::string filename = "logs/" + today + ".json";

    std::ifstream checkFile(filename);
    if (checkFile.is_open()) {
        checkFile.close();
        std::cout << "  \033[33m[!] Entry already exists for today.\033[0m\n";
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

    if (e.worked_on.empty()) {
        std::cout << "  \033[31m[x] worked_on cannot be empty. Entry cancelled.\033[0m\n\n";
        return;
    }

    e.learned = ask("What did you learn?");
    e.blocked = ask("What blocked you? (type 'nothing' if nothing)");
    e.tags    = ask("Tags -- space separated (e.g. cpp networking debug):");

    std::cout << "  \033[34mMood? [1-5]:\033[0m\n";
    std::cout << "  > ";
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

    std::cout << "  ------------------------------------------\n";
    std::cout << "  \033[33mEntry Preview -- " << today << "\033[0m\n";
    std::cout << "  ------------------------------------------\n";
    std::cout << "  Worked on : " << e.worked_on << "\n";
    std::cout << "  Learned   : " << e.learned   << "\n";
    std::cout << "  Blocked   : " << e.blocked   << "\n";
    std::cout << "  Tags      : " << e.tags      << "\n";
    std::cout << "  Mood      : " << e.mood      << "/5\n";
    std::cout << "  ------------------------------------------\n\n";

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
        std::cout << "  \033[32m[OK] Entry saved to " << filename << "\033[0m\n\n";
    } else {
        std::cout << "  \033[31m[x] Could not save entry.\033[0m\n\n";
    }
}

std::vector<std::string> getJsonFiles() {
    std::vector<std::string> files;

#ifdef _WIN32
    WIN32_FIND_DATAA ffd;
    HANDLE hFind = FindFirstFileA("logs\\*.json", &ffd);
    if (hFind == INVALID_HANDLE_VALUE) return files;
    do {
        files.push_back("logs/" + std::string(ffd.cFileName));
    } while (FindNextFileA(hFind, &ffd) != 0);
    FindClose(hFind);
#else
    DIR* dir = opendir("logs");
    if (!dir) return files;
    struct dirent* ent;
    while ((ent = readdir(dir)) != nullptr) {
        std::string name = ent->d_name;
        if (name.size() > 5 && name.substr(name.size() - 5) == ".json") {
            files.push_back("logs/" + name);
        }
    }
    closedir(dir);
#endif

    return files;
}

void listEntries() {
    std::vector<std::string> files = getJsonFiles();

    if (files.empty()) {
        std::cout << "  \033[31m[x] No logs folder or entries found.\033[0m\n";
        std::cout << "  Run './devlog new' to create your first entry!\n\n";
        return;
    }

    std::sort(files.begin(), files.end());

    int totalMood         = 0;
    std::string firstDate = "";
    std::string lastDate  = "";
    std::map<std::string, int> tagCount;

    std::cout << "\n  \033[33m------------------------------------------\033[0m\n";
    std::cout << "  \033[33mAll Entries (" << files.size() << " found)\033[0m\n";
    std::cout << "  \033[33m------------------------------------------\033[0m\n\n";

    int i = 1;
    for (const auto& filename : files) {
        Entry e = readEntry(filename);

        if (firstDate.empty()) firstDate = e.date;
        lastDate = e.date;
        totalMood += e.mood;

        std::istringstream iss(e.tags);
        std::string tag;
        while (iss >> tag) tagCount[tag]++;

        std::string moodBar = "";
        for (int m = 0; m < e.mood; m++)  moodBar += "*";
        for (int m = e.mood; m < 5; m++)  moodBar += "-";

        std::cout << "  \033[34m[" << i << "] " << e.date << "\033[0m\n";
        std::cout << "      Worked on : " << e.worked_on << "\n";
        std::cout << "      Learned   : " << e.learned   << "\n";
        std::cout << "      Blocked   : " << e.blocked   << "\n";
        std::cout << "      Tags      : " << e.tags      << "\n";
        std::cout << "      Mood      : [" << moodBar << "] " << e.mood << "/5\n\n";
        i++;
    }

    std::string topTag = "none";
    int topCount = 0;
    for (const auto& pair : tagCount) {
        if (pair.second > topCount) {
            topCount = pair.second;
            topTag   = pair.first;
        }
    }

    double avgMood = (double)totalMood / files.size();

    std::cout << "  \033[33m------------------------------------------\033[0m\n";
    std::cout << "  \033[33mSummary\033[0m\n";
    std::cout << "  \033[33m------------------------------------------\033[0m\n";
    std::cout << "  Total entries : " << files.size() << "\n";
    std::cout << "  Date range    : " << firstDate << "  ->  " << lastDate << "\n";
    std::cout << "  Avg mood      : " << std::fixed << std::setprecision(1) << avgMood << "/5\n";
    std::cout << "  Top tag       : " << topTag << " (" << topCount << " times)\n";
    std::cout << "  \033[33m------------------------------------------\033[0m\n\n";
}

void searchEntries(const std::string& keyword) {
    std::vector<std::string> files = getJsonFiles();

    if (files.empty()) {
        std::cout << "  \033[31m[x] No entries found.\033[0m\n";
        std::cout << "  Run './devlog new' to create your first entry!\n\n";
        return;
    }

    std::sort(files.begin(), files.end());

    std::vector<Entry> matches;
    for (const auto& filename : files) {
        Entry e = readEntry(filename);
        if (entryMatches(e, keyword)) matches.push_back(e);
    }

    if (matches.empty()) {
        std::cout << "\n  \033[31m[x] No entries found for: \"" << keyword << "\"\033[0m\n\n";
        return;
    }

    std::cout << "\n  \033[33m------------------------------------------\033[0m\n";
    std::cout << "  \033[33mSearch: \"" << keyword << "\" (" << matches.size() << " found)\033[0m\n";
    std::cout << "  \033[33m------------------------------------------\033[0m\n\n";

    int i = 1;
    for (const auto& e : matches) {
        std::string moodBar = "";
        for (int m = 0; m < e.mood; m++)  moodBar += "*";
        for (int m = e.mood; m < 5; m++)  moodBar += "-";

        std::cout << "  \033[34m[" << i << "] " << e.date << "\033[0m\n";
        std::cout << "      Worked on : " << e.worked_on << "\n";
        std::cout << "      Learned   : " << e.learned   << "\n";
        std::cout << "      Blocked   : " << e.blocked   << "\n";
        std::cout << "      Tags      : " << e.tags      << "\n";
        std::cout << "      Mood      : [" << moodBar << "] " << e.mood << "/5\n\n";
        i++;
    }

    std::cout << "  \033[33m------------------------------------------\033[0m\n";
    std::cout << "  " << matches.size() << " match(es) for \"" << keyword << "\"\n";
    std::cout << "  \033[33m------------------------------------------\033[0m\n\n";
}

void showStats() {
    std::vector<std::string> files = getJsonFiles();

    if (files.empty()) {
        std::cout << "  \033[31m[x] No entries found.\033[0m\n";
        std::cout << "  Run './devlog new' to create your first entry!\n\n";
        return;
    }

    std::sort(files.begin(), files.end());

    std::vector<Entry> entries;
    for (const auto& filename : files) entries.push_back(readEntry(filename));

    int totalEntries     = entries.size();
    int totalMood        = 0;
    int bestMood         = 0;
    std::string bestMoodDate = "";
    std::map<std::string, int> tagCount;

    for (const auto& e : entries) {
        totalMood += e.mood;
        if (e.mood > bestMood) {
            bestMood     = e.mood;
            bestMoodDate = e.date;
        }
        std::istringstream iss(e.tags);
        std::string tag;
        while (iss >> tag) tagCount[tag]++;
    }

    std::string topTag = "none";
    int topCount       = 0;
    for (const auto& pair : tagCount) {
        if (pair.second > topCount) {
            topCount = pair.second;
            topTag   = pair.first;
        }
    }

    int currentStreak = 0;
    std::string checkDate = getToday();
    for (int i = 0; i < totalEntries; i++) {
        std::ifstream f("logs/" + checkDate + ".json");
        if (f.is_open()) {
            f.close();
            currentStreak++;
            checkDate = addDays(checkDate, -1);
        } else break;
    }

    int longestStreak = 1;
    int currentRun    = 1;
    for (int i = 1; i < (int)entries.size(); i++) {
        if (entries[i].date == addDays(entries[i-1].date, 1)) {
            currentRun++;
            if (currentRun > longestStreak) longestStreak = currentRun;
        } else {
            currentRun = 1;
        }
    }

    double avgMood = (double)totalMood / totalEntries;

    std::cout << "\n  \033[33m------------------------------------------\033[0m\n";
    std::cout << "  \033[33mDEV STATS\033[0m\n";
    std::cout << "  \033[33m------------------------------------------\033[0m\n\n";
    std::cout << "  Total entries   : " << totalEntries << "\n";
    std::cout << "  Current streak  : " << currentStreak << " day(s)\n";
    std::cout << "  Longest streak  : " << longestStreak << " day(s)\n";
    std::cout << "  Best mood day   : " << bestMoodDate << " (" << bestMood << "/5)\n";
    std::cout << "  Most used tag   : " << topTag << " (" << topCount << " times)\n";
    std::cout << "  Avg mood        : " << std::fixed << std::setprecision(1) << avgMood << "/5\n";
    std::cout << "  Total tags used : " << tagCount.size() << " unique\n\n";

    std::cout << "  Mood trend (oldest -> newest):\n  ";
    for (const auto& e : entries) {
        if      (e.mood >= 4) std::cout << "\033[32m[G]\033[0m";
        else if (e.mood == 3) std::cout << "\033[33m[Y]\033[0m";
        else                  std::cout << "\033[31m[R]\033[0m";
    }
    std::cout << "\n  (G=good, Y=ok, R=rough)\n\n";
    std::cout << "  \033[33m------------------------------------------\033[0m\n\n";
}

void showWeek() {
    std::vector<std::string> files = getJsonFiles();
    std::sort(files.begin(), files.end());

    time_t now = time(0);
    tm* ltm = localtime(&now);
    int todayWeekday   = ltm->tm_wday;
    int daysFromMonday = (todayWeekday == 0) ? 6 : todayWeekday - 1;

    std::string weekStart = addDays(getToday(), -daysFromMonday);
    std::string weekEnd   = addDays(weekStart, 6);

    std::cout << "\n  \033[33m------------------------------------------\033[0m\n";
    std::cout << "  \033[33mTHIS WEEK | " << weekStart << " to " << weekEnd << "\033[0m\n";
    std::cout << "  \033[33m------------------------------------------\033[0m\n\n";

    std::vector<Entry> weekEntries;
    for (const auto& filename : files) {
        Entry e = readEntry(filename);
        if (e.date >= weekStart && e.date <= weekEnd) weekEntries.push_back(e);
    }

    if (weekEntries.empty()) {
        std::cout << "  No entries this week yet.\n";
        std::cout << "  Run './devlog new' to log today!\n\n";
        return;
    }

    int totalMood = 0;
    int i = 1;
    for (const auto& e : weekEntries) {
        std::string moodBar = "";
        for (int m = 0; m < e.mood; m++)  moodBar += "*";
        for (int m = e.mood; m < 5; m++)  moodBar += "-";

        std::cout << "  \033[34m[" << i << "] " << e.date << "\033[0m";
        std::cout << "  Mood: [" << moodBar << "] " << e.mood << "/5\n";
        std::cout << "      " << e.worked_on << "\n\n";

        totalMood += e.mood;
        i++;
    }

    double avgMood = (double)totalMood / weekEntries.size();

    std::cout << "  \033[33m------------------------------------------\033[0m\n";
    std::cout << "  Days logged : " << weekEntries.size() << "/7\n";
    std::cout << "  Avg mood    : " << std::fixed << std::setprecision(1) << avgMood << "/5\n";
    std::cout << "  \033[33m------------------------------------------\033[0m\n\n";
}

int main(int argc, char* argv[]) {

    #ifdef _WIN32
        system("color");
    #endif

    printBanner();

    // NEW -- runs every time devlog starts
    checkReminders();

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
            std::cout << "  \033[31m[x] Please provide a date.\033[0m\n";
            std::cout << "  Usage: ./devlog read 2026-03-01\n\n";
        } else {
            readCommand(argv[2]);
        }
    } else if (command == "list") {
        listEntries();
    } else if (command == "search") {
        if (argc < 3) {
            std::cout << "  \033[31m[x] Please provide a keyword.\033[0m\n";
            std::cout << "  Usage: ./devlog search cpp\n\n";
        } else {
            searchEntries(argv[2]);
        }
    } else if (command == "stats") {
        showStats();
    } else if (command == "week") {
        showWeek();
    } else if (command == "report") {
        std::cout << "  [report] Coming on Day 15!\n\n";
    } else if (command == "edit") {
        std::cout << "  [edit] Coming on Day 20!\n\n";
    } else {
        std::cout << "  Unknown command: \"" << command << "\"\n";
        std::cout << "  Run './devlog help' to see available commands.\n\n";
    }

    return 0;
}
