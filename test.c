#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct HashNode {
    char *key;
    char *value;
    struct HashNode *next;
} HashNode;

unsigned long hash(char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash % 101;
}

HashNode* create_node(char *key, char *value) {
    HashNode *new_node = (HashNode*) malloc(sizeof(HashNode));
    new_node->key = key;
    new_node->value = value;
    new_node->next = NULL;
    return new_node;
}

void insert_hash_table(HashNode** hash_table, char *key, char *value) {
    unsigned long index = hash(key);
    HashNode *new_node = create_node(key, value);

    if (hash_table[index] == NULL) {
        hash_table[index] = new_node;
    } else {
        HashNode *current = hash_table[index];
        while (current->next != NULL)
            current = current->next;
        current->next = new_node;
    }
}

char* trim_whitespace(char *str) {
    int start = 0;
    int end = strlen(str) - 1;

    // Trim leading space
    while (isspace(str[start]))
        start++;

    // Trim trailing space
    while (isspace(str[end]))
        end--;

    // Shift the string and terminate it
    for (int i = start; i <= end; i++)
        str[i - start] = str[i];
    str[end - start + 1] = '\0';

    // Convert to lowercase
    for (int i = 0; str[i]; i++)
        str[i] = tolower(str[i]);

    return str;
}

int main() {
    FILE *file = fopen("data.csv", "r");
    if (!file) {
        printf("Error: Unable to open the CSV file.\n");
        return 1;
    }

    HashNode *hash_table[101] = {NULL};
    char line[1024];
    int row = 0;

    while (fgets(line, sizeof(line), file)) {
        row++;
        if (line[0] == '\n' || line[0] == '\r') {
            printf("Row %d skipped (empty row)\n", row);
            continue;
        }

        char *token = strtok(line, ",");
        if (!token) {
            printf("Row %d skipped (invalid format)\n", row);
            continue;
        }

        if (row == 1) {
            // Parse header row
            trim_whitespace(token);
            insert_hash_table(hash_table, token, NULL);
        } else {
            bool skip_row = false;

            // Parse data row
            HashNode *current_node = hash_table[hash(token)];
            char *key = current_node ? current_node->key : NULL;
            while (key && strcmp(key, token)) {
                current_node = current_node->next;
                if (!current_node) {
                    key = NULL;
                    break;
                }
                key = current_node->key;
            }

            if (!key || (strcmp(token, "dl") == 0 && (strcmp(trim_whitespace(strtok(NULL, ",")), "n") == 0 || strcmp(trim_whitespace(strtok(NULL, ",")), "no") == 0))) {
                skip_row = true;
            }

            if (!skip_row) {
                token = strtok(NULL, ",");
                if (!token) {
                    printf("Row %d skipped (invalid format)\n", row);
                    continue;
                }
                trim_whitespace(token);

                if (current_node->value) {
                    free(current_node->value);
                }
                current_node->value = token;
            }

            if (skip_row) {
                printf("Row %d skipped\n", row);
            }
        }
    }

    fclose(file);

    // Print the hash table
    for (int i = 0; i < 101; i++) {
        if (hash_table[i]) {
            HashNode *current = hash_table[i];
            printf("Key: %s, Value: %s\n", current->key, current->value);

            while (current->next) {
                current = current->next;
                printf("Key: %s, Value: %s\n", current->key, current->value);
            }
        }
    }

    return 0;
}
