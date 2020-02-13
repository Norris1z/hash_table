#ifndef __T_PHP_HASH_TABLE__
#define __T_PHP_HASH_TABLE__
#include <stddef.h>
#include <stdbool.h>

struct hash_table_data;

struct hash_table
{
    size_t size;
    size_t count;
    struct hash_table_data **data;
};

struct hash_table *create_hash_table(size_t default_size);

void free_hash_table(struct hash_table *ht);

void hash_table_insert(struct hash_table *ht, char *key, struct hash_table_data *data);

struct hash_table_data *hash_table_get(struct hash_table *ht, char *key);

void hash_table_remove(struct hash_table *ht, char *key);

/**
 * The functions below are to be implemented by the user
 * */
void free_hash_table_data(struct hash_table_data *data);

char *extract_hash_table_data_key(struct hash_table_data *data);

bool hash_table_data_is_deleted(struct hash_table_data *data);

//please dont call free in here. Set something like data->deleted = true;
void delete_hash_table_data(struct hash_table_data *data);

#endif //__T_PHP_HASH_TABLE__