Project 1: Dynamic Academic Performance and Risk Analysis Engine
Project description
This C program manages student academic records, calculates academic statistics, sorts and searches records, and identifies students who may be at academic risk.
Requirements
- A C compiler that supports the C11 standard (for example, GCC).
- A terminal or command prompt.
Files
- main.c — complete C source code.
- README.md — project instructions and documentation.
- sample_output.txt — example output from a run using the initial dataset.
Compile and run
On Linux or macOS, open a terminal in this folder and run:
gcc -std=c11 -Wall -Wextra -pedantic main.c -o academic
./academic
If your compiler requires explicit math-library linking, use:
gcc -std=c11 -Wall -Wextra -pedantic main.c -o academic -lm
./academic
On Windows with MinGW:
gcc -std=c11 -Wall -Wextra -pedantic main.c -o academic.exe
academic.exe
When the program starts, enter your own university student ID.
Initial dataset
The program starts with 10 sample student records:
ID	Name	Course	Grade
1001	Alice Johnson	Mathematics	85
1002	Brian Smith	Programming	48
1003	Clara Williams	Mathematics	92
1004	David Brown	Programming	65
1005	Esther Davis	Database	37
1006	Felix Martin	Database	76
1007	Grace Wilson	Mathematics	88
1008	Henry Thomas	Programming	55
1009	Irene Moore	Database	95
1010	James Taylor	Mathematics	29


These are example records and can be changed in initialize_students().
Menu operations
1. Add a student record.
2. Display all student records.
3. Sort by grade, name, or student ID.
4. Calculate the class average.
5. Find the top-performing student.
6. Search by student ID or exact full name.
7. Remove a student record.
8. Run the custom Academic Risk Index analysis.
9. Generate the academic summary report.
10. Exit and release allocated memory.
The program rejects duplicate student IDs and grades outside the range 0 to 100.
Custom analysis: Academic Risk Index
The custom analysis calculates:
Academic Risk Index = 100 - Student Grade
Risk categories:
- Low risk: grade from 70 to 100.
- Moderate risk: grade from 50 to below 70.
- High risk: grade below 50.
A lower risk index represents stronger academic performance. The analysis also calculates the percentage of students in each category.
C concepts demonstrated
- typedef struct for student records.
- calloc() for initial dynamic allocation.
- realloc() for growing and shrinking the student array.
- A double pointer (Student **) to allow the resizing function to update the allocated array.
- Pointer traversal when displaying records and calculating statistics.
- strcmp() for exact name searching and name sorting.
- Function pointers for menu-operation dispatch.
- Comparator function pointers for the three sorting strategies.
- free() to release allocated memory before the program exits.
Testing checklist
- [ ] Display the initial 10 records.
- [ ] Add a new student.
- [ ] Try adding a duplicate student ID.
- [ ] Try entering a grade below 0 or above 100.
- [ ] Sort by grade, name, and ID.
- [ ] Calculate the average and find the top student.
- [ ] Search for a student by ID and by exact name.
- [ ] Remove a student and display the remaining records.
- [ ] Run the Academic Risk Index analysis.
- [ ] Generate the summary report.
- [ ] Exit the program.
Expected initial summary
For the original 10 records:
- Total records: 10
- Average grade: 67.00
- Highest grade: 95.00
- Lowest grade: 29.00
- Passed (50 or above): 7
- Below pass mark: 3
- Pass rate: 70.00%
- Low risk: 5 students
- Moderate risk: 2 students
- High risk: 3 students