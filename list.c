#include "list.h"



// Initialisation de la liste
void initList(List* list){
    list->head = NULL ;
    list->tail = NULL ;
}

// Ajouter un élément à la fin de la liste
void rPush(List* list, const char* element){
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    strncpy(newNode->data,element,sizeof(newNode->data)-1);
    newNode->data[sizeof(newNode->data)-1] = '\0' ;
    newNode->next = NULL ;

    if(list->head == NULL){
        list->head = newNode ;
        list->tail = newNode ;
    } else {
        list->tail->next = newNode ;
        list->tail = newNode ;
    }
}

// Ajouter un élement au début de la liste
void lPush(List *list, const char* element) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    strncpy(newNode->data, element, sizeof(newNode->data) - 1);
    newNode->data[sizeof(newNode->data) - 1] = '\0';
    newNode->next = NULL;

    if (list->head == NULL) {
        // La liste est vide
        list->head = newNode;
        list->tail = newNode;
    } else {
        // Ajouter le nouvel élément au début de la liste
        newNode->next = list->head;
        list->head = newNode;
    }

}

// Afficher les elements de la liste
void LKEYS(List* list, char* result, size_t resultSize){
    result[0] = '\0' ;
    Node* current = list->head;
    while (current != NULL){
        strncat(result, current->data, resultSize - strlen(result) -1);
        strncat(result, "\n", resultSize - strlen(result) -1);
        current = current->next ;
        
    }
    
}

const char* rPop(List* list, char* value) {
    if (list->head == NULL) {
        // La liste est vide
        return NULL;
    }

    Node* current = list->head;

    // Trouver le dernier élément
    while (current->next != NULL) {
        current = current->next;
    }

    // Copier la valeur du dernier élément
    strncpy(value, current->data, 127);
    return value;
}

const char* lPop(List *list, char* value){
    if(list->head != NULL){
        strncpy(value, list->head->data,127) ;
        return value ;
    }else {
        // Liste vide
        return NULL ;
    }
}
