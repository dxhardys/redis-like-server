#ifndef LIST_H
#define LIST_H


#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// Représentation d'un élément dans la liste
typedef struct Node {
    char data[128] ;
    struct Node* next ;

} Node;


// Représentation de la liste
typedef struct List {
    Node* head ;
    Node* tail ;
} List;



void initList(List* list); // Initialisation de la liste
void rPush(List* list, const char* element);// Ajouter un élément à la fin de la liste
void lPush(List* list, const char* element);// Ajouter un élement au début de la liste
void LKEYS(List* list, char* result, size_t resultSize); // Afficher les elements de la liste
const char* rPop(List *list, const char* value); // Retourne l'élément à la fin de la liste
const char* lPop(List *list, const char* value); // Retourne le premier élement de la liste

#endif