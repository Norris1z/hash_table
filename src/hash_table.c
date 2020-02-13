#include "hash_table.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

struct hash_table *create_hash_table(size_t default_size)
{
    struct hash_table *table = malloc(sizeof(struct hash_table));
    if (table == NULL)
    {
        perror("malloc failed for hash_table");
        exit(EXIT_FAILURE);
    }
    table->size = default_size;
    table->count = 0;
    table->data = calloc(default_size, sizeof(struct hash_table_data *));
    if (table->data == NULL)
    {
        perror("calloc failed for hash_table data");
        exit(EXIT_FAILURE);
    }
    return table;
}

void free_hash_table(struct hash_table *ht)
{
    for (size_t i = 0; i < ht->size; i++)
    {
        struct hash_table_data *data = ht->data[i];
        if (data != NULL)
        {
            free_hash_table_data(data);
        }
    }
    free(ht->data);
    free(ht);
}

static size_t hash(char *key, const int large_prime_number, const size_t bucket_size)
{
    size_t hash = 0;
    size_t key_len = strlen(key);
    for (size_t i = 0; i < key_len; i++)
    {
        hash += pow(large_prime_number, key_len - (i + 1)) * key[i];
        hash = hash % bucket_size;
    }
    return hash;
}

static size_t get_hash(char *key, const size_t bucket_size, const int attempts)
{
    int hash1 = hash(key, 6829, bucket_size);
    int hash2 = hash(key, 383, bucket_size);
    return (hash1 + attempts * (hash2 == 0 ? 1 : hash2)) % bucket_size;
}

static int is_prime(const int number)
{
    if (number < 2)
    {
        return -1;
    }
    if (number < 4)
    {
        return 1;
    }
    if ((number % 2) == 0)
    {
        return 0;
    }
    for (int i = 3; i <= floor(sqrt((double)number)); i += 2)
    {
        if ((number % i) == 0)
        {
            return 0;
        }
    }
    return 1;
}

static int next_prime(int number)
{
    while (is_prime(number) != 1)
    {
        number++;
    }
    return number;
}

static void resize_hash_table(struct hash_table *ht, const size_t size)
{
    struct hash_table *new_hash_table = create_hash_table(size);
    new_hash_table->size = next_prime(size);
    for (size_t i = 0; i < ht->size; i++)
    {
        struct hash_table_data *data = ht->data[i];
        if (data != NULL && !hash_table_data_is_deleted(data))
        {
            hash_table_insert(new_hash_table, extract_hash_table_data_key(data), data);
        }
    }

    ht->count = new_hash_table->count;

    const int current_table_size = ht->size;
    ht->size = new_hash_table->size;
    new_hash_table->size = current_table_size;

    struct hash_table_data **table_data = ht->data;
    ht->data = new_hash_table->data;
    new_hash_table->data = table_data;
    //we have pointed the contents of new_hash_table to ht and we are freeing the old ht now pointed by new_hash_table
    free_hash_table(new_hash_table);
}

static void resize_hash_table_up(struct hash_table *ht)
{
    resize_hash_table(ht, ht->size * 2);
}

static void resize_hash_table_down(struct hash_table *ht)
{
    resize_hash_table(ht, ht->size / 2);
}

void hash_table_insert(struct hash_table *ht, char *key, struct hash_table_data *data)
{
    const int load_ratio = ht->count * 100 / ht->size;
    if (load_ratio > 70) //resize up if load ratio  > 0.7
    {
        resize_hash_table_up(ht);
    }

    size_t index = get_hash(extract_hash_table_data_key(data), ht->size, 0);
    struct hash_table_data *existing_data = ht->data[index];
    size_t i = 1;
    while (existing_data != NULL)
    {
        if (hash_table_data_is_deleted(existing_data) || strcmp(extract_hash_table_data_key(data), key) == 0)
        {
            free_hash_table_data(existing_data);
            break;
        }
        else
        {
            index = get_hash(extract_hash_table_data_key(data), ht->size, i);
            existing_data = ht->data[index];
            i++;
        }
    }
    ht->data[index] = data;
    ht->count++;
}

struct hash_table_data *hash_table_get(struct hash_table *ht, char *key)
{
    size_t index = get_hash(key, ht->size, 0);
    struct hash_table_data *data = ht->data[index];
    size_t i = 1;
    while ((data != NULL) && (i < ht->size))
    {
        if (!hash_table_data_is_deleted(data) && strcmp(extract_hash_table_data_key(data), key) == 0)
        {
            return data;
        }
        index = get_hash(key, ht->size, i);
        data = ht->data[index];
        i++;
    }
    return NULL;
}

void hash_table_remove(struct hash_table *ht, char *key)
{
    const int load_ratio = ht->count * 100 / ht->size;
    if (load_ratio < 10) //resize down if load ration < 0.1
    {
        resize_hash_table_down(ht);
    }

    size_t index = get_hash(key, ht->size, 0);
    struct hash_table_data *data = ht->data[index];
    size_t i = 1;
    while (data != NULL)
    {
        if (!hash_table_data_is_deleted(data))
        {
            if (strcmp(extract_hash_table_data_key(data), key) == 0)
            {
                delete_hash_table_data(data);
                ht->count--;
                break;
            }
        }
        index = get_hash(key, ht->size, i);
        data = ht->data[index];
        i++;
    }
}
