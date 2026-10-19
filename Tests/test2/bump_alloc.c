#define HEAP_SIZE 32

static char heap[HEAP_SIZE];
static unsigned long heap_idx = 0;

typedef struct {
    char *_string;
    unsigned long _size;
} data_t;

void *heap_alloc(unsigned long size) {
    if (heap_idx + size > HEAP_SIZE) {
        return 0;
    }

    void *ptr_to_heap = &heap[heap_idx];
    heap_idx += size;

    return ptr_to_heap;
}

void heap_clean(void) {
    heap_idx = 0;
}

data_t *create_data(char *string, unsigned long size) {
    data_t *data = heap_alloc(sizeof(data_t));
    if (!data) {
        return 0;
    }

    char *heap_string = heap_alloc(size);
    if (!heap_string) {
        return 0;
    }

    for (unsigned long i = 0; i < size; ++i) {
        heap_string[i] = string[i];
    }

    data->_string = heap_string;
    data->_size = size;

    return data;
}

data_t *add_data_char(data_t *data, unsigned long idx, char c) {
    if (idx >= HEAP_SIZE) {
        return 0;
    }

    if (idx >= data->_size) {
        char *heap_string = heap_alloc(idx - data->_size);
        if (!heap_string) {
            return 0;
        }

        for (unsigned long i = 0; i < data->_size; ++i) {
            heap_string[i] = data->_string[i];
        }

        heap_string[idx] = c;

        data->_string = heap_string;
        data->_size = idx + 1;
    } else {
        data->_string[idx] = c;
    }

    return data;
}

data_t *add_data(data_t *data, unsigned long start_idx, char *string, unsigned long size) {
    if (start_idx + size >= HEAP_SIZE) {
        return 0;
    }

    for (int i = 0; i < size; ++i) {
        data_t *new_data = add_data_char(data, start_idx + i, string[i]);
        if (!new_data) {
            return 0;
        }

        data = new_data;
    }

    return data;
}

int test1(void) {
    unsigned long buf1_size = 5;
    char buf1[] = { 'h', 'e', 'l', 'l', 'o' };

    unsigned long buf2_size = 5;
    char buf2[] = { 'w', 'o', 'r', 'l', 'd' };

    data_t *data1 = create_data(buf1, buf1_size);
    if (!data1) {
        return 1;
    }

    data_t *data2 = create_data(buf2, buf2_size);
    if (!data2) {
        return 1;
    }

    data1 = add_data(data1, buf1_size, buf2, buf2_size);
    if (!data1) {
        return 1;
    }

    return 0;
}

int main(void) {
    if (!test1()) {
        return 1;
    }
    
    return 0;
}