Dynamic Text Mining and Linguistic Pattern Analyzer
Project overview
This program accepts a paragraph of text, stores it in dynamically allocated memory, extracts words, counts their frequencies and performs different text analyses.

The user enters the source title and author/publication details before pasting the paragraph. The paragraph can be longer than a fixed input buffer because the program grows its text allocation as more lines are entered.
Files
- main.c — complete C source code.
- README.md — project documentation.
- sample_output.txt — illustrative output and test examples.
Compile and run
On macOS or Linux:
gcc -std=c11 -Wall -Wextra -pedantic main.c -o text_analyzer
./text_analyzer
On Windows with MinGW:
gcc -std=c11 -Wall -Wextra -pedantic main.c -o text_analyzer.exe
text_analyzer.exe
How to enter text
1. Enter your student ID.
2. Enter the title of the book/article.
3. Enter the author or publication.
4. Paste or type the paragraph.
5. Press ENTER on an empty line to finish entering the paragraph.
Use a paragraph of at least 150 words from a source you are allowed to use. Record the real source details in the program.
Menu operations
1. Count total words.
2. Find the longest word.
3. Find the most frequent word.
4. Display distinct words and their frequencies.
5. Sort word statistics alphabetically or by frequency.
6. Search for a word.
7. Generate a text summary.
8. Run lexical diversity analysis.
9. Inspect memory addresses.
10. Exit and release allocated memory.
Word handling rules
- Uppercase and lowercase letters are treated as the same word. For example, Text and text are counted together as text.
- Words are made from letters and digits.
- Punctuation marks are separators.
- Apostrophes split a token. For example, don't is treated as don and t.
- Hyphens split a token. For example, well-known is treated as well and known.
- Repeated spaces, tabs and line breaks do not create empty words.
- A token longer than 39 characters is truncated to its first 39 characters before it is stored. This is documented so that the fixed name[40] field is never overflowed.
Custom analysis: Lexical Diversity Score
The program calculates lexical diversity using:
Lexical Diversity Score = (Distinct words / Total words) * 100
Distinct words are the unique normalized words in the paragraph. Total words includes every recognized occurrence. A higher percentage means a greater proportion of the text consists of different words.
Memory inspection
Option 9 prints:
- The address of the dynamically allocated text buffer.
- The address of the dynamically allocated word-statistics array.
- The address of the first word-statistics entry, if one exists.
The addresses are printed with %p after casting the pointers to (void *). The values can change each time the program runs.
C concepts demonstrated
- Structures using typedef struct.
- Dynamic memory allocation with realloc().
- Dynamic text-buffer growth.
- Dynamic word-statistics array growth.
- Pointer traversal through the input text.
- strcmp(), strlen() and strcpy().
- Character handling with isalnum() and tolower().
- Function-pointer dispatch for menu operations.
- qsort() with comparator function pointers.
- Memory address inspection with %p.
- Memory cleanup with free().
Testing checklist
- [ ] Enter a paragraph of at least 150 words.
- [ ] Include repeated words with different capitalization.
- [ ] Include punctuation, apostrophes and hyphenated words.
- [ ] Check total word count.
- [ ] Check longest and most frequent words.
- [ ] Display and sort word statistics both ways.
- [ ] Search for a word that exists and one that does not.
- [ ] Generate the summary and lexical diversity score.
- [ ] Inspect the memory addresses.
- [ ] Exit and confirm allocated memory is released.