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

const char* rPop(List *list, const char* value){
    if(list->head == NULL){
        return NULL ;
    }
    Node *current = list->head;
    while (current->next != NULL){
        current = current->next;
    }

    return current->data;
    
}