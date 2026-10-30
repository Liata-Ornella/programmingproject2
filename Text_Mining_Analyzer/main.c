#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#define WORD_LENGTH 40
#define INITIAL_CAPACITY 16
#define LINE_SIZE 512
#define TITLE_LENGTH 120
#define AUTHOR_LENGTH 100

typedef struct {
    char name[WORD_LENGTH];
    size_t count;
} WordStat;

typedef struct {
    char *text;
    size_t text_length;
    WordStat *words;
    size_t word_count;
    size_t word_capacity;
    size_t total_words;
    char source_title[TITLE_LENGTH];
    char source_author[AUTHOR_LENGTH];
} TextAnalyzer;

typedef void (*AnalysisFunction)(TextAnalyzer *);

static int read_line(const char *prompt, char *buffer, size_t size);
static int read_integer(const char *prompt, int *value);
static int enter_text(TextAnalyzer *analyzer);
static int append_text(char **text, size_t *length, size_t *capacity,
                       const char *line);
static int rebuild_statistics(TextAnalyzer *analyzer);
static int add_word(TextAnalyzer *analyzer, const char *word);
static void normalize_word(char *word);
static void free_analyzer(TextAnalyzer *analyzer);

static void count_words(TextAnalyzer *analyzer);
static void longest_word(TextAnalyzer *analyzer);
static void most_frequent(TextAnalyzer *analyzer);
static void display_word_statistics(TextAnalyzer *analyzer);
static void sort_word_statistics(TextAnalyzer *analyzer);
static void search_word(TextAnalyzer *analyzer);
static void generate_text_summary(TextAnalyzer *analyzer);
static void lexical_diversity_analysis(TextAnalyzer *analyzer);
static void inspect_memory(TextAnalyzer *analyzer);
static void show_menu(TextAnalyzer *analyzer);

static int compare_alphabetically(const void *a, const void *b);
static int compare_by_frequency(const void *a, const void *b);

int main(void)
{
    TextAnalyzer analyzer = {NULL, 0, NULL, 0, 0, 0, "", ""};
    int choice;

    char student_id[40];

    printf("Enter your student ID: ");
    if (!read_line("", student_id, sizeof(student_id)) ||
        student_id[0] == '\0') {
        printf("A student ID is required.\n");
        return EXIT_FAILURE;
    }

    printf("\n===============================================\n");
    printf("Student ID: %s\n", student_id);
    printf("Project: Dynamic Text Mining and Linguistic Pattern Analyzer\n");
    printf("Custom Analysis: Lexical Diversity Score\n");
    printf("===============================================\n");

    printf("\nEnter source title: ");
    if (!read_line("", analyzer.source_title,
                   sizeof(analyzer.source_title)) ||
        analyzer.source_title[0] == '\0') {
        printf("A source title is required.\n");
        return EXIT_FAILURE;
    }

    printf("Enter author or publication: ");
    if (!read_line("", analyzer.source_author,
                   sizeof(analyzer.source_author)) ||
        analyzer.source_author[0] == '\0') {
        printf("Author or publication details are required.\n");
        return EXIT_FAILURE;
    }

    if (!enter_text(&analyzer)) {
        free_analyzer(&analyzer);
        return EXIT_FAILURE;
    }

    AnalysisFunction operations[8] = {
        count_words,
        longest_word,
        most_frequent,
        display_word_statistics,
        sort_word_statistics,
        search_word,
        generate_text_summary,
        lexical_diversity_analysis
    };

    do {
        show_menu(&analyzer);

        if (!read_integer("Choose an option: ", &choice)) {
            printf("Invalid input. Enter a whole number.\n");
            continue;
        }

        if (choice >= 1 && choice <= 8) {
            operations[choice - 1](&analyzer);
        } else if (choice == 9) {
            inspect_memory(&analyzer);
        } else if (choice == 10) {
            printf("Exiting text analyzer...\n");
        } else {
            printf("Choose an option from 1 to 10.\n");
        }

    } while (choice != 10);

    free_analyzer(&analyzer);
    printf("Allocated memory released. Program finished.\n");
    return EXIT_SUCCESS;
}

static int read_line(const char *prompt, char *buffer, size_t size)
{
    size_t len;
    int ch;

    if (size == 0) return 0;
    printf("%s", prompt);

    if (fgets(buffer, (int)size, stdin) == NULL) return 0;

    len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
        return 1;
    }

    ch = getchar();
    if (ch != '\n' && ch != EOF) {
        while (ch != '\n' && ch != EOF) ch = getchar();
        buffer[0] = '\0';
        return 0;
    }

    return 1;
}

static int read_integer(const char *prompt, int *value)
{
    char buffer[100], *end;
    long result;

    if (!read_line(prompt, buffer, sizeof(buffer))) return 0;
    errno = 0;
    result = strtol(buffer, &end, 10);
    while (*end == ' ' || *end == '\t') end++;

    if (buffer == end || *end != '\0' || errno == ERANGE ||
        result < -2147483647L - 1L || result > 2147483647L)
        return 0;

    *value = (int)result;
    return 1;
}

/* Read any number of lines. An empty line ends paragraph entry. */
static int enter_text(TextAnalyzer *analyzer)
{
    char line[LINE_SIZE];
    size_t capacity = 0;

    printf("\nPaste or type your paragraph below.\n");
    printf("Press ENTER on an empty line when you have finished.\n\n");

    analyzer->text = NULL;
    analyzer->text_length = 0;

    while (1) {
        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("Input ended unexpectedly.\n");
            return 0;
        }

        if (strcmp(line, "\n") == 0 || strcmp(line, "\r\n") == 0)
            break;

        if (!append_text(&analyzer->text, &analyzer->text_length,
                         &capacity, line)) {
            printf("Unable to allocate enough memory for the text.\n");
            return 0;
        }
    }

    if (analyzer->text_length == 0) {
        printf("No text was entered.\n");
        return 0;
    }

    if (!rebuild_statistics(analyzer)) {
        printf("Unable to build word statistics.\n");
        return 0;
    }

    printf("\nText loaded successfully.\n");
    printf("Source: %s — %s\n",
           analyzer->source_title, analyzer->source_author);
    printf("Words detected: %zu\n", analyzer->total_words);
    return 1;
}

static int append_text(char **text, size_t *length, size_t *capacity,
                       const char *line)
{
    size_t line_length = strlen(line);
    size_t needed = *length + line_length + 1;

    if (needed > *capacity) {
        size_t new_capacity = (*capacity == 0) ? 256 : *capacity;
        char *temporary;

        while (new_capacity < needed) {
            if (new_capacity > ((size_t)-1) / 2) return 0;
            new_capacity *= 2;
        }

        temporary = realloc(*text, new_capacity);
        if (temporary == NULL) return 0;

        *text = temporary;
        *capacity = new_capacity;
    }

    memcpy(*text + *length, line, line_length);
    *length += line_length;
    (*text)[*length] = '\0';
    return 1;
}

static void normalize_word(char *word)
{
    char *ptr;
    for (ptr = word; *ptr != '\0'; ptr++) {
        *ptr = (char)tolower((unsigned char)*ptr);
    }
}

static int add_word(TextAnalyzer *analyzer, const char *word)
{
    size_t i;
    WordStat *temporary;

    for (i = 0; i < analyzer->word_count; i++) {
        if (strcmp(analyzer->words[i].name, word) == 0) {
            analyzer->words[i].count++;
            analyzer->total_words++;
            return 1;
        }
    }

    if (analyzer->word_count == analyzer->word_capacity) {
        size_t new_capacity = analyzer->word_capacity == 0
                                  ? INITIAL_CAPACITY
                                  : analyzer->word_capacity * 2;

        if (new_capacity > ((size_t)-1) / sizeof(WordStat))
            return 0;

        temporary = realloc(analyzer->words,
                            new_capacity * sizeof(WordStat));
        if (temporary == NULL) return 0;

        analyzer->words = temporary;
        analyzer->word_capacity = new_capacity;
    }

    strcpy(analyzer->words[analyzer->word_count].name, word);
    analyzer->words[analyzer->word_count].count = 1;
    analyzer->word_count++;
    analyzer->total_words++;
    return 1;
}

/*
 * Words are sequences of alphabetic characters or digits.
 * Punctuation, apostrophes and hyphens are separators.
 * Case is ignored. Tokens longer than 39 characters are safely
 * truncated to 39 characters before they are stored.
 */
static int rebuild_statistics(TextAnalyzer *analyzer)
{
    const char *ptr = analyzer->text;
    char word[WORD_LENGTH];
    size_t used;

    analyzer->word_count = 0;
    analyzer->total_words = 0;

    while (*ptr != '\0') {
        while (*ptr != '\0' &&
               !isalnum((unsigned char)*ptr)) {
            ptr++;
        }

        if (*ptr == '\0') break;

        used = 0;
        while (*ptr != '\0' && isalnum((unsigned char)*ptr)) {
            if (used < WORD_LENGTH - 1) {
                word[used++] = *ptr;
            }
            ptr++;
        }

        if (used > 0) {
            word[used] = '\0';
            normalize_word(word);

            if (!add_word(analyzer, word)) return 0;
        }
    }

    return 1;
}

static void count_words(TextAnalyzer *analyzer)
{
    printf("Total number of words: %zu\n", analyzer->total_words);
}

static void longest_word(TextAnalyzer *analyzer)
{
    size_t i, longest = 0;
    const char *word = NULL;

    if (analyzer->word_count == 0) {
        printf("There are no words to inspect.\n");
        return;
    }

    for (i = 0; i < analyzer->word_count; i++) {
        size_t length = strlen(analyzer->words[i].name);
        if (length > longest) {
            longest = length;
            word = analyzer->words[i].name;
        }
    }

    printf("Longest word: %s\n", word);
    printf("Length: %zu characters\n", longest);
}

static void most_frequent(TextAnalyzer *analyzer)
{
    size_t i, best = 0;

    if (analyzer->word_count == 0) {
        printf("There are no words to inspect.\n");
        return;
    }

    for (i = 1; i < analyzer->word_count; i++) {
        if (analyzer->words[i].count > analyzer->words[best].count)
            best = i;
    }

    printf("Most frequent word: %s\n", analyzer->words[best].name);
    printf("Frequency: %zu\n", analyzer->words[best].count);
}

static void display_word_statistics(TextAnalyzer *analyzer)
{
    size_t i;

    if (analyzer->word_count == 0) {
        printf("No word statistics available.\n");
        return;
    }

    printf("\n%-25s %s\n", "WORD", "FREQUENCY");
    printf("------------------------------------\n");

    for (i = 0; i < analyzer->word_count; i++) {
        printf("%-25s %zu\n",
               analyzer->words[i].name,
               analyzer->words[i].count);
    }

    printf("Distinct words: %zu\n", analyzer->word_count);
}

static int compare_alphabetically(const void *a, const void *b)
{
    const WordStat *w1 = (const WordStat *)a;
    const WordStat *w2 = (const WordStat *)b;
    return strcmp(w1->name, w2->name);
}

static int compare_by_frequency(const void *a, const void *b)
{
    const WordStat *w1 = (const WordStat *)a;
    const WordStat *w2 = (const WordStat *)b;

    if (w1->count < w2->count) return 1;
    if (w1->count > w2->count) return -1;
    return strcmp(w1->name, w2->name);
}

static void sort_word_statistics(TextAnalyzer *analyzer)
{
    int choice;

    if (analyzer->word_count < 2) {
        printf("Not enough distinct words to sort.\n");
        return;
    }

    printf("1. Alphabetical order\n");
    printf("2. Frequency (highest first)\n");

    if (!read_integer("Choose sorting method: ", &choice) ||
        (choice != 1 && choice != 2)) {
        printf("Invalid sorting option.\n");
        return;
    }

    qsort(analyzer->words, analyzer->word_count, sizeof(WordStat),
          choice == 1 ? compare_alphabetically : compare_by_frequency);

    printf("Word statistics sorted.\n");
    display_word_statistics(analyzer);
}

static void search_word(TextAnalyzer *analyzer)
{
    char query[WORD_LENGTH];
    size_t i;

    if (!read_line("Enter a word to search for: ",
                   query, sizeof(query))) {
        printf("Invalid search word.\n");
        return;
    }

    normalize_word(query);

    for (i = 0; i < analyzer->word_count; i++) {
        if (strcmp(analyzer->words[i].name, query) == 0) {
            printf("'%s' occurs %zu time(s).\n",
                   query, analyzer->words[i].count);
            return;
        }
    }

    printf("The word '%s' was not found.\n", query);
}

static void generate_text_summary(TextAnalyzer *analyzer)
{
    size_t i, longest_length = 0, total_characters = 0;
    const char *longest = "(none)";
    size_t most_index = 0;
    double average = 0.0;

    if (analyzer->total_words == 0) {
        printf("No words available for a summary.\n");
        return;
    }

    for (i = 0; i < analyzer->word_count; i++) {
        size_t length = strlen(analyzer->words[i].name);
        total_characters += length * analyzer->words[i].count;

        if (length > longest_length) {
            longest_length = length;
            longest = analyzer->words[i].name;
        }

        if (analyzer->words[i].count >
            analyzer->words[most_index].count) {
            most_index = i;
        }
    }

    average = (double)total_characters /
              (double)analyzer->total_words;

    printf("\n========== TEXT SUMMARY ==========\n");
    printf("Source: %s\n", analyzer->source_title);
    printf("Author/publication: %s\n", analyzer->source_author);
    printf("Total words: %zu\n", analyzer->total_words);
    printf("Distinct words: %zu\n", analyzer->word_count);
    printf("Average word length: %.2f characters\n", average);
    printf("Longest word: %s (%zu characters)\n",
           longest, longest_length);
    printf("Most frequent word: %s (%zu times)\n",
           analyzer->words[most_index].name,
           analyzer->words[most_index].count);
    printf("Lexical diversity: %.2f%%\n",
           (double)analyzer->word_count /
           (double)analyzer->total_words * 100.0);
}

static void lexical_diversity_analysis(TextAnalyzer *analyzer)
{
    double score;

    if (analyzer->total_words == 0) {
        printf("No text available for lexical diversity analysis.\n");
        return;
    }

    score = (double)analyzer->word_count /
            (double)analyzer->total_words * 100.0;

    printf("\n--- LEXICAL DIVERSITY ANALYSIS ---\n");
    printf("Distinct words: %zu\n", analyzer->word_count);
    printf("Total words: %zu\n", analyzer->total_words);
    printf("Lexical Diversity Score: %.2f%%\n", score);
    printf("Formula: (Distinct words / Total words) * 100\n");
    printf("A higher score means a larger proportion of the text "
           "uses different words.\n");
}

static void inspect_memory(TextAnalyzer *analyzer)
{
    printf("\n--- MEMORY ADDRESS INSPECTION ---\n");
    printf("Text buffer address: %p\n", (void *)analyzer->text);
    printf("Word statistics array address: %p\n",
           (void *)analyzer->words);

    if (analyzer->word_count > 0) {
        printf("Selected word-stat entry address: %p\n",
               (void *)&analyzer->words[0]);
    } else {
        printf("Selected word-stat entry: not available (no words)\n");
    }
}

static void show_menu(TextAnalyzer *analyzer)
{
    (void)analyzer;

    printf("\n========== TEXT ANALYZER MENU ==========\n");
    printf("1. Count total words\n");
    printf("2. Find longest word\n");
    printf("3. Find most frequent word\n");
    printf("4. Display word statistics\n");
    printf("5. Sort word statistics\n");
    printf("6. Search for a word\n");
    printf("7. Generate text summary\n");
    printf("8. Run lexical diversity analysis\n");
    printf("9. Inspect memory addresses\n");
    printf("10. Exit\n");
}

static void free_analyzer(TextAnalyzer *analyzer)
{
    free(analyzer->text);
    free(analyzer->words);

    analyzer->text = NULL;
    analyzer->words = NULL;
    analyzer->text_length = 0;
    analyzer->word_count = 0;
    analyzer->word_capacity = 0;
    analyzer->total_words = 0;
}
