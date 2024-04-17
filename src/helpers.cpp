#include "helpers.h"

void run_command(std::string line, PeterOS::ExtendedManager& manager) {
    std::istringstream iss(line);
      std::vector<std::string> tokens;
      std::string token;

      // Extract tokens and store them in a vector
      while (iss >> token) {
          tokens.push_back(token);
      }

      std::string cmd = tokens[0];

      if (cmd == "id") {
        manager.init_default();
      }
      else if (cmd == "to") {
        manager.timeout();
      }
      else if (cmd == "cr") {
        manager.create(stoi(tokens[1]));
      }
      else if (cmd == "de") {
        manager.destroy(stoi(tokens[1]), 0);
        manager.print_RL();
        // manager.print_children();
      }
      else if (cmd == "rq") {
        manager.request(stoi(tokens[1]), stoi(tokens[2]));
      }
      else if (cmd == "rl") {
        manager.release(stoi(tokens[1]), stoi(tokens[2]));
      }
      else if (cmd == "in") {
        manager.init(stoi(tokens[1]), stoi(tokens[2]), stoi(tokens[3]), stoi(tokens[4]), stoi(tokens[5]));
      }
}