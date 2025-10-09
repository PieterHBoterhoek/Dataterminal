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
    std::unordered_map<std::string, std::function<void()>> commands;
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
            data.commands[key] = [url]() { openURL(url); };
        } else if (type == "steam") {
            int appId = val.value("value", 0);
            data.commands[key] = [appId]() {
                std::string command = "start steam://run/" + std::to_string(appId);
                system(command.c_str());
            };
        } else if (type == "response") {
            std::string response = val.value("value", "");
            data.commands[key] = [response]() { std::cout << response << "\n"; };
        } else {
            std::cerr << "Unknown command type for key: " << key << std::endl;
        }
    }

    return data; 
}

void randomWord() {
    // List of words
    std::vector<std::string> words = {
        "apple!", "banana!", "cherry!", "elderberry!", "cool!", "awesome!", "amazing!", "fantastic!"
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

int main() {
    // first load and check the path
    json config = getJsonPathFromConfig();
    if (config.empty()) {
        std::cerr << "No valid config found. Exiting.\n";
        return 1;
    }
    std::string jsonPath = config.value("jsonPath", "");
    int cir = config["prefs"].value("cir", 0);

    if (jsonPath.empty()) {
        std::cerr << "No valid path to commands.json was provided. Exiting.\n";
        return 1;
    }

    // then load and check the commands
    CommandData data = loadJsonCommands(jsonPath);

    if (data.commands.empty()) {
        std::cerr << "No commands were loaded. --Reset to enter a new path.\n";
    }

    std::string input;
    std::cout << "---------------------------\n|> DataTerminal - V.0.15 <|\n---------------------------\n"; 
    std::cout << "Welcome to Dataterminal - Type 'help' to list commands or 'exit' to quit\n"; 

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input == "help") {
            std::cout << "Available commands:\n";
            for (auto it = data.commandsJson.begin(); it != data.commandsJson.end(); ++it) {
                std::cout << "- " << it.key() << "\n";
            }
            std::cout << "Other commands: \n";
            std::cout << "- help\n- cir\n- --reset\n- exit\n";
            continue; // skip the rest of the loop or it will display a not a command warning
        }

        // for if the user wants to disable the random words
        if (input == "cir") {
            if (cir == 0) {
                cir = 1;
                std::cout << "Disabled random words \n";
            } else {
                cir = 0;
                std::cout << "Enabled random words \n";
            }

            // save/update the config
            config["prefs"]["cir"] = cir;
            std::ofstream out(getConfigFilePath());
            out << config.dump(4) << std::endl;

            continue;
        }
        
        if (input == "--reset") {
            fs::remove(getConfigFilePath());
            std::cout << "Configuration reset. Run the DataTerminal again to reconfigure.\n";
            return 0;
        }

        if (input == "exit") {
            std::cout << "Closing program.....\n";
            break;
        } 
        

        // look for a key that matches the user input in the map
        auto it = data.commands.find(input);
        if (it != data.commands.end()) { 
            it->second(); // call the lambda function associated with that key like "yt" [] () {openURL("youtube.com"); })
            if (cir == 0) {
                randomWord();
            }
        } else { 
            std::cout << "You entered: " << input << ", thats not a command silly!" "\nTry 'help' for a list of commands\n";
        }
    }

    return 0;
}
