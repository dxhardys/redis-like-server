#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include "list.h"
#include "hashmap.h"



struct th_info
{
	int fd;
	int i;
    struct HashMap *hashMap; 
    struct List *list; 
};


void * handle_client(void * pctx)
{

    struct th_info * ctx = (struct th_info *)pctx;
    int client_fd = ctx->fd;
const char *welcome = 
    "\n"
    " _______ _______ ______ ________________       _______ _______ _______         _______ _______ \n"
    "(  ____ (  ____ (  __  \\__   __(  ____ \\     (  ____ (  ____ (  ____ |\\     /(  ____ (  ____ )\n"
    "| (    )| (    \\| (  \\  )  ) (  | (    \\/     | (    \\| (    \\| (    )| )   ( | (    \\| (    )|\n"
    "| (____)| (__   | |   ) |  | |  | (_____ _____| (_____| (__   | (____)| |   | | (__   | (____)|\n"
    "|     __|  __)  | |   | |  | |  (_____  (_____(_____  |  __)  |     __( (   ) |  __)  |     __)\n"
    "| (  |  | (     | |   ) |  | |        ) |           ) | (     | (\\ (   \\ \\_/ /| (     | (    \n"
    "| ) \\ \\_| (____/| (__/  ___) (__/\\____) |     /\\____) | (____/| ) \\ \\__ \\   / | (____/| ) \\ \\__\n"
    "|/   \\__(_______(______/\_______\\_______)     \\_______(_______|/   \\__/  \\_/  (_______|/   \\__/\n"
    "\n" 
    "Bienvenue sur ce serveur tapez 'HELP' pour voir la liste des commandes supportée \n" ;
    write(client_fd,welcome,strlen(welcome)); 


    while(1){

  
        char buffer[128];
        int bytesRead = read(client_fd, buffer, sizeof(buffer) - 1);
        char key[128];
        char value[128];
        printf("%s",buffer);
        if (bytesRead > 0)
        {
            buffer[bytesRead] = '\0'; // On rajoute un caractere de fin.
            
//----------------------COMMANDE PING----------------------------------------------------------//
        if (strncmp(buffer, "PING", 4) == 0) {
            // Si la commande commence par "PING"
            if ((buffer[4] == '\0') || (buffer[4] == '\n')) {
            // Envoyer la réponse 'PONG' si la commande est juste "PING"
            const char *response = "PONG\n";
            write(client_fd, response, strlen(response));

            } else if (buffer[4] == ' ') {
                // Si la commande commence par "PING " avec un message
                // Envoyer le message comme réponse
                const char *response = &buffer[5]; // Ignorer "PING " et envoyer le reste comme réponse
                write(client_fd, response, strlen(response));
            } else {
                // Commande inconnue
                const char *response = "Erreur de ping \n";
                write(client_fd, response, strlen(response));
            }
//----------------------COMMANDE GET ---------------------------------------------------------//
        } else if (strncmp(buffer, "GET",3) == 0){
            if (buffer[3] == ' ')
            {
                if(sscanf(&buffer[4], "%127s", key) == 1){
                   const char *value = get(ctx->hashMap, key);
                   if(value != NULL){
                    // Clé trouvée
                    
                    write(client_fd, value,strlen(value));
                   } else {
                    // Clé inéxistante
                    const char *response = "(nil) \n";
                    write(client_fd,response,strlen(response));

                   }
                   
 
                } else {
                    const char *response = "Format invalide pour la commande GET\n";
                    write(client_fd, response, strlen(response));
                }
            } else {
                // Format de la commande Invalide
                const char *response = "Format invalide pour la commande GET\n";
                write(client_fd, response, strlen(response));
            }
//----------------------COMMANDE SET----------------------------------------------------------//
        } else if (strncmp(buffer,"SET",3) == 0){

            if (buffer[3] == ' ')
            {
                if(sscanf(&buffer[4], "%127s %127s", key, value) == 2){
                   put(ctx->hashMap, key, value);
                   const char *response = "OK \n" ;
                   write(client_fd,response,strlen(response));
 
                } else {
                    const char *response = "Format invalide pour la commande SET\n";
                    write(client_fd, response, strlen(response));
                }
            } else {
                // Commande mal formaté
                const char *response = "Format invalide pour la commande SET\n";
                write(client_fd, response, strlen(response));
            }
            
//----------------------COMMANDE DEL--------------------------------------------------------//
        } else if (strncmp(buffer, "DEL", 3) == 0) {
            if (buffer[3] == ' ') {
                char keys[128];
                if (sscanf(&buffer[4], "%127[^\n]", keys) == 1) {
                    char *token = strtok(keys, " ");
                    int deletedCount = 0;

                    while (token != NULL) {
                        const char *value = get(ctx->hashMap, token);
                        if (value != NULL) {
                            del(ctx->hashMap, token);
                            deletedCount++;
                        }
                        token = strtok(NULL, " ");
                    }

                    // Envoyer la réponse indiquant le nombre de clés supprimées
                    char response[128];
                    snprintf(response, sizeof(response), "integer (%d)", deletedCount);
                    write(client_fd, response, strlen(response));
                        } else {
                            const char *response = "Format Invalide pour la commande DEL";
                            write(client_fd, response , strlen(response));

                        }
                    }
//--------------------COMMANDE KEYS----------------------------------------------------------//
        } else if(strncmp(buffer,"KEYS",4) == 0) {
            char result[1024] ;
            // Récuperation de toute les clés
            getKeys(ctx->hashMap, result,sizeof(result));

            write(client_fd,result,strlen(result));


//--------------------LKEYS-------------------------------------------------------------------//
        }else if(strncmp(buffer,"LKEYS",5) == 0){
            char result[1024] ;
            // Récupération des éléments de la liste
            LKEYS(ctx->list, result,sizeof(result));

            write(client_fd,result,strlen(result));


//--------------------RPUSH--------------------------------//
        }else if(strncmp(buffer,"RPUSH",5) == 0){
            if(buffer[5]==' '){
                    char value[256];
                    int pushedValues =0 ;
                    if (sscanf(&buffer[6], "%127[^\n]", value) == 1) {
                    char *token = strtok(value, " ");

                    while (token != NULL) {
                        rPush(ctx->list, token);
                        token = strtok(NULL, " ");
                        pushedValues++;
                    }

                // Envoyer la réponse indiquant le nombre de clés supprimées
                char response[128];
                snprintf(response, sizeof(response), "integer (%d)", pushedValues);
                write(client_fd, response, strlen(response));
                } 
        
            } else {
                    const char* response = "Format invalide pour la commande RPUSH\n";
                    write(client_fd, response, strlen(response));
                }
//--------------------RPOP---------------------------------//
        }else if(strncmp(buffer,"RPOP",4) == 0){
            char value[128];
            if((rPop(ctx->list,value)) != NULL){
                const char *response = value ;
                write(client_fd,response,strlen(response));

            }else{
                const char *response = "La liste est vide" ;
                write(client_fd,response,strlen(response));
            }



//---------------------COMMANDE APPEND----------------------------------------------------------//
        } else if(strncmp(buffer,"APPEND", 6) == 0){
            if(buffer[6] == ' '){
                char key[128];
                char text[128];
                if (sscanf(&buffer[7], "%127s %127s",key, text) == 2)
                {
                    append(ctx->hashMap, key, text);
                    char response[128];
                    const char *value = get(ctx->hashMap,key);
                    snprintf(response,sizeof(response), "integer (%zu)",strlen(value));
                    write(client_fd,response,strlen(response));
                } else {
                    const char *response = "Format Invalide pour la commande APPEND \n";
                    write(client_fd,response,strlen(response));
                }
                

            }else{
                const char *response = "Format Invalide pour la commande APPEND \n";
                write(client_fd,response,strlen(response));
            }

//-------------------COMMANDE EXISTS----------------------------------------------------------//

        } else if(strncmp(buffer,"EXISTS", 6) == 0){
            if (buffer[6] == ' ') {
            char keys[128];
            if (sscanf(&buffer[7], "%127[^\n]", keys) == 1) {
                char *token = strtok(keys, " ");
                int existsCount = 0;

                while (token != NULL) {
                    const char *value = get(ctx->hashMap, token);
                    if (value != NULL) {
                        existsCount++;
                    }
                    token = strtok(NULL, " ");
                }

                // Envoyer la réponse indiquant le nombre de clés trouvées
                char response[128];
                snprintf(response, sizeof(response), "integer(%d)", existsCount);
                write(client_fd, response, strlen(response));
        } else {
            const char *response = "Format invalide pour la commande EXISTS";
            write(client_fd, response, strlen(response));
        }
    }
//-----------------COMMANDE ECHO----------------------------------------------------------------//

        } else if (strncmp(buffer,"ECHO",4) == 0){
            if(buffer[4] == '\0'){

                const char *response = "(erreur) nombre d'arguments éronée pour la commande 'echo' " ;
                write(client_fd, response, strlen(response));

            }else if (buffer[4] == ' '){
                // Retourner le message comme réponse
                const char *response = &buffer[5];
                write(client_fd, response, strlen(response));

            }
            
//---------------COMMANDE SAVE -----------------------------------------------------------------//
        }else if(strncmp(buffer, "SAVE" ,4) == 0){
            if (buffer[5] == '\0'){
                const char *response = "Donnez un nom au fichier : SAVE <Nom_du_fichier>";
                write(client_fd,response,strlen(response));

            } else {
                char filename[128];
                if (sscanf(&buffer[5] ,"%127s", filename) == 1){
                    saveHashMapToFile(ctx->hashMap, filename);
                    const char *response = "Sauvegardé avec succes";
                    write(client_fd,response,strlen(response));

                } else {
                    const char *response = "Nom de fichier invalide";
                    write(client_fd,response,strlen(response));
                }
                
            }
//----------------COMMANDE LOAD ----------------------------------------------------------------//
        }else if (strncmp(buffer, "LOAD",4) == 0) {
           if(buffer[5] == '\0'){
               const char *response = "Spécifiez le fichier à charger : LOAD <Nom_du_fichier>";
               write(client_fd,response,strlen(response));
           } else {
               char filename[128];
               if (sscanf(&buffer[5], "%127s", filename) == 1) {
                   loadHashMapFromFile(ctx->hashMap, filename);
                   const char *response = "Chargé avec succés";
                   write(client_fd,response,strlen(response));
               } else {
                   const char *response = "Nom de fichier invalide";
                   write(client_fd,response,strlen(response));
               }
           }
//--------------------COMMANDE STRLEN----------------------------------------------------------//
        }else if(strncmp(buffer, "STRLEN",6) == 0){
           if (buffer[6] == ' ')
           {
               if(sscanf(&buffer[7], "%127s", key) == 1){
                  const char *value = get(ctx->hashMap, key);
                  if(value != NULL){
                   // Clé trouvée
                   //size_t length = strlen(value);
                   char lengthStr[128];
                  // snprintf(lengthStr, sizeof(lengthStr), "%zu", length);
                   snprintf(lengthStr,sizeof(lengthStr), "(integer) %zu",strlen(value));
                   write(client_fd, lengthStr, strlen(lengthStr));
                  } else {
                   // Clé inéxistante
                   const char *response = "(integer) 0 \n";
                   write(client_fd,response,strlen(response));
                  }
               } else {
                   const char *response = "Format invalide pour la commande STRLEN\n";
                   write(client_fd, response, strlen(response));
               }
           } else {
               // Format de la commande Invalide
               const char *response = "Format invalide pour la commande STRLEN\n";
               write(client_fd, response, strlen(response));
           }
//----------------COMMANDE RANDOMKEY------------------------------------------------------------//
        }else if(strncmp(buffer,"RANDOMKEY",9) == 0){
               const char* value =getRandomKey(ctx->hashMap);
               if (value != NULL)
               {
                   write(client_fd,value,strlen(value));
               }else{
                   const char* response = "Aucune clé dans la base de données";
                   write(client_fd,response,strlen(response));
               }
//-----------------MGET----------------------------------------------------------------//
        }else if(strncmp(buffer,"MGET",4)== 0){
            if (buffer[4] == ' ') {
              char keys[128];
                 if (sscanf(&buffer[5], "%127[^\n]", keys) == 1) {
                    char *token = strtok(keys, " ");
                    int i = 1;
                    while (token != NULL) {
                          const char *value = get(ctx->hashMap, token);
                         if (value != NULL) {
                            char response[128];
                            snprintf(response,sizeof(response), "%d)%s  \n",i,value);

                            //strcpy(response,value);
                            //strcat(response," \n");
                            write(client_fd,response,strlen(response));
                            ++i ;
                         }else{
                            char response[128];
                            snprintf(response,sizeof(response), "%d)(nil)  \n",i);
                            write(client_fd,response,strlen(response));
                         }
                         token = strtok(NULL, " ");
                    }
                         } else {
                             const char *response = "Format Invalide pour la commande MGET";
                             write(client_fd, response , strlen(response));

                         }
                     }

//------------------QUIT---------------------------------------------------------------------------//       
        }else if (strncmp(buffer,"QUIT",4) == 0 ){
           const char *response = "Au revoir ! \n";
           write(client_fd,response, strlen(response));
           close(client_fd);
           break;

//------------------MSET---------------------------------------------------------------------------//       
        }else if (strncmp(buffer,"MSET",4) == 0){
           mset(ctx->hashMap,&buffer[5]) ;

 //-----------------COMMANDE HELP----------------------------------------------------------------//
        }else if(strncmp(buffer,"HELP",4) == 0){
           if((buffer[4] == '\0') || (buffer[4] == '\n')){
           const char *response = "Vous êtes sur un serveur qui implemente une partie des commandes du protocol REDIS tapez HELP <COMMAND> pour obtenir de l'aide supplémentaire  \n"
           "Voici la liste des commandes implementée : \nPING\nECHO\nSET\nGET\nDEL\nAPPEND\nEXISTS\nKEYS\nSTRLEN\nMGET\nMSET\nRANDOMKEY\nLKEYS\nRPUSH\nRPOP\nSAVE\nLOAD\nQUIT\n" ;
           write(client_fd,response,strlen(response));
           } else {
               char command[128];
               if(sscanf(&buffer[5], "%127s", command) == 1) {
                   if(strcmp(command, "PING") == 0){
                      const char *response = "Renvoie 'PONG' si aucun argument n'est fourni, sinon retourne une copie de l'argument. "
                     "Cette commande est utile pour :\n"
                     "1. Tester si une connexion est toujours vivante. \n"
                     "2. Vérifier la capacité du serveur à fournir des données. \n"
                     "3. Mesurer la latence. \n";
                       write(client_fd, response,strlen(response));
                   }else if(strcmp(command, "SET") == 0){
                       const char *response = "Définir la clé pour contenir la valeur de la chaîne. Si la clé contient déjà une valeur, \n"
                       " elle est écrasée, quel que soit son type. Toute durée de vie antérieure associée à la clé est supprimée en cas d'opération SET réussie. \n";
                       write(client_fd, response,strlen(response));
                   }else if(strcmp(command, "GET") == 0){
                       const char *response = "Obtenez la valeur de la clé. Si la clé n'existe pas, la valeur spéciale nil est renvoyée. \n" 
                       "Une erreur est renvoyée si la valeur stockée dans key n'est pas une chaîne, car GET ne gère que les valeurs de chaîne.";
                       write(client_fd, response,strlen(response));
                   }else if(strcmp(command, "DEL") == 0){
                       const char *response = "Supprime les clés spécifiées. Une clé est ignorée si elle n'existe pas. \n";
                       write(client_fd, response,strlen(response));
                   }else if(strcmp(command, "APPEND") == 0){
                       const char *response = "Si la clé existe déjà et est une chaîne, cette commande ajoute la valeur à la fin de la chaîne.\n"
                       "Si la clé n'existe pas, elle est créée et définie comme une chaîne vide, donc APPEND sera similaire à SET dans ce cas particulier.\n";
                       write(client_fd, response,strlen(response));
                   }else if(strcmp(command, "EXISTS") == 0){
                       const char *response = "Renvoie si la clé existe. L'utilisateur doit être conscient que si la même clé existante est mentionnée plusieurs" 
                       "fois dans les arguments, elle sera comptée plusieurs fois. Donc, si somekey existe, EXISTS somekey somekey renverra 2. \n";
                       write(client_fd, response,strlen(response));
                   }else if(strcmp(command, "KEYS") == 0){
                       const char *response = "Retourne toutes les clés \n";
                       write(client_fd, response,strlen(response));
                   }else if(strcmp(command,"ECHO") == 0){
                       const char *response = "Retourne la chaine de caractere saisie \n";
                       write(client_fd,response,strlen(response));
                   }else if(strcmp(command, "SAVE") == 0){
                       const char *response = "Sauvegarde les paires clé/valeur actuelle dans un fichier JSON "
                       "SAVE <FILENAME.json> \n" ;
                       write(client_fd,response, strlen(response));
                   }else if(strcmp(command,"LOAD") == 0) {
                       const char *response = "Charge les paires clé/valeur à partir d'un fichier JSON "
                       "LOAD <FILENAME.json> \n" ;
                       write(client_fd,response,strlen(response));
                   }else if (strcmp(command,"STRLEN") == 0){
                       const char *response = "Retourne la taille de la valeur de la clé saisie : STRLEN <mykey> \n";
                       write(client_fd,response,strlen(response));    
                   }else if (strcmp(command,"MSET") == 0 ){
                       const char *response ="Permet de saisir plusieurs clé/valeur à la fois.\n" ;
                       write(client_fd,response,strlen(response));
                   }else if(strcmp(command,"RANDOMKEY") == 0 ){
                       const char *response = "Renvoie une clé aléatoire de la base de donnée\n" ;
                       write(client_fd,response,strlen(response));
                   }else if(strcmp(command,"QUIT") == 0){
                       const char *response = "Ferme la connexion avec le serveur";
                       write(client_fd,response,strlen(response));
                   }else if(strcmp(command,"MGET") == 0 ){
                       const char *response = "Permet de récuperer plusieurs clés à la fois : GET <key1> <key2> <key3> \n";
                       write(client_fd,response,strlen(response));
                   }else if (strncmp(command,"LKEYS",5)== 0){
                       const char *response = "Affiche tous les élements de la liste";
                       write(client_fd,response,strlen(response));
                   }else if (strncmp(command,"RPUSH",5)== 0){
                       const char *response = "Permet ";
                       write(client_fd,response,strlen(response));
                   }else if(strncmp(command,"RPOP",4) == 0){
                       const char *response = "";
                       write(client_fd,response,strlen(response));
                   }else{
                       const char *response = "format invalide \n";
                       write(client_fd, response,strlen(response));
                   }
               }
           }
        } else {
            const char *response = "Commande inconnue ou non pris en charge \n";
            write(client_fd,response,strlen(response));

        }

        } else if (bytesRead == 0) {
            // Le client s'est déconnecté
            break;
        } else {
            // Erreur de lecture
            perror("read");
            break;
        }
    
    }
    close(client_fd);
    free(ctx);

    return NULL;
}

int main(int argc, char const *argv[])
{

    //Initialisation de la table de Hachage
    struct HashMap hashMap;
    initHashMap(&hashMap);

    //Initialisation de la liste
    struct List list;
    initList(&list) ;



    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    struct addrinfo * result = NULL;

    int ret = getaddrinfo(NULL, argv[1], &hints, &result);


    if(ret < 0)
    {
        herror("getaddrinfo");
        return 1;
    }

    int sock = 0;
    int server_ready = 0;

    struct addrinfo *tmp;
    for(tmp = result; tmp != NULL; tmp=tmp->ai_next)
    {
        sock = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);

        if(sock < 0)
        {
            perror("socket");
            continue;
        }

        if( bind(sock, tmp->ai_addr, tmp->ai_addrlen) < 0)
        {
            perror("bind");
            continue;
        }

        if( listen(sock, 5) < 0)
        {
            perror("listen");
            continue;
        }

        server_ready = 1;
        break;
    }

    if(server_ready == 0)
    {
        fprintf(stderr, "PAS SERVER READY :-'(");
        return 1;
    }


    int client_fd = -1;

    struct sockaddr client_addr;
    socklen_t client_addr_len;

    int i = 0;


    while(1){

        client_fd = accept(sock, &client_addr, &client_addr_len);

        if(client_fd < 0)
        {
            perror("accept");
            break;
        }
	
	struct th_info * ctx = malloc(sizeof(struct th_info));
	if(ctx == NULL)
	{
		perror("malloc");
	}
	ctx->fd = client_fd;
	ctx->i = i++;
    ctx->hashMap = &hashMap;
    ctx->list = &list;

	pthread_t th;
	pthread_create(&th, NULL, handle_client, (void*)ctx);

    }

    close(sock);
    destroyHashMap(&hashMap);
    return 0;
}
