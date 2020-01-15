/* test C Language api */

#include <stdio.h>
#include <stdlib.h>

typedef struct hash_node_s {
    int key;
    int data; /* usually, we save the data by a pointer. */
    struct hash_node_s *next;
} hash_node_t;

typedef struct hash_table_s {
    struct hash_node_s **hash_sets;
    int n; /* hash table count */
} hash_table_t;

static hash_table_t *hash_create(int n)
{
    hash_table_t *tbl;

    tbl = calloc(1, sizeof(hash_table_t));
    if (tbl == NULL) {
        return NULL;
    }
    tbl->hash_sets = calloc(n, sizeof(hash_node_t *));
    if (tbl->hash_sets == NULL) {
        free(tbl);
        return NULL;
    }
    tbl->n = n;

    return tbl;
}

static int hash_key(hash_table_t *tbl, int key)
{
    int index;
    
    index = key % tbl->n;
    if (index < 0) {
        index += tbl->n;
    }
    
    return index;
}

static int hash_insert(hash_table_t *tbl, int key, int data)
{
    hash_node_t *bucket_head, *entry;
    int index;

    /* usually we need a hash algorithm for generate index. */
    index = hash_key(tbl, key); 
    bucket_head = tbl->hash_sets[index];
    entry = calloc(1, sizeof(hash_node_t));
    if (entry == NULL) {
        return -1;
    }
    entry->data = data;
    entry->key = key;
    entry->next = bucket_head;
    tbl->hash_sets[index] = entry;

    return 0;
}

static hash_node_t *hash_find(hash_table_t *tbl, int key)
{
    hash_node_t *bucket_head, *entry;
    int index;

    index = hash_key(tbl, key); /* usually we need a hash algorithm for generate index. */
    bucket_head = tbl->hash_sets[index];
    entry = bucket_head;
    while (entry) {
        if (entry->key == key) {
            return entry;
        }
        entry = entry->next;
    }

    return NULL;
}

static int *sum(int *nums, int nums_size, int target, int *return_size)
{
    int i, tmp, *ret;
    hash_table_t *tbl;
    hash_node_t *node;

    tbl = hash_create(nums_size);
    if (tbl == NULL) {
        *return_size = 0;
        return NULL;
    }
    ret = calloc(1, sizeof(int) * 2);
    if (ret == NULL) {
        *return_size = 0;
        return NULL;
        
    }
    *return_size = 2;
    for (i = 0; i < nums_size; i++) {
        tmp = target - nums[i];
        node = hash_find(tbl, tmp);
        if (node != NULL) {
            ret[0] = node->data;
            ret[1] = i;
            return ret;
        }
        hash_insert(tbl, nums[i], i);
    }
    *return_size = 0;
    free(ret);

    return NULL;
}

int main(int argc, char *argv[])
{
    int arr[8] = {-3, 7, 11, 15, 16, 13, 9, 1};
    int *ret_index, size;

    ret_index = sum(arr, 8, -2, &size);
    printf("ret size: %d\r\n", size);
    for (int i = 0; i < size; i++) {
        printf("%d, ", ret_index[i]);
    }
    printf("\r\n");
    free(ret_index);

    return 0;
}
