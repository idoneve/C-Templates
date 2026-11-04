#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct vector_t vector_t;
typedef struct chinese_remainder_t chinese_remainder_t;

uint64_t egcd(uint64_t, uint64_t, int64_t *);
uint64_t gcd_uint64(const uint64_t, const uint64_t);
uint8_t is_coprime(const size_t, const uint64_t *, const uint64_t);

vector_t *vector_init(const size_t, int64_t *);
vector_t *vector_rand_init(const size_t, int64_t, int64_t);
vector_t *vector_rand_coprime_init(const size_t n, const uint64_t start, const uint64_t end);
void vector_free(vector_t *);

chinese_remainder_t *crt_init(vector_t *, vector_t *);
void crt_free(chinese_remainder_t *);
void crt_print(const chinese_remainder_t *);
void crt_reduce(chinese_remainder_t *);
uint64_t crt_solve(const chinese_remainder_t *);

struct vector_t {
    size_t nargs;
    int64_t *args;
};

struct chinese_remainder_t {
    vector_t *moduli;
    vector_t *residue;
    uint64_t M;
};

uint64_t egcd(uint64_t a, uint64_t m, int64_t *inv) {
    int64_t old_r = (int64_t)a, r = (int64_t)m;
    int64_t old_s = 1, s = 0;

    while (r != 0) {
        int64_t q = old_r / r;
        int64_t t;

        t = old_r - q * r; old_r = r; r = t;
        t = old_s - q * s; old_s = s; s = t;
    }

    if (old_r != 1) return 0;

    int64_t inv_ = old_s % (int64_t)m;
    if (inv_ < 0) inv_ += (int64_t)m;
    *inv = inv_;

    return 1;
}

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

vector_t *vector_init(const size_t nargs, int64_t *args) {
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

    vector_t *vector = vector_init(n, (int64_t *)args);

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
    if (moduli->nargs == 0 || moduli->nargs != residue->nargs) return NULL;

    chinese_remainder_t *crt = malloc(sizeof(*crt));
    if (!crt) return NULL;

    crt->moduli = moduli;
    crt->residue = residue;

    uint64_t M = 1;
    for (size_t i = 0; i < moduli->nargs; ++i) {
        int64_t m = moduli->args[i];
        if (m <= 1) { free(crt); return NULL; }
        if (M > UINT64_MAX / (uint64_t)m) { free(crt); return NULL; }
        M *= (uint64_t)m;
    }
    crt->M = M;
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

void crt_reduce(chinese_remainder_t *crt) {
    if (!crt || !crt->residue || !crt->moduli || crt->residue->nargs != crt->moduli->nargs) return;

    for (size_t i = 0; i < crt->residue->nargs; ++i) {
        int64_t m = crt->moduli->args[i];
        if (m <= 0) continue;

        int64_t r = crt->residue->args[i] % m;
        if (r < 0) r += m;

        crt->residue->args[i] = r;
    }
}

uint64_t crt_solve(const chinese_remainder_t *crt) {
    if (!crt || !crt->moduli || !crt->residue ||
        crt->moduli->nargs != crt->residue->nargs ||
        crt->moduli->nargs == 0) {
        puts("Error: CRT not initialized");
        return 0;
    }

    __int128 sum = 0;
    const size_t n = crt->moduli->nargs;

    for (size_t i = 0; i < n; ++i) {
        int64_t mi = crt->moduli->args[i];
        if (mi <= 0) {
            puts("Error: non-positive modulus");
            return 0;
        }
        uint64_t Mi = crt->M / (uint64_t)mi;
        int64_t inv;
        if (!egcd(Mi, (uint64_t)mi, &inv)) {
            puts("Error: moduli not pairwise coprime");
            return 0;
        }
        sum += (__int128)crt->residue->args[i] * (int64_t)Mi * inv;
    }

    __int128 M = (__int128)crt->M;
    __int128 x = ((sum % M) + M) % M;

    printf("x = %" PRId64 " (mod %" PRId64 ")\n", (int64_t)x, crt->M);
    return (uint64_t)x;
}

// TODO: switch to bignum library
int main(void) {
    srandom((unsigned int)time(NULL));

    const size_t n = 5;

    vector_t *residue = vector_rand_init(n, 0, 99);
    if (!residue) { fputs("residue init failed\n", stderr); return 1; }

    vector_t *moduli = vector_rand_coprime_init(n, 2, 17);
    if (!moduli) { fputs("moduli init failed\n", stderr); vector_free(residue); return 1; }

    chinese_remainder_t *crt = crt_init(moduli, residue);
    if (!crt) { fputs("crt init failed\n", stderr); vector_free(moduli); vector_free(residue); return 1; }

    crt_reduce(crt);
    crt_print(crt);
    crt_solve(crt);

    crt_free(crt);
    vector_free(moduli);
    vector_free(residue);

    return 0;
}