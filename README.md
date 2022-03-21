# Volga-IT Pathfinder
## Description
This project was created for [Volga-IT](https://volga-it.org/disciplines/#dis2792) olymp in C++ category.

Compiled executable program is using `input.txt` file in execution folder and produce `output.txt` file.
Input file must contain labyrinth 10x10 size, have one `@` and one `&` symbols for Ivan and Elena characters and unlimited `.` and `#` symbols.
Output file contains: meeting result message, turn count and explored labyrinth ASCII image.

This program runs in common mode by default. In this mode program awaits enter before closing. But also test mode exists. For using test mode you need to use console:

- `out_program_name.exe -t`
- `out_program_name.exe --test_mode`

Opposite to common mode, this doesn't require user input and closes the program immediately. This can be useful for automated tests.

Note: in case if you have some troubles with compilation (normally you haven't) I put executable binary in `exe` folder. This version of program represents `x64 Release` version.

## Documentation
Code was documented in Doxygen comment style. Also HTML version was created. In offline, you can access by using `doc/html/index.html` file.

## Examples
Example of an input file:
```
###.####..
..#.##..#.
...#.#...#
#...#..#..
##...@.###
.#.#....##
#...#####.
##....##.#
#..##.....
....##.&..
```

Example of an output file for previous input file:
```
Ivan and Elena had meet!
Turn count: 58

##????##??
..#??#..#?
...#?#...#
#...#..#..
?#...@.###
?#.#....#?
#...#####?
?#....##.#
#..##.....
....##.&..
```

Example of an input file:
```
#####.....
#@..#.....
#...#.....
#...#.....
#####.....
.....#####
.....#...#
.....#...#
.....#..&#
.....#####
```

Example of an output file for this previous file:
```
Ivan and Elena cannot meet!
Turn count: 16
```

## Compilation
### CMake (Any platform)
Follow these steps for getting executable:
1. Open Terminal, Windows Terminal or Bash for Windows.
2. Execute `git clone https://github.com/Helltraitor/volga-it-pathfinder.git` line.
3. Execute `cd volga-it-pathfinder` line.
4. Execute `mkdir build` line.
5. Execute `cd build` line.
6. Execute `cmake ../` line.
7. Execute  `cmake --build . --config Release` line.
8. Take executable file from `./Release`.

### Visual Studio
Follow this steps for getting executable:
1. Clone repository or download source files.
2. Open Microsoft Visual Studio in this folder.
3. Open `CMakeLists.txt` file, choose `x64 Release` and push button `Current document (CMakeLists.txt)`.
4. Take executable file from `out\build\x64-Release`.
