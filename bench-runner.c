#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

extern int TrieCase(const char *s, size_t l);
extern int Trie(const char *s, size_t l);

extern const char *GPerf(const char *str, unsigned int len);
extern const char *GPerfCase(const char *str, unsigned int len);

extern unsigned int DjbHash(const char *s, size_t l);
extern unsigned int DjbCase(const char *s, size_t l);
extern unsigned int DjbCase2(const char *s, size_t l);
extern unsigned long AlphaHash(const char *Text, size_t Length);
extern unsigned long AlphaHash2(const char *Text, size_t Length);

double gettime()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000000.0 + ts.tv_nsec;
}

#define RUN_FUNCTION(id, fun, notfound) { \
	double a = gettime(); \
	for (loop = 0; loop < max; loop++) \
		fun(w, l); \
	double b = gettime(); \
	if (!getenv("SHORT")) \
		printf("|%5.0f (%c)", (b-a) / loop, fun(w, l) == notfound ? '!' : 'X'); \
	totals[id] += (b - a); \
	fflush(stdout); \
}

int main(int argc, char **argv)
{
    printf("%-30s", "word");
    printf("|%-9s", "Trie");
    printf("|%-9s", "TrieCase");
    printf("|%-9s", "GPerfCase");
    printf("|%-9s", "GPerf");
    printf("|%-9s", "DJBCase");
    printf("|%-9s", "DJBCase2");
    printf("|%-9s", "DJB");
    printf("|%-9s", "APTCase");
    printf("|%-9s", "APTCase2");
    printf
        ("\n------------------------------|---------|---------|---------|---------|---------|---------|---------|---------|----------\n");

#ifdef ITERS
    long long max = argc > 1 ? strtoll(argv[1], NULL, 0) : ITERS;
#else
    long long max = strtoll(argv[1], NULL, 0);
#endif

    double totals[12] = { 0 };
    size_t bytes = 0;

#ifdef WORDS
    char words[] = WORDS;
    char *w;
    for (w = strtok(words, " "); w != NULL; w = strtok(NULL, " ")) {
#else
    for (int i = 2; i < argc; i++) {
        const char *w = argv[i];
#endif
        size_t l = strlen(w);
	bytes += l;
        long long loop = 0;
        if (!getenv("SHORT"))
			printf("%-30s", w);

        RUN_FUNCTION(0, Trie, -1);
        RUN_FUNCTION(1, TrieCase, -1);
        RUN_FUNCTION(2, GPerfCase, NULL);
        RUN_FUNCTION(3, GPerf, NULL);
        RUN_FUNCTION(4, DjbCase, 5381);
        RUN_FUNCTION(5, DjbCase2, 5381);
        RUN_FUNCTION(6, DjbHash, 5381);
        RUN_FUNCTION(7, AlphaHash, 0);
        RUN_FUNCTION(8, AlphaHash2, 0);

		if (!getenv("SHORT"))
			printf("\n");
    }

	if (!getenv("SHORT"))
		printf("------------------------------|---------|---------|---------|---------|---------|---------|---------|---------|----------\n");

    printf("%-30s", "<total>");
    for (int i = 0; i < 9; i++) {
        printf("|%9.0f", totals[i] / max);
    }
    printf("\n");
    printf("%-30s", "nanosecond/byte");
    for (int i = 0; i < 9; i++) {
        printf("|%9.2f", totals[i] / (max * bytes));
    }
    printf("\n");
    printf("%-30s", "byte/nanosecond");
    for (int i = 0; i < 9; i++) {
        printf("|%9.2f", max * bytes / totals[i]);
    }
    printf("\n");

    if (!getenv("SHORT")) {
		printf("%-30s", "word");
		printf("|%-9s", "Trie");
		printf("|%-9s", "TrieCase");
		printf("|%-9s", "GPerfCase");
		printf("|%-9s", "GPerf");
		printf("|%-9s", "DJBCase");
		printf("|%-9s", "DJBCase2");
		printf("|%-9s", "DJB");
		printf("|%-9s", "APTCase");
		printf("|%-9s", "APTCase2");
	}
	printf("\n");
    return 0;
}
