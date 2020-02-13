#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "../src/hash_table.h"

struct hash_table_data
{
    char *key;
    int value;
    bool deleted;
};

char *extract_hash_table_data_key(struct hash_table_data *data)
{
    return data->key;
}

bool hash_table_data_is_deleted(struct hash_table_data *data)
{
    return data->deleted;
}

void delete_hash_table_data(struct hash_table_data *data)
{
    data->deleted = true;
}

void free_hash_table_data(struct hash_table_data *data)
{
    free(data);
}

void *malloc_or_die(size_t size)
{
    void *memory = malloc(size);
    if (memory == NULL)
    {
        perror("malloc failed for hash_table");
        exit(EXIT_FAILURE);
    }
    return memory;
}

int main()
{
    struct hash_table *table = create_hash_table(10);
    struct hash_table_data *me = malloc_or_die(sizeof(struct hash_table_data));
    me->key = "norris";
    me->value = 99;
    hash_table_insert(table, me->key, me);
    struct hash_table_data *bro = malloc_or_die(sizeof(struct hash_table_data));
    bro->key = "jab";
    bro->value = 55;
    hash_table_insert(table, bro->key, bro);
    printf("Count is %zu\n", table->count);

    struct hash_table_data *item = hash_table_get(table, "norris");
    if (item != NULL)
    {
        printf("Found and value is %d\n", item->value);
    }

    struct hash_table_data *item2 = hash_table_get(table, "jab");
    if (item2 != NULL)
    {
        printf("Found and value is %d\n", item2->value);
    }

    struct hash_table_data *item3 = hash_table_get(table, "aminu");
    if (item3 == NULL)
    {
        printf("aminu is not set in hash_table\n");
    }
    hash_table_remove(table, "jab");

    printf("Count is %zu\n", table->count);

    struct hash_table_data *item4 = hash_table_get(table, "jab");
    if (item4 == NULL)
    {
        printf("jab has been deleted\n");
    }

    free_hash_table(table);
}