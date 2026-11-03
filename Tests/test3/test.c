#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct vector_t vector_t;
typedef struct chinese_remainder_t chinese_remainder_t;

uint64_t gcd_uint64(const uint64_t, const uint64_t);
uint8_t is_coprime(const size_t, const uint64_t *, const uint64_t);

vector_t *vector_init(const size_t, const int64_t *);
vector_t *vector_rand_init(const size_t, int64_t, int64_t);
vector_t *vector_rand_coprime_init(const size_t n, const uint64_t start, const uint64_t end);
void vector_free(vector_t *);

chinese_remainder_t *crt_init(vector_t *, vector_t *);
void crt_free(chinese_remainder_t *);
void crt_print(const chinese_remainder_t *);

struct vector_t {
    size_t nargs;
    int64_t *args;
};

struct chinese_remainder_t {
    vector_t *moduli;
    vector_t *residue;
};

uint64_t gcd_uint64(uint64_t a, uint64_t b) {
    while (b) {
        uint64_t t = a % b;
        a = b;
        b = t;
    }

    return a;
}

uint8_t is_coprime(const size_t nargs, const uint64_t *args, const uint64_t n) {
    if (n == 0) return 0;

    for (size_t i = 0; i < nargs; ++i) {
        if (gcd_uint64(args[i], n) != 1) return 0;
    }

    return 1;
}

vector_t *vector_init(const size_t nargs, const int64_t *args) {
    if (nargs == 0 || args == NULL) return NULL;

    vector_t *vector = malloc(sizeof(*vector));
    if (!vector) return NULL;

    vector->nargs = nargs;
    vector->args = malloc(sizeof(int64_t) * nargs);
    if (!vector->args) {
        free(vector);
        return NULL;
    }

    memcpy(vector->args, args, sizeof(int64_t) * nargs);

    return vector;
}

vector_t *vector_rand_init(const size_t n, const int64_t start, const int64_t end) {
    if (n == 0 || end <= start) return NULL;

    int64_t *args = (int64_t *)malloc(sizeof(int64_t) * n);
    for (size_t i = 0; i < n; ++i) {
        args[i] = start + random() % (end - start + 1);
    }

    vector_t *vector = vector_init(n, args);

    free(args);

    return vector;
}

vector_t *vector_rand_coprime_init(const size_t n, const uint64_t start, const uint64_t end) {
    if (n == 0 || end <= start || start < 1) return NULL;

    uint64_t span = end - start + 1;

    uint64_t *args = calloc(n, sizeof(*args));
    if (!args) return NULL;

    const size_t MAX_ATTEMPTS = 10000;
    size_t count = 0;
    size_t i = 0;
    while (i < n) {
        if (++count > MAX_ATTEMPTS) {
            free(args);
            args = NULL;

            return NULL;
        }

        uint64_t cand = start + (uint64_t)random() % span;
        if (is_coprime(i, args, cand)) {
            args[i++] = cand;
        }
    }

    vector_t *vector = vector_init(n, args);

    free(args);

    return vector;
}

void vector_free(vector_t *vector) {
    if (!vector) return;

    free(vector->args);
    free(vector);
}

chinese_remainder_t *crt_init(vector_t *moduli, vector_t *residue) {
    if (!moduli || !residue) return NULL;

    chinese_remainder_t *crt = malloc(sizeof(*crt));
    if (crt == NULL) return NULL;

    crt->moduli = moduli;
    crt->residue = residue;

    return crt;
}

void crt_free(chinese_remainder_t *crt) {
    if (crt == NULL) return;

    free(crt);
}

void crt_print(const chinese_remainder_t *crt) {
    if (!crt || !crt->residue || !crt->moduli) {
        puts("Error: CRT not initialized");
        return;
    }
    if (crt->residue->nargs == 0 || crt->moduli->nargs == 0 ||
        crt->residue->nargs != crt->moduli->nargs) {
        puts("Error: moduli/residues size mismatch");
        return;
    }
    for (size_t i = 0; i < crt->residue->nargs; ++i) {
        printf("x === %" PRId64 " (mod %" PRId64 ")\n",
            crt->residue->args[i], crt->moduli->args[i]);
    }
}

int main(void) {
    srandom((unsigned int)time(NULL));

    const size_t n = 10;

    vector_t *residue = vector_rand_init(n, 0, 999);
    if (!residue) { fputs("residue init failed\n", stderr); return 1; }

    vector_t *moduli = vector_rand_coprime_init(n, 2, 1000);
    if (!moduli) { fputs("moduli init failed\n", stderr); vector_free(residue); return 1; }

    chinese_remainder_t *crt = crt_init(moduli, residue);
    if (!crt) { fputs("crt init failed\n", stderr); vector_free(moduli); vector_free(residue); return 1; }

    crt_print(crt);

    crt_free(crt);
    vector_free(moduli);
    vector_free(residue);

    return 0;
}