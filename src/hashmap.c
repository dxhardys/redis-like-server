#include "../include/hashmap.h"


// Fonction de hashage 
unsigned int hash(const char* key) {
    unsigned int hash = 0;
    while (*key) {
        hash = (hash << 5) + *key++;
    }
    return hash % TABLE_SIZE;
}

// Initialisation de la hashmap
void initHashMap(struct HashMap* hashMap) {
    for (int i = 0; i < TABLE_SIZE; ++i) {
        hashMap->table[i] = NULL;
    }
}


void put(struct HashMap* hashMap, const char* key, const char* value) {
    unsigned int index = hash(key);

    // Crée une nouvelle paire clé-valeur
    struct KeyValue* newPair = (struct KeyValue*)malloc(sizeof(struct KeyValue));
    if (newPair == NULL) {
        perror("malloc");
        return;
    }
    strncpy(newPair->key, key, sizeof(newPair->key));
    strncpy(newPair->value, value, sizeof(newPair->value));
    newPair->next = NULL;

    
    if (hashMap->table[index] == NULL) {
        hashMap->table[index] = newPair;
    } else {
        struct KeyValue* current = hashMap->table[index];
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newPair;
    }
}


const char* get(struct HashMap* hashMap, const char* key) {
    unsigned int index = hash(key);

    
    struct KeyValue* current = hashMap->table[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }

    return NULL; // Clé non trouvée
}


void destroyHashMap(struct HashMap* hashMap) {
    // On itère et on libère
    for (int i = 0; i < TABLE_SIZE; ++i) {
        struct KeyValue* current = hashMap->table[i];
        while (current != NULL) {
            struct KeyValue* next = current->next;
            free(current);
            current = next;
        }
    }
}


void del(struct HashMap* hashMap, const char* key) {
    unsigned int index = hash(key);

    struct KeyValue* current = hashMap->table[index];
    struct KeyValue* previous = NULL;

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            if (previous != NULL) {
                previous->next = current->next;
            } else {
                hashMap->table[index] = current->next;
            }

            free(current);
            return;
        }

        previous = current;
        current = current->next;
    }
}


void getKeys(struct HashMap* hashMap, char* result, size_t resultSize) {
    // Initialiser la chaîne de résultat
    result[0] = '\0';

    // Parcourir la hashmap et concaténer les clés dans la chaîne de résultat
    for (int i = 0; i < TABLE_SIZE; ++i) {
        struct KeyValue* current = hashMap->table[i];
        while (current != NULL) {
            strncat(result, current->key, resultSize - strlen(result) - 1);
            strncat(result, "\n", resultSize - strlen(result) - 1);
            current = current->next;
        }
    }
}


void append(struct HashMap* hashMap, const char* key, const char* text) {
    unsigned int index = hash(key);

    // Recherche la clé dans la hashmap
    struct KeyValue* current = hashMap->table[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            // Si la clé existe on concatene le texte à ajouter
            strncat(current->value, text, sizeof(current->value) - strlen(current->value) - 1);
            return;
        }
        current = current->next;
    }

    // Si la clé n'existe pas on crée une nouvelle paire clé-valeur
    put(hashMap, key, text);
}


const char* getRandomKey(const struct HashMap* hashMap) {
    // Nombre total d'éléments dans la hashmap
    int totalItems = 0;

    // Compter le nombre total d'éléments dans la hashmap
    for (int i = 0; i < TABLE_SIZE; ++i) {
        struct KeyValue* current = hashMap->table[i];
        while (current != NULL) {
            totalItems++;
            current = current->next;
        }
    }

    // Si la hashmap est vide, retourner NULL
    if (totalItems == 0) {
        return NULL;
    }

    // Générer un index aléatoire entre 0 et (totalItems - 1)
    int randomIndex = rand() % totalItems;

    // Trouver l'élément correspondant à l'index aléatoire
    for (int i = 0; i < TABLE_SIZE; ++i) {
        struct KeyValue* current = hashMap->table[i];
        while (current != NULL) {
            if (randomIndex == 0) {
                // Retourner la clé de l'élément trouvé
                return current->key;
            }
            // Passer à l'élément suivant
            current = current->next;
            randomIndex--;
        }
    }

    return NULL;
}


cJSON* serializeHashMap(struct HashMap* hashMap) {
    cJSON* jsonRoot = cJSON_CreateObject();

    for (int i = 0; i < TABLE_SIZE; ++i) {
        struct KeyValue* current = hashMap->table[i];
        while (current != NULL) {
            cJSON_AddItemToObject(jsonRoot, current->key, cJSON_CreateString(current->value));
            current = current->next;
        }
    }

    return jsonRoot;
}


void saveHashMapToFile(struct HashMap* hashMap, const char* filename) {
    cJSON* jsonRoot = serializeHashMap(hashMap);

    char* jsonString = cJSON_Print(jsonRoot);
    FILE* file = fopen(filename, "w");
    if (file != NULL) {
        fputs(jsonString, file);
        fclose(file);
    }

    cJSON_Delete(jsonRoot);
    free(jsonString);
}


void loadHashMapFromFile(struct HashMap* hashMap, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);

        char* jsonString = (char*)malloc(fileSize + 1);
        if (jsonString != NULL) {
            fread(jsonString, 1, fileSize, file);
            jsonString[fileSize] = '\0';

            cJSON* jsonRoot = cJSON_Parse(jsonString);
            if (jsonRoot != NULL) {
                // Désérialisation : parcourir le JSON et ajouter les paires clé-valeur à la hashmap
                cJSON* jsonItem = jsonRoot->child;
                while (jsonItem != NULL) {
                    const char* key = jsonItem->string;
                    const char* value = jsonItem->valuestring;

                    // Ajouter la paire clé-valeur à la hashmap
                    if (key != NULL && value != NULL) {
                        put(hashMap, key, value);
                    }

                    jsonItem = jsonItem->next;
                }
                cJSON_Delete(jsonRoot);
            }
            free(jsonString);
        }
        fclose(file);
    }
}


void mset(struct HashMap* hashMap, const char* keysAndValues) {
    // On récupère la chain concaténé
    char* inputCopy = strdup(keysAndValues);
    if (inputCopy == NULL) {
        perror("strdup");
        return;
    }
    // On récupere les élements séparé par un espace
    char* token = strtok(inputCopy, " ");
    char* currentKey = NULL;
    char* currentValue = NULL;
    int isKey = 1;

    while (token != NULL) {
        if (isKey) {
            currentKey = strdup(token);
        } else {
            currentValue = strdup(token);
            put(hashMap, currentKey, currentValue);
            free(currentKey);
            free(currentValue);
        }

        isKey = !isKey;
        token = strtok(NULL, " ");
    }

    free(inputCopy);
}

