# PeterOS:
A Process and Resource Management Shell Program

## Project Files
143b_p1_JZ\
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
[Upload](#how-to-upload) `143b_p1_JZ/` folder to **ICS Openlab**. Under the `143b_p1_JZ/` directory, running `make` in the command line will generate an executable named `PeterShell` under the `bin/` directory


### Required Option - mutually exclusive
- **`-m`**\
Starts the program in manual mode, allowing for users to enter inputs via command line
- **`-f INPUT_FILE` (Use this for grading)**\
Starts the program in file mode. The program reads the command from `INPUT_FILE` line by line and execute them sequentially to completion. A sample input file is placed under `rsrc/` folder.

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
2. Run `./bin/PeterShell -m` or `./bin/PeterShell -f [path_to_input.txt]` to start program
3. `make clean` deletes object files, executable, and output.txt
4. *File mode* will exit once the program executes every command from `input.txt`. The user can exit *manual mode* with `q` command. Both modes produce a `output.txt` in the the current working directory upon normal exit.

### How to Upload
Just a friendly reminder on how to upload file to ICS Openlab :>
```
scp -r /path/to/local/folder username@openlab.ics.uci.edu:/path/to/remote/destination
```