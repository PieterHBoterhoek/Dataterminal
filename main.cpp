#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <functional>
#include "nlohmann/json.hpp"
#include <cstdlib>       
#include <filesystem>    
#include <vector>
#include <random>
#include <ctime>
#include <windows.h>
namespace fs = std::filesystem;

using json = nlohmann::json;

fs::path getConfigFilePath() {
#ifdef _WIN32 // on windows try to get the user home directory and then build the path
    const char* homeDrive = std::getenv("HOMEDRIVE");
    const char* homePath = std::getenv("HOMEPATH");
    if (homeDrive && homePath) {
        return fs::path(std::string(homeDrive) + std::string(homePath)) / ".cmdrc";
    }
#else // on linux based machines
    const char* home = std::getenv("HOME");
    if (home) {
        return fs::path(home) / ".cmdrc";
    }
#endif
    return fs::path(".cmdrc"); // fallback to current working directory
}

// get the users json file path
json getJsonPathFromConfig() {
    fs::path configPath = getConfigFilePath();
    json config;

    if (fs::exists(configPath)) {
        std::ifstream in(configPath);
        try {
            in >> config;
        } catch (const std::exception& e) {
            std::cerr << "Failed to parse config file: " << e.what() << "\n";
            return {};
        }
        return config;
    }

    std::cout << "Welcome! It looks like this is your first time running the DataTerminal.\n";
    std::cout << "Please enter the full path to your commands.json file:\n> ";
    std::string jsonPath;
    std::getline(std::cin, jsonPath);

    // remove any quotes
    jsonPath.erase(std::remove(jsonPath.begin(), jsonPath.end(), '\"'), jsonPath.end());

    // check if file exists
    if (!fs::exists(jsonPath)) {
        std::cerr << "Error: Could not find the json file at " << jsonPath << "\n";
        return {};
    }

    // create and save config
    config["jsonPath"] = jsonPath;
    config["prefs"]["cir"] = 0; 

    std::ofstream out(configPath);
    out << config.dump(4) << std::endl;

    return config;
}


// define what to do on different OS
void openURL(const std::string& url) {
    #ifdef _WIN32
        system(("start " + url).c_str());
    #elif __APPLE__
        system(("open " + url).c_str());
    #else
        system(("xdg-open " + url).c_str());
    #endif
}

// create a struct for easy access
struct CommandData {
    std::unordered_map<std::string, std::function<bool()>> commands;
    json commandsJson;
};

// load the json using the given json file
CommandData loadJsonCommands(const std::string& filename) {
    CommandData data;

    // open the file then read the json file into it so { "yt": "youtube.com" } becomes { {"yt", "youtube.com"}}
    std::ifstream file(filename); 
    if (!file) { 
        std::cerr << "Failed to open " << filename << std::endl;
        return data;
    }

    file >> data.commandsJson;

    // store the key and values in the commands map so the later function can use them also check the type
    for (auto& [key, val] : data.commandsJson.items()) {
        if (!val.is_object()) continue;

        std::string type = val.value("type", "");
        
        if (type == "url") {
            std::string url = val.value("value", "");

            data.commands[key] = [url]() { 
                openURL(url); 
                return true;
            };

        } else if (type == "steam") {
            int appId = val.value("value", 0);

            data.commands[key] = [appId]() {
                std::string command = "start steam://run/" + std::to_string(appId);
                system(command.c_str());
                return true;
            };

        } else if (type == "response") {
            std::string response = val.value("value", "");

            data.commands[key] = [response]() { 
                std::cout << response << "\n"; 
                return false;
            };

        } else if (type == "app") {
            std::string path = val.value("value", "");

            data.commands[key] = [path]() {
                std::string command = "start \"\" \"" + path + "\"";
                system(command.c_str());
                return true;
            };
            
        } else {
            std::cerr << "  Unknown command type for key: " << key << std::endl;
        }
    }

    return data; 
}

// just a funny function to give you random words on input // disable it with cir
void randomWord() {
    // List of words
    std::vector<std::string> words = {
        "  apple!", "  banana!", "  cherry!", "  elderberry!", "  cool!", "  awesome!", "  amazing!", "  fantastic!"
    };

    // Initialize random engine with time-based seed
    std::mt19937 rng(static_cast<unsigned int>(time(nullptr)));

    // Create distribution for valid indices
    std::uniform_int_distribution<size_t> dist(0, words.size() - 1);

    // Choose random word
    std::string randomWord = words[dist(rng)];

    std::cout << randomWord <<"\n";
    return;
}

void closeTerminal() {
    // get current console window and terminate it
    HWND hwnd = GetConsoleWindow();
    
    if (hwnd != NULL) {
        DWORD processId;

        GetWindowThreadProcessId(hwnd, &processId);
        
        HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processId);
        
        // terminate the process
        if (hProcess != NULL) {
            TerminateProcess(hProcess, 0);
            CloseHandle(hProcess);
        }
    }
}

void SetColor(int textColor)
{
    std::cout << "\033[" << textColor << "m";
}

void ResetColor() { std::cout << "\033[0m"; }

// debugger
void debugFunction(json config) {
    SetColor(32);
    std::cout << "Debugger: \n";
    while (true) {
        std::string input;

        std::cout << "> ";
        std::getline(std::cin, input);

        if (input == "help") {
            std::cout << "  Available debug commands: \n";
            std::cout << "  - help\n  - processId\n  - exit\n";
            continue;
        }
        
        if (input == "processId") {
            HWND hwnd = GetConsoleWindow();
            if (hwnd != NULL) {
                DWORD processId;
                GetWindowThreadProcessId(hwnd, &processId);
                std::cout << "  cmd processId: " << processId << "\n";
            }
            continue;
        }
        
        if (input == "commandsPath") {
            std::cout << config["jsonPath"] << "\n";
            continue;
        }

        if (input == "prefs") {
            std::cout << config["prefs"] << "\n";
            continue;
        }

        if (input == "exit") {
            std::cout << "  Closing debug menu.....\n";
            ResetColor();
            break;
        } 
        else {
            std::cout << "  that is not a debug option\n";
            continue;
        }
    }
    return;
}

int main() {
    std::string LoadMsg = 
    "---------------------------\n"
    "|> DataTerminal - V.0.18.1 <|\n"
    "---------------------------\n"
    "Welcome to Dataterminal - Type 'help' to list commands or 'exit' to quit\n";

    // first load and check the path
    json config = getJsonPathFromConfig();
    if (config.empty()) {
        std::cerr << "  No valid config found. Exiting.\n";
        return 1;
    }
    std::string jsonPath = config.value("jsonPath", "");
    int cir = config["prefs"].value("cir", 0);
    int coi = config["prefs"].value("coi", 0);

    if (jsonPath.empty()) {
        std::cerr << "  No valid path to commands.json was provided. Exiting.\n";
        return 1;
    }

    // then load and check the commands
    CommandData data = loadJsonCommands(jsonPath);

    if (data.commands.empty()) {
        std::cerr << "  No commands were loaded. --Reset to enter a new path.\n";
    }

    std::string input;
    std::cout << LoadMsg;  

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input == "help") {
            std::cout << "  Available commands:\n";
            for (auto it = data.commandsJson.begin(); it != data.commandsJson.end(); ++it) {
                std::cout << "  - " << it.key() << "\n";
            }
            std::cout << "  Other commands: \n";
            std::cout << "  - help\n  - cir\n  - --reset\n  - exit\n";
            continue; // skip the rest of the loop or it will display a not a command warning
        }

        // for if the user wants to disable the random words (check input random?)
        if (input == "cir") {
            if (cir == 0) {
                cir = 1;
                SetColor(31);
                std::cout << "  Disabled random words \n";
                ResetColor();
            } else {
                cir = 0;
                SetColor(32);
                std::cout << "  Enabled random words \n";
                ResetColor();
            }

            // save/update the config
            config["prefs"]["cir"] = cir;
            std::ofstream out(getConfigFilePath());
            out << config.dump(4) << std::endl;

            continue;
        }

        // close on input
        if (input == "coi") {
            if (coi == 0) {
                coi = 1;
                SetColor(31);
                std::cout << "  Disabled close on input\n";
                ResetColor();
            } else {
                coi = 0;
                SetColor(32);
                std::cout << "  Enabled close on input \n";
                ResetColor();
            }

            config["prefs"]["coi"] = coi;
            std::ofstream out(getConfigFilePath());
            out << config.dump(4) << std::endl;

            continue;
        }

        if (input == "debug") {
            debugFunction(config);
            continue;
        }
        
        if (input == "--reset") {
            fs::remove(getConfigFilePath());
            std::cout << "  Configuration reset. Run the DataTerminal again to reconfigure.\n";
            return 0;
        }

        if (input == "exit") {
            std::cout << "  Closing program.....\n";
            closeTerminal();
            break;
        } 
        

        // look for a key that matches the user input in the map
        auto it = data.commands.find(input);

        if (it != data.commands.end()) { 
            bool shouldClose = it->second();// call the lambda function associated with that key like "yt" [] () {openURL("youtube.com"); }) and get the bool to check if coi should work

            if (cir == 0) 
                randomWord();
            if (coi == 0) {
                if (shouldClose) {
                    closeTerminal();
                    break;
                }
                else
                    continue;
            }
        } else { 
            std::cout << "  You entered: " << input << ", thats not a command silly!" "\n  Try 'help' for a list of commands\n";
        }
    }

    return 0;
}
