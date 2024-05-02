#include "helpers.h"

#include <iostream>
#include <sstream>
#include <vector>

void run_command(std::string line, PeterOS::ExtendedManager& manager) {
  std::istringstream iss(line);
  std::vector<std::string> tokens;
  std::string token;

  // Extract tokens and store them in a vector
  if (line != "") {
    while (iss >> token) {
      tokens.push_back(token);
    }

    std::string cmd = tokens[0];

    if (cmd == "id") {
      manager.init_default();
    } else if (cmd == "to") {
      manager.timeout();
    } else if (cmd == "cr") {
      manager.create(stoi(tokens[1]));
    } else if (cmd == "de") {
      if (manager.isValidDestroy(stoi(tokens[1]))) {
        manager.destroy(stoi(tokens[1]));
      }
    } else if (cmd == "rq") {
      manager.request(stoi(tokens[1]), stoi(tokens[2]));
    } else if (cmd == "rl") {
      int cur_proc = manager.isValidRelease(stoi(tokens[1]), stoi(tokens[2]));
      if (cur_proc != -1) {
        manager.release(cur_proc, stoi(tokens[1]), stoi(tokens[2]));
      }
    } else if (cmd == "in") {
      manager.init(stoi(tokens[1]), stoi(tokens[2]), stoi(tokens[3]),
                   stoi(tokens[4]), stoi(tokens[5]));
    } else if (cmd == "pp") {
      manager.print_parent(stoi(tokens[1]));
    } else if (cmd == "pc") {
      manager.print_children(stoi(tokens[1]));
    } else if (cmd == "prl") {
      manager.print_RL();
    } else if (cmd == "pr") {
      manager.print_resource(stoi(tokens[1]));
    } else if (cmd == "q") {
      std::cout << "Goodbye :)" << std::endl;
    } else {
      std::cout << "Invalid command" << std::endl;
    }
    manager.print_RL();
  }
}