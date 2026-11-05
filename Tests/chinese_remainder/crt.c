#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct vector_t vector_t;
typedef struct chinese_remainder_t chinese_remainder_t;

static int is_pairwise_coprime(const mpz_t *, size_t, const mpz_t);

vector_t *vector_init(size_t, const mpz_t *);
vector_t *vector_rand_init(size_t, long, long);
vector_t *vector_rand_coprime_init(size_t n, unsigned long, unsigned long);
void vector_free(vector_t *);

chinese_remainder_t *crt_init(vector_t *, vector_t *);
void crt_free(chinese_remainder_t *);
void crt_print(const chinese_remainder_t *);
void crt_reduce(chinese_remainder_t *);
int crt_solve(const chinese_remainder_t *, mpz_t);
void crt_random(const size_t, const unsigned long);
void crt_encrypt(const char *, const char *, mpz_t *);
void crt_decypt(const mpz_t *, const char *, mpz_t *);

struct vector_t {
    size_t argc;
    mpz_t *argv;
};

struct chinese_remainder_t {
    vector_t *moduli;
    vector_t *residue;
    mpz_t M;
};

static int is_pairwise_coprime(const mpz_t *argv, size_t count, const mpz_t cand) {
    for (size_t i = 0; i < count; ++i) {
        mpz_t g;
        mpz_init(g);
        mpz_gcd(g, argv[i], cand);
        int ok = mpz_cmp_ui(g, 1) == 0;
        mpz_clear(g);
        if (!ok) return 0;
    }
    return 1;
}

vector_t *vector_init(size_t argc, const mpz_t *argv) {
    if (argc == 0 || argv == NULL) return NULL;

    vector_t *v = malloc(sizeof(*v));
    if (!v) return NULL;

    v->argc = argc;
    v->argv = malloc(sizeof(mpz_t) * argc);
    if (!v->argv) { free(v); return NULL; }

    for (size_t i = 0; i < argc; ++i)
        mpz_init_set(v->argv[i], argv[i]);

    return v;
}

vector_t *vector_rand_init(size_t n, long start, long end) {
    if (n == 0 || end <= start) return NULL;

    mpz_t *tmp = malloc(sizeof(mpz_t) * n);
    if (!tmp) return NULL;

    for (size_t i = 0; i < n; ++i) {
        long r = start + random() % (end - start + 1);
        mpz_init_set_si(tmp[i], r);
    }

    vector_t *v = vector_init(n, tmp);

    for (size_t i = 0; i < n; ++i) mpz_clear(tmp[i]);
    free(tmp);
    return v;
}

vector_t *vector_rand_coprime_init(size_t n, unsigned long start, unsigned long end) {
    if (n == 0 || end <= start || start < 1) return NULL;

    unsigned long span = end - start + 1;
    mpz_t *tmp = malloc(sizeof(mpz_t) * n);
    if (!tmp) return NULL;

    const size_t MAX_ATTEMPTS = 100000;
    size_t count = 0, i = 0;
    while (i < n) {
        if (++count > MAX_ATTEMPTS) {
            for (size_t k = 0; k < i; ++k) mpz_clear(tmp[k]);
            free(tmp);
            return NULL;
        }
        unsigned long r = start + (unsigned long)random() % span;
        mpz_t cand; mpz_init_set_ui(cand, r);
        if (is_coprime_with(tmp, i, cand))
            mpz_init_set(tmp[i++], cand);
        mpz_clear(cand);
    }

    vector_t *v = vector_init(n, tmp);
    for (size_t k = 0; k < n; ++k) mpz_clear(tmp[k]);
    free(tmp);
    return v;
}

void vector_free(vector_t *v) {
    if (!v) return;
    for (size_t i = 0; i < v->argc; ++i)
        mpz_clear(v->argv[i]);
    free(v->argv);
    free(v);
}

chinese_remainder_t *crt_init(vector_t *moduli, vector_t *residue) {
    if (!moduli || !residue) return NULL;
    if (moduli->argc == 0 || moduli->argc != residue->argc) return NULL;

    chinese_remainder_t *crt = malloc(sizeof(*crt));
    if (!crt) return NULL;

    crt->moduli = moduli;
    crt->residue = residue;
    mpz_init_set_ui(crt->M, 1);

    for (size_t i = 0; i < moduli->argc; ++i) {
        if (mpz_cmp_ui(moduli->argv[i], 1) <= 0) {
            mpz_clear(crt->M);
            free(crt);
            return NULL;
        }
        mpz_mul(crt->M, crt->M, moduli->argv[i]);
    }
    return crt;
}

void crt_free(chinese_remainder_t *crt) {
    if (!crt) return;
    mpz_clear(crt->M);
    free(crt);
}

void crt_print(const chinese_remainder_t *crt) {
    if (!crt || !crt->residue || !crt->moduli) {
        puts("Error: CRT not initialized");
        return;
    }
    if (crt->residue->argc == 0 ||
        crt->residue->argc != crt->moduli->argc) {
        puts("Error: moduli/residues size mismatch");
        return;
    }
    for (size_t i = 0; i < crt->residue->argc; ++i) {
        gmp_printf("x === %Zd (mod %Zd)\n",
            crt->residue->argv[i], crt->moduli->argv[i]);
    }
}

void crt_reduce(chinese_remainder_t *crt) {
    if (!crt || !crt->residue || !crt->moduli ||
        crt->residue->argc != crt->moduli->argc) return;

    for (size_t i = 0; i < crt->residue->argc; ++i) {
        if (mpz_sgn(crt->moduli->argv[i]) <= 0) continue;
        mpz_mod(crt->residue->argv[i],
            crt->residue->argv[i],
            crt->moduli->argv[i]);
    }
}

int crt_solve(const chinese_remainder_t *crt, mpz_t x) {
    if (!crt || !crt->moduli || !crt->residue ||
        crt->moduli->argc != crt->residue->argc ||
        crt->moduli->argc == 0) {
        puts("Error: CRT not initialized");
        return 0;
    }

    const size_t n = crt->moduli->argc;
    mpz_t sum, Mi, inv, term;
    mpz_inits(sum, Mi, inv, term, NULL);
    mpz_set_ui(sum, 0);

    for (size_t i = 0; i < n; ++i) {
        const __mpz_struct *mi = (__mpz_struct *)&crt->moduli->argv[i];
        if (mpz_sgn(mi) <= 0) {
            puts("Error: non-positive modulus");
            goto fail;
        }
        mpz_divexact(Mi, crt->M, mi);
        if (mpz_invert(inv, Mi, mi) == 0) {
            puts("Error: moduli not pairwise coprime");
            goto fail;
        }
        mpz_mul(term, crt->residue->argv[i], Mi);
        mpz_mul(term, term, inv);
        mpz_add(sum, sum, term);
    }

    mpz_mod(x, sum, crt->M);

    mpz_clears(sum, Mi, inv, term, NULL);
    return 1;

fail:
    mpz_clears(sum, Mi, inv, term, NULL);
    return 0;
}

void crt_random(const size_t n_, const unsigned long max_) {
    const size_t n = n_;
    const unsigned long max = max_;

    vector_t *residue = vector_rand_init(n, -(long)max, (long)max);
    if (!residue) { fputs("residue init failed\n", stderr); return; }

    vector_t *moduli = vector_rand_coprime_init(n, 2, max);
    if (!moduli) {
        fputs("moduli init failed\n", stderr);
        vector_free(residue); return;
    }

    chinese_remainder_t *crt = crt_init(moduli, residue);
    if (!crt) {
        fputs("crt init failed\n", stderr);
        vector_free(moduli); vector_free(residue); return;
    }

    crt_reduce(crt);
    crt_print(crt);

    mpz_t x; mpz_init(x);
    if (crt_solve(crt, x))
        gmp_printf("\nx = %Zd (mod %Zd)\n", x, crt->M);
    mpz_clear(x);

    crt_free(crt);
    vector_free(moduli);
    vector_free(residue);
}

void crt_encrypt(const char *str_hash, const char *key, mpz_t *output) {
    const size_t L_str = strlen(str_hash);
    const size_t L_key = strlen(key);

    if (L_str != L_key) {
        printf("%lu != %lu\n", L_str, L_key);
        return;
    }
    const size_t LENGTH = L_str;

    mpz_t *mpz_str_hash = malloc(LENGTH * sizeof(mpz_t));
    mpz_t *mpz_key = malloc(LENGTH * sizeof(mpz_t));
    if (!mpz_str_hash || !mpz_key) return;

    for (size_t i = 0; i < LENGTH; ++i) {
        mpz_init(mpz_str_hash[i]);
        mpz_init(mpz_key[i]);

        uint64_t ascii_str = (uint64_t)(unsigned char)str_hash[i];
        uint64_t ascii_key = (uint64_t)(unsigned char)key[i];

        mpz_set_ui(mpz_str_hash[i], ascii_str);
        mpz_set_ui(mpz_key[i], ascii_key);
    }

    printf("Original String: %s\n", str_hash);
    for (size_t i = 0; i < LENGTH; ++i) {
        gmp_printf("'%c' -> %Zd\n", str_hash[i], mpz_str_hash[i]);
    }

    printf("\nOriginal Key: %s\n", key);
    for (size_t i = 0; i < LENGTH; ++i) {
        gmp_printf("'%c' -> %Zd\n", key[i], mpz_key[i]);
    }

    vector_t *moduli = vector_init(30, (const mpz_t *)mpz_key);
    vector_t *residue = vector_init(30, (const mpz_t *)mpz_str_hash);

    chinese_remainder_t *crt = crt_init(moduli, residue);
    if (!crt) {
        fputs("crt init failed\n", stderr);
        vector_free(moduli); vector_free(residue); return;
    }

    crt_print(crt);

    mpz_t x; mpz_init(x);
    if (crt_solve(crt, x))
        gmp_printf("\nx = %Zd (mod %Zd)\n", x, crt->M);
    mpz_set(output, x);
    mpz_clear(x);

    for (size_t i = 0; i < LENGTH; ++i) {
        mpz_clear(mpz_str_hash[i]);
        mpz_clear(mpz_key[i]);
    }
    free(mpz_str_hash);
    free(mpz_key);

    crt_free(crt);
    vector_free(moduli);
    vector_free(residue);
}

void crt_decypt(const mpz_t *hash, const char *key, mpz_t *output) {
    // TODO
}

int main(void) {
    srandom((unsigned int)time(NULL));

    const char str_hash[] = "MYNAMEISAUSTINBITCHHHHHWHOTFRU";
    const char key[] = "\x61\x65\x67\x6b\x7f\x02\x03\x89\x05\x07\x0b\x0d\x6d\x71\x11\x35\x3b\x43\x13\x17\x1d\x1f\x25\x2b\x2f\x47\x49\x4f\x53\x3d";
    
    mpz_t x; mpz_init(x);
    crt_encrypt(str_hash, key, x);
    mpz_clear(x);

    return 0;
}