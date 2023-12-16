#ifndef HASHMAP_H_
#define HASHMAP_H_

#define TABLE_SIZE 100

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cjson/cJSON.h> 

// Structure pour représenter un élément dans la hashmap
struct KeyValue {
    char key[128];
    char value[128];
    struct KeyValue* next;
};

// Structure pour représenter la hashmap
struct HashMap {
    struct KeyValue* table[TABLE_SIZE];
};


unsigned int hash(const char* key); // Fonction de hachage 
void mset(struct HashMap* hashMap, const char* keysAndValues); // Inserer plusieurs clé/valeur à la fois
void initHashMap(struct HashMap* hashMap); // Initialise la HASHMAP
void put(struct HashMap* hashMap, const char* key, const char* value); // Ajoute une paire à la HASHMAP
void destroyHashMap(struct HashMap* hashMap) ; // Libère les ressources allouées par la hashmap
void del(struct HashMap* hashMap, const char* key) ; // Supprimer une paire de la HASHMAP
void getKeys(struct HashMap* hashMap, char* result, size_t resultSize) ; // Retourne toutes les clés de la HASHMAP
void append(struct HashMap* hashMap, const char* key, const char* text) ; // Etend (append) la valeur d'une clé dans la HASHMAP
void saveHashMapToFile(struct HashMap* hashMap, const char* filename) ; // Stockage de la Hashmap sur le disque en format JSON
void loadHashMapFromFile(struct HashMap* hashMap, const char* filename) ;// Chargement de la hashmap depuis le disque 
const char* get(struct HashMap* hashMap, const char* key); // Cherche une paire dans la HASHMAP
const char* getRandomKey(const struct HashMap* hashMap) ; //Retourne une clé aléatoire
cJSON* serializeHashMap(struct HashMap* hashMap) ; // Sérialisation de la hashmap en JSON.





#endif