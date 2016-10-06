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

extern int re2c(const char *YYCURSOR, size_t dummy);
extern int re2ci(const char *YYCURSOR, size_t dummy);
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

#define RUN_FUNCTION(name, fun, notfound) { \
	double a = gettime(); \
	for (loop = 0; loop < max; loop++) \
		fun(w, l); \
	double b = gettime(); \
	names[id] = name; \
	printf("        \"%s\": {\"matched\": \"%c\", \"time\": %f},\n", name, fun(w, l) == notfound ? '!' : 'X', (b-a) / loop); \
	totals[id] += (b - a); \
	fflush(stdout); \
	id++; \
}

int main(int argc, char **argv)
{
#ifdef ITERS
    long long max = argc > 1 ? strtoll(argv[1], NULL, 0) : ITERS;
#else
    long long max = strtoll(argv[1], NULL, 0);
#endif

    char *names[128] = {0};
    double totals[128] = { 0 };
    size_t bytes = 0;

#ifdef WORDS
    char words[] = WORDS;
    char *w;
    printf("{\n");
    printf("\"results\": {\n");
    for (w = strtok(words, " "); w != NULL; w = strtok(NULL, " ")) {
#else
    for (int i = 2; i < argc; i++) {
        const char *w = argv[i];
#endif
        size_t l = strlen(w);
	//fprintf(stderr, "%-100s\r", w);
	printf("    \"%s\": {\n", w);
	bytes += l;
        long long loop = 0;
	unsigned int id = 0;

        RUN_FUNCTION("Trie", Trie, -1);
        RUN_FUNCTION("Trie (*)", TrieCase, -1);
	if (getenv("ONLY_TRIE") == NULL) {
	    RUN_FUNCTION("re2c", re2c, -1);
	    RUN_FUNCTION("re2c (*)", re2ci, -1);
	    RUN_FUNCTION("gperf (*)", GPerfCase, NULL);
	    RUN_FUNCTION("gperf", GPerf, NULL);
	    RUN_FUNCTION("djb (*)", DjbCase, 5381);
	    RUN_FUNCTION("djb (**)", DjbCase2, 5381);
	    RUN_FUNCTION("djb", DjbHash, 5381);
	    RUN_FUNCTION("apt (*)", AlphaHash, 0);
	    RUN_FUNCTION("apt (**)", AlphaHash2, 0);
	}
	printf("        \"_\": 0\n");
	printf("   },\n");

    }
    //fprintf(stderr, "%-100s\n", "");
    printf("    \"_\": 0\n");
    printf("},\n");
    printf("\"summary\": {\n");


    for (int i = 0; names[i] != 0; i++) {
	printf("    \"%s\": {", names[i]);
        printf("        \"total\": %9.0f,", totals[i] / max);
        printf("        \"ns_per_byte\": %9.4f,", totals[i] / (max * bytes));
        printf("        \"bytes_per_ns\": %9.4f", max * bytes / totals[i]);
	printf("    },\n");
    }
    printf("    \"_\": 0");
    printf("}\n");
    printf("}\n");
    

    
    return 0;
}
