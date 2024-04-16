#include <fstream>
#include <iostream>
#include <unistd.h>
#include "usage.h"
#include "helpers.h"
#include "exm.h"


int main(int argc, char *argv[]) {
  int f_flag = 0;
  int m_flag = 0;
  char* INFILE = NULL;

  // command line argument parsing
  int option;
  while ((option = getopt(argc, argv, "fm")) != -1) {
    switch (option) {
      case 'f': // file mode
        f_flag = 1;
        break;
      case 'm': // manual mode
        m_flag = 1;
        break;
      case '?': // Unknown option
        std::cerr << "Unknown option: " << char(optopt) << std::endl;
        std::cerr << USAGE_MSG;
        return -1;
    }
  }

  if (argc < 2) {
    std::cerr << "Not enough arugments\n" << USAGE_MSG;
    return -1;
  }

  if (optind + 1 == argc) {
    INFILE = *(argv + optind);
  }
  else {
    if (!m_flag) {
      std::cerr << "Error parising input file\n" << USAGE_MSG;
      return -1;
    }
  }

  if (! (f_flag | m_flag)) {
    std::cerr << "Required option not specified\n" << USAGE_MSG;
    return -1;
  }

  if (f_flag && m_flag) {
    std::cerr << "Too many arguments\n" << USAGE_MSG;
    return -1;
  }

  // main process
  PeterOS::ExtendedManager& manager = PeterOS::ExtendedManager::instance();
  // File mode
  if (f_flag) {
    std::cout << "Reading command from file: " << INFILE << std::endl;
    std::ifstream file(INFILE);
    if (!file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return 1;
    }
    std::string line;
    while (std::getline(file, line)) {
      run_command(line, manager);
      std::cout << line << std::endl;
    }
    file.close(); // Close the file
  }

  // Manual mode
  if (m_flag) {
    std::cout << "[Manual mode]" << std::endl;
    std::string line;
    while (line != "q") {
      std::cout << "Enter a command: ";
      std::getline(std::cin, line);
      run_command(line, manager);
    }
  }

  return 0;
}