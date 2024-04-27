# PeterOS:
A Process and Resource Management Shell Program

## Project Files
.\
├── CMakeLists.txt\
├── include\
│   ├── exm.h\
│   ├── helpers.h\
│   └── usage.h\
├── Makefile\
├── README.md\
├── rsrc\
│   └── input.txt\
└── src\
    ├── exm.cpp\
    ├── helpers.cpp\
    └── shell.cpp

3 directories, 10 files
### `Header` files
| File | Description |
| --- | --- |
| `exm.h` | Defines the `ExtendedManager` class |
| `helpers.h` | Contains helper function definitions |
| `usage.h` | Contains usage message for using `PeterShell` |

### `cpp` Files
| File | Description |
| --- | --- |
| `exm.cpp` | Contains implementation for all shell operations |
| `helpers.cpp` | Processes user inputs and invoke appropriate shell operations |
| `shell.cpp` | A presentation shell for taking user commands |

### `Cmake` file
A `Makefile` contains configurations that compile and link appropriate object files into one executable `PeterShell` 

## How to run
Running `make` in the command line will generate an executable named `PeterShell` under the `bin/` directory


### Required Option - mutually exclusive
- **`-m`**\
Starts the program in manual mode, allowing for users to enter inputs via command line
- **`-f INPUT_FILE` (Use this for grading)**\
Starts the program in file mode. The program reads the command from `INPUT_FILE` line by line and execute them sequentially to completion

### Optional Option
- **`-v (to be implemented)`**\
The program will run in verbose mode, explaining the effects following each command and providing detailed reasons for errors

### Example:

To start program using manual mode:
```
$ ./bin/PeterShell -m
```
To start program using file mode:
```
$ ./bin/PeterShell -f rsrc/input.txt 
```

### TLDR:
1. `make`
2. `./bin/PeterShell -m` or `./bin/PeterShell -f [path_to_input.txt]`
3. `make clean` deletes object files, executable, and output.txt