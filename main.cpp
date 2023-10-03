#include <iostream>
#include <cstring>
#include <filesystem>
#include <map>
#include <limits>
#include <cstdlib>

#ifdef _WIN32 // Windows
#include <windows.h>
#define ChangeDirectory(path) SetCurrentDirectory(path)
#define CheckCommand(command) checkCommandWindows(command)

#else // Unix-like systems
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#define ChangeDirectory(path) chdir(path)
#define CheckCommand(command) checkCommandUnix(command)

#endif
// Function to check if a command exists on Windows
bool checkCommandWindows(const char* command) {
    std::string whereCmd = "where " + std::string(command) + " > nul 2>&1";
    int result = system(whereCmd.c_str());
    return result == 0;
}

// Function to check if a command exists on Unix-like systems
//bool checkCommandUnix(const char* command) {
//    pid_t childPid = fork();
//
//    if (childPid == 0) { // Child process
//        execl("/bin/sh", "/bin/sh", "-c", command, NULL);
//        _exit(EXIT_FAILURE);
//    } else if (childPid < 0) {
//        return false; // Fork failed
//    } else { // Parent process
//        int status;
//        waitpid(childPid, &status, 0);
//        return WEXITSTATUS(status) == 0; // Check if the exit status is 0
//    }
//}

int main() {
    std::map<std::string, std::string> customCommands;
    std::cout << "\nWelcome to your new command line interpreter." << std::endl;
    std::cout << "\nManual:\n" << "1.When combining commands to a special command,\n the position of each command is critical (first one will be preformed first)." << std::endl;
    std::cout << "2.When executing a special command, write the input in this format: inputForCommand1+inputForCommand2,\n if there is no + the input will be the input to the second command." << std::endl;
    std::cout << "3.You can't create a special command using special command and 'stop'." << std::endl;
    std::cout << "4.You can't create a special command using special command." << std::endl;
    std::cout << "5.Special command that is not a combination of commands does not take inputs.\n" << std::endl;

    std::cout << "Press anything to continue:\n";
    std::cin.get();
    std::cout << "\nPlease choose an action:" << std::endl;
    int stop = 1;
    while (stop) {
        int action;
        std::cout << "1. To preform commands, enter '1'." << std::endl;
        std::cout << "2. To preform actions on a command, enter '2'." << std::endl;
        std::cout << "3. To show all created commands, enter '3'." << std::endl;
        std::cout << "4. To exit, enter '4'.\n" << std::endl;
        while (!(std::cin >> action)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "You didn't enter a number.\n";
        }
        if (action > 4 || action < 1) {
            std::cout << "You didn't choose an option.\n" << std::endl;
            continue;
        } else if (action == 1) {
            std::cin.ignore();
            int condition = 1;
            while (condition) {
                std::cout << "\nPlease write your commands: (to stop write stop)" << std::endl;
                const int bufferSize = 100;  // Adjust the buffer size as needed
                char buffer[bufferSize];
                std::cin.getline(buffer, bufferSize);
                char* charPointer = new char[strlen(buffer) + 1];
                strcpy(charPointer, buffer);
                int inMap=0;
                std::string searchKeyStr = std::string(charPointer);
                std::string beforeSpace;
                std::string afterSpace;
                std::string commandOneInput;
                std::string commandTwoInput;

                size_t spacePos = searchKeyStr.find(' ');
                if (spacePos != std::string::npos) {
                    // Extract the substring before the first space
                    beforeSpace = searchKeyStr.substr(0, spacePos);

                    // Extract the substring after the first space (excluding the space itself)
                    afterSpace = searchKeyStr.substr(spacePos + 1);
                    size_t pos = afterSpace.find('+');
                    if(pos != std::string::npos){
                        commandOneInput = afterSpace.substr(0, pos); // "command1"
                        commandTwoInput = afterSpace.substr(pos + 1); // "command2"
                    } else {
                        commandTwoInput = afterSpace;
                    }

                } else {
                    // If no space is found, the whole string goes to partBeforeSpace
                    beforeSpace = searchKeyStr;
                    // PartAfterSpace is an empty string
                    commandTwoInput = "";
                    commandOneInput = "";
                }
                auto it = customCommands.find(beforeSpace);
                if (it != customCommands.end()) {
                    std::string value = it->second;
                    char targetChar = ' '; // The character you want to count
                    int count = 0; // Initialize the count to zero

                    for (char c : value) {
                        if (c == targetChar) {
                            count++; // Increment the count when the target character is found
                        }
                    }
                    if(count > 0){
                        std::cout << "here" << std::endl;
                        std::string print;
                        std::stringstream X(value);

                        while (getline(X, print, ' ')) {
                            char delimiter = '+'; // The character to use as the delimiter
                            size_t pos = print.find(delimiter);
                            size_t size1 = print.size();
                            if (size1 < 2){
                                continue;
                            }
                            if (pos != std::string::npos) {
                                std::string part1 = print.substr(0, pos); // Extract the first part
                                std::string part2 = print.substr(pos + 1); // Extract the second part

                                std::cout << "\n   command: " << part1 << "\n   input: " << part2 << std::endl;
                                std::string combine = part1;
                                combine += " ";
                                combine += part2;
                                const char* execute = combine.c_str();
                                if (strncmp(execute, "cd ", 3) == 0) {
                                    const char* directory = execute + 3;
                                    if (std::filesystem::exists(directory) && std::filesystem::is_directory(directory)) {
                                        std::filesystem::current_path(directory);
                                    } else {
                                        std::cout << "Directory does not exist or change failed: " << directory << std::endl;
                                    }
                                } else if (strcmp(buffer, "cd..") == 0) {
                                    std::filesystem::current_path("..");
                                } else{
                                    std::system(execute);
                                }
                            } else {
                                std::cout << "You managed to break my system, congrats! "  << std::endl;
                            }
                        }
                        inMap=1;
                    }
                    size_t pos = value.find('+');
                    if (pos != std::string::npos && inMap ==0) {
                        std::string part1 = value.substr(0, pos) + " " + commandOneInput; // "command1"
                        std::string part2 = value.substr(pos + 1) + " " + commandTwoInput; // "command2"
                        const char* charPointer1 = part1.c_str();
                        const char* charPointer2 = part2.c_str();
                        std::cout << "\n   command: " << value.substr(0, pos) << "\n   input: " << commandOneInput << std::endl;
                        if (strncmp(charPointer1, "cd ", 3) == 0) {
                            const char* directory = charPointer1 + 3;
                            if (std::filesystem::exists(directory) && std::filesystem::is_directory(directory)) {
                                std::filesystem::current_path(directory);
                            } else {
                                std::cout << "Directory does not exist or change failed: " << directory << std::endl;
                            }
                        } else if (strcmp(buffer, "cd..") == 0) {
                            std::filesystem::current_path("..");
                        } else{
                            std::system(charPointer1);
                        }
                        std::cout << "   command: " << value.substr(pos + 1) << "\n   input: " << commandTwoInput << std::endl;
                        if (strncmp(charPointer2, "cd ", 3) == 0) {
                            const char* directory = charPointer2 + 3;
                            if (std::filesystem::exists(directory) && std::filesystem::is_directory(directory)) {
                                std::filesystem::current_path(directory);
                            } else {
                                std::cout << "Directory does not exist or change failed: " << directory << std::endl;
                            }
                        } else if (strcmp(buffer, "cd..") == 0) {
                            std::filesystem::current_path("..");
                        } else {
                            std::system(charPointer2);
                        }

                    }
                    inMap=1;
                }

                if (std::strcmp(charPointer, "stop") == 0) {
                    condition = 0;
                } else if (strncmp(charPointer, "cd ", 3) == 0) {
                    const char* directory = charPointer + 3;
                    if (std::filesystem::exists(directory) && std::filesystem::is_directory(directory)) {
                        std::filesystem::current_path(directory);
                    } else {
                        std::cout << "Directory does not exist or change failed: " << directory << std::endl;
                    }
                } else if (strcmp(buffer, "cd..") == 0) {
                    std::filesystem::current_path("..");
                } else if(inMap == 0){
                    std::system(charPointer);
                }

                delete[] charPointer;
            }
        } else if (action == 2) {
            int option;
            std::cout << "\n1. To create a command, enter '1'" << std::endl;
            std::cout << "2. To combine commands, enter '2'" << std::endl;
            std::cout << "3. To change name of a command, enter '3'" << std::endl;
            std::cout << "4. To delete a command, enter '4'\n" << std::endl;
            while (!(std::cin >> option)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "You didn't enter a number.\n";
            }
            std::cin.ignore();
            if (option > 4 || option < 1) {
                std::cout << "You didn't choose an option." << std::endl;
                continue;
            } else if(option==1){
                std::cout << "Please write your command, the format is: \ncommand1 input1\ncommand2 input2\n..." << std::endl;
                std::string line;
                std::string script;
                int check=1;
                while (true) {
                    std::getline(std::cin, line);
                    if (line.empty()) {
                        break;
                    }
                    char delimiter = ' '; // The character to use as the delimiter
                    size_t pos = line.find(delimiter);

                    if (pos != std::string::npos) {
                        std::string part1 = line.substr(0, pos); // Extract the first part
                        std::string part2 = line.substr(pos + 1); // Extract the second part

                        std::cout << "Command: " << part1 << std::endl;
                        std::cout << "Input: " << part2 << std::endl;
                        script += part1;
                        script += "+";
                        script += part2;
                        script += " ";
                        std::string checkTheCommand = part1;
                        checkTheCommand += " ";
                        checkTheCommand += part2;
                        const char* command1 = checkTheCommand.c_str();
                        int exitCode1 = CheckCommand(command1);
                        if (exitCode1 == 0) {
                            std::cout << "Your command and input are valid." << std::endl;
                        } else {
                            std::cout << "Your command and input are not valid." << std::endl;
                            check=0;
                            break;
                        }
                    } else {
                        script += line;
                        script += "+";
                        script += "  ";
                        std::cout << "Command: " << line << std::endl;
                        std::cout << "Input:" << std::endl;
                        const char* command1 = line.c_str();
                        int exitCode1 = CheckCommand(command1);
                        if (exitCode1 == 0) {
                            std::cout << "Your command and input are valid." << std::endl;
                        } else {
                            std::cout << "Your command and input are not valid." << std::endl;
                            check=0;
                            break;
                        }
                    }
                }
                if(check==0){
                    continue;
                }
                std::cout << "your script:" << std::endl;
                std::string print;
                std::stringstream X(script);

                while (getline(X, print, ' ')) {
                    char delimiter = '+'; // The character to use as the delimiter
                    size_t pos = print.find(delimiter);
                    size_t size1 = print.size();
                    if (size1 < 2){
                        continue;
                    }
                    if (pos != std::string::npos) {
                        std::string part1 = print.substr(0, pos); // Extract the first part
                        std::string part2 = print.substr(pos + 1); // Extract the second part

                        std::cout << "   command: " << part1 << "\n   input: " << part2 << std::endl;
                    } else {
                        std::cout << "You managed to break my system, congrats! "  << std::endl;
                    }
                }
                std::cout << "Please write a name to your command.\n" << std::endl;
                std::string name;
                std::getline(std::cin, name);
                auto it = customCommands.find(name);
                if (it != customCommands.end()) {
                    int stopName=1;
                    while(stopName==1){
                        std::cout << "A command with this name already exists, please write other name:" << std::endl;
                        std::getline(std::cin, name);
                        auto itName = customCommands.find(name);
                        if(itName != customCommands.end()){
                            continue;
                        } else {
                            customCommands[name] = script;
                            std::cout << "The command has been created.\n" << std::endl;
                            stopName=0;
                        }
                    }
                } else {
                    customCommands[name] = script;
                    std::cout << "The command has been created.\n" << std::endl;
                }
            } else if (option==2){
                std::cout << "Please write your commands, the format is: command1+command2:\n" << std::endl;
                const int bufferSize = 100;
                char buffer[bufferSize];
                std::cin.getline(buffer, bufferSize);
                char* charPointer = new char[strlen(buffer) + 1];
                strcpy(charPointer, buffer);
                char* command1 = strtok(charPointer, "+");
                char* command2 = strtok(nullptr, "+");
                if (command1 && command2) {
                    int exitCode1 = CheckCommand(command1);
                    int exitCode2 = CheckCommand(command2);
                    if (exitCode1 == 0) {
                        std::cout << "Command 1 '" << command1 << "' is valid." << std::endl;
                    } else {
                        std::cout << "Command 1 '" << command1 << "' is not valid." << std::endl;
                    }
                    if (exitCode2 == 0) {
                        std::cout << "Command 2 '" << command2 << "' is valid." << std::endl;
                    } else {
                        std::cout << "Command 2 '" << command2 << "' is not valid." << std::endl;
                    }
                    if(exitCode1 == 0 && exitCode2 == 0){
                        std::string concatenatedCommand = command1 + std::string(command2);
                        customCommands[concatenatedCommand] = std::string(command1) + "+" + std::string(command2);
                        std::cout << "The command has been created." << std::endl;
                        std::cout << "The command name is " << command1 << command2 << ".\n" << std::endl;

                    } else {
                        std::cout << "The command has not been created.\n" << std::endl;
                    }
                } else {
                    std::cout << "Invalid input format. Use 'command1+command2'.\n" << std::endl;
                }
                delete[] charPointer;
            } else if(option == 3){
                std::cout << "\nPlease write the command name you want to change:\n" << std::endl;
                const int bufferSize = 100;
                char buffer[bufferSize];
                std::cin.getline(buffer, bufferSize);
                char* charPointer = new char[strlen(buffer) + 1];
                strcpy(charPointer, buffer);
                std::cout << "\nPlease write the new command name:\n" << std::endl;
                const int bufferSize2 = 100;
                char buffer2[bufferSize2];
                std::cin.getline(buffer2, bufferSize2);
                char* charPointer2 = new char[strlen(buffer2) + 1];
                strcpy(charPointer2, buffer2);
                std::string searchKeyStr = std::string(charPointer);
                std::string newKeyStr = std::string(charPointer2);
                auto it = customCommands.find(searchKeyStr);
                if (it != customCommands.end()) {
                    std::string value = it->second;
                    customCommands.erase(it);
                    customCommands[newKeyStr] = value;
                    std::cout << "The command name has updated successfully.\n" << std::endl;
                } else {
                    std::cout << "The command was not found.\n" << std::endl;
                }
                delete[] charPointer;
                delete[] charPointer2;
            } else {
                std::cout << "\nPlease write the command name you want to delete:\n" << std::endl;
                const int bufferSize = 100;
                char buffer[bufferSize];
                std::cin.getline(buffer, bufferSize);
                char* charPointer = new char[strlen(buffer) + 1];
                strcpy(charPointer, buffer);
                std::string searchKeyStr = std::string(charPointer);
                auto it = customCommands.find(searchKeyStr);
                if (it != customCommands.end()) {
                    customCommands.erase(it);
                    std::cout << "The command has been deleted.\n" << std::endl;
                } else {
                    std::cout << "The command was not found.\n" << std::endl;
                }
                delete[] charPointer;
            }

        } else if(action==3) {
            std::cin.ignore();
            int count=0;
            for (const auto& pair : customCommands) {
                count++;
                if(count==1){
                    std::cout << "\nYour special commands:" << std::endl;
                }
                char targetChar = ' '; // The character you want to count
                int charCount = 0; // Initialize the count to zero

                for (char c : pair.second) {
                    if (c == targetChar) {
                        charCount++; // Increment the count when the target character is found
                    }
                }
                if(charCount == 0){
                    std::cout <<count<< ". " << "command name: " << pair.first << ", combined commands: " << pair.second << std::endl;
                } else {
                    std::cout <<count<< ". " << "command name: " << pair.first << ", The command actions: " << std::endl;
                    std::string script=pair.second;
                    std::string print;
                    std::stringstream X(script);

                    while (getline(X, print, ' ')) {
                        char delimiter = '+'; // The character to use as the delimiter
                        size_t pos = print.find(delimiter);
                        size_t size1 = print.size();
                        if (size1 < 2){
                            continue;
                        }
                        if (pos != std::string::npos) {
                            std::string part1 = print.substr(0, pos); // Extract the first part
                            std::string part2 = print.substr(pos + 1); // Extract the second part

                            std::cout << "   command: " << part1 << "\n   input: " << part2 << std::endl;
                        } else {
                            std::cout << "You managed to break my system, congrats! "  << std::endl;
                        }
                    }

                }
            }
            if(count==0){
                std::cout << "\nYou have no special commands.\n" << std::endl;
            } else {
                std::cout << "" << std::endl;
            }
        } else {
                std::cin.ignore();
                stop = 0;
                std::cout << "\nClosing your new command line interpreter.\n" << std::endl;
            }
        }
    return 0;
}

