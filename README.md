# Programming Projects Portfolio

**Author:** Liata Sifa Ornella  
**Language:** C  
**Repository:** [programmingproject2](https://github.com/Liata-Ornella/programmingproject2)

## About this repository

This repository contains three C programming projects completed as part of my programming coursework. The projects demonstrate how I applied core C concepts to build interactive, menu-driven programs. They cover dynamic memory management, structures, unions, arrays, pointers, strings, sorting, searching, random data generation, and function pointers.

The repository includes the main implementations of the projects. Where I developed more than one version of a project, both the original version and the revised/student-style version are part of my work. The versions are intended to demonstrate the same required functionality; the original versions retain the more formal wording and presentation.

---

## Project 1: Dynamic Academic Performance and Risk Analysis

This project is a student-record management system. It stores student information and provides tools to review academic performance and identify students who may need support.

### Main features
- Stores student ID, name, course, and grade in a `struct`.
- Starts with sample student records and allows new students to be added.
- Dynamically grows and shrinks the student array using memory allocation functions.
- Displays student records and sorts them by grade, name, or ID.
- Calculates the class average and identifies the top-performing student.
- Searches for students by ID or name.
- Removes a student using their ID.
- Runs a custom academic analysis and generates a summary report.
- Uses function pointers to connect menu choices to their functions.

### C concepts practised
Structures, arrays of structures, dynamic memory allocation, pointers and pointer traversal, strings and `strcmp()`, sorting, searching, modular functions, and function pointers.

---

## Project 2: Dynamic Text Mining and Linguistic Pattern Analyzer

This project analyses a paragraph entered by the user. It processes the text and produces basic statistics about the words used.

### Main features
- Accepts a paragraph of at least 150 words and records its source title and author/publication.
- Stores the text dynamically so that the paragraph can contain multiple lines.
- Counts the total number of words and the number of distinct words.
- Builds a dynamically allocated word-frequency list.
- Finds the longest word and the most frequently used word.
- Displays word statistics and sorts them alphabetically or by frequency.
- Searches for a word entered by the user.
- Produces a text summary and calculates lexical diversity.
- Includes a memory-inspection option to display addresses used by the program.
- Uses function pointers to dispatch menu operations.

### Text-processing rules
Words are normalised to lowercase. Letters and digits form words; punctuation, apostrophes, and hyphens are treated as separators. The word-statistics structure has a fixed-size word field, so very long words are limited to the available field size, as documented in the source/README.

### C concepts practised
Dynamic strings, `malloc()`/`realloc()`/`free()`, structures, arrays, pointers, pointer traversal, `strlen()`, `strcpy()`, `strcmp()`, sorting with comparator functions, and function pointers.

---

## Project 3: Dynamic IoT Event Monitoring and Callback Engine

This project simulates an IoT monitoring system in which different devices generate readings. Each reading is sent to a callback function that checks whether the device is operating within its expected range.

### Supported device types
- **Temperature sensor:** readings are checked against a normal range of 0–40 °C.
- **Pressure sensor:** readings are checked against a normal range of 90–110 kPa.
- **Battery voltage monitor:** readings are checked against a normal range of 3.30–4.20 V.

The simulation generates values over wider documented ranges so that both normal readings and abnormal alerts can be demonstrated.

### Main features
- Registers devices with unique IDs and prevents duplicate IDs.
- Stores devices in a dynamically allocated array that can grow and shrink.
- Displays registered devices and removes a device by ID.
- Generates at least 10 simulated readings in a monitoring run.
- Dispatches readings to separate temperature, pressure, and battery callback functions.
- Updates device status and reports abnormal readings with the responsible callback.
- Displays detected alerts and a monitoring summary.
- Uses a custom sensor-drift callback to compare a device's current reading with its previous reading.

### Custom callback: Sensor Drift Detector
The `sensor_drift_detector()` callback compares each device's current reading with its previous reading. It reports a drift warning when the absolute change exceeds the documented limit for that sensor type: 8 °C for temperature, 15 kPa for pressure, or 0.35 V for battery voltage. The first reading becomes the baseline. After each comparison, the current reading is saved for the next check.

### C concepts practised
Structures, unions, arrays of structures, dynamic memory allocation, pointers, pointer traversal, random number generation with `rand()` and `srand()`, callback functions, and function pointers.

---

## Building and running the projects

Each project has its own source file and should be compiled from its own project folder.

### Project 1 — Academic Performance

```bash
gcc -std=c11 -Wall -Wextra -pedantic main.c -o academic
./academic
```

If the source file in your Project 1 folder is named `academic_risk_engine.c`, compile it using:

```bash
gcc -std=c11 -Wall -Wextra -pedantic academic_risk_engine.c -o academic
./academic
```

### Project 2 — Text Mining

```bash
gcc -std=c11 -Wall -Wextra -pedantic main.c -o text_analyzer
./text_analyzer
```

Enter a paragraph of at least 150 words when prompted. Use the correct source title and author/publication details if your assignment requires a published source.

### Project 3 — IoT Monitoring

```bash
gcc -std=c11 -Wall -Wextra -pedantic main.c -o iot_monitor
./iot_monitor
```

Register devices of the three supported types before generating readings. The program requires at least 10 readings per simulation.

> **Note:** The exact source filename may differ depending on which version of a project folder you open. Run the compile command from the folder containing that source file.

---

## Learning outcomes

Through these projects, I practised:
- Breaking a larger problem into smaller functions.
- Designing and using structures and unions.
- Allocating, resizing, and freeing memory safely.
- Traversing arrays with pointers.
- Working with strings and text data.
- Using function pointers to select operations and callbacks.
- Validating user input and handling common error conditions.
- Testing programs with sample data and reviewing their output.

## Author

**Liata Sifa Ornella**

C Programming Coursework Projects
