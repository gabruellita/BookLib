#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <stdbool.h>
#include <ctype.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

// functie care verifica daca un string are spatiu
bool contains_space(char *string)
{
    if (strchr(string, ' ') != NULL)
    {
        return true;
    }
    return false;
}

int main(int argc, char *argv[])
{
    // declaram si initializam variabilele folosite

    // input care va reprezenta comanda
    char *input = calloc(LINELEN, sizeof(char));

    // cookies retine cookie-ul pentru contul curent
    char *cookies = calloc(LINELEN, sizeof(char));

    // token retine cheia de acces a contului curent
    char *token = calloc(LINELEN, sizeof(char));

    // message si response pointer catre messageul trimis catre server
    // response este raspunsul primit de la server
    char *message;
    char *response;

    // initializare socket
    int sockfd;

    while (1)
    {
        // citimi comanda
        scanf("%s", input);
        // apelam getchar() deoarece scanf lasa newline in buffer
        // cand citim cu fgets acesta va fii luat de el
        // de aceea scapam de el cu getchar
        getchar();

        // verificam daca input este register
        if (!strcmp(input, "register"))
        {
            // deschidem socketul
            sockfd = open_connection("34.246.184.49", 8080, AF_INET, SOCK_STREAM, 0);

            char *username = calloc(BUFLEN, sizeof(char));
            char *password = calloc(BUFLEN, sizeof(char));
            char *response_copy = calloc(BUFLEN, sizeof(char));

            // citimi username si password cu fgets pentru a luat si space-urile
            printf("username=");
            fgets(username, 120, stdin);
            username[120] = '\0';

            printf("password=");
            fgets(password, 120, stdin);
            password[120] = '\0';

            // verificam daca avem space si daca da nu trimitem catre server si afisam eroare
            if (contains_space(username) || contains_space(password))
            {
                printf("  ERROR Nu ai voie cu spatii\n");
            }
            else
            {
                // initialsam un obiect json si ii punem fieldurile
                // username si password
                JSON_Value *root = json_value_init_object();
                JSON_Object *rootObject = json_value_get_object(root);
                json_object_set_string(rootObject, "username", username);
                json_object_set_string(rootObject, "password", password);

                // transform json-ul intr-un string pentru al transmite ca parametru functiei post
                char *body_data = json_serialize_to_string_pretty(root);

                // facem un request de tip post la server pentru a crea user-ul nou
                message = compute_post_request("34.246.184.49", "/api/v1/tema/auth/register", "application/json", &body_data, 1, NULL, 0);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);

                strncpy(response_copy, response, 12);
                response_copy[12] = '\0';

                // verificam codul de la raspunsul de la server
                // pentru codul 201 inseamna ca am reusit inregistrarea restul de cooduri au dus la eroare
                if (!strcmp(response_copy, "HTTP/1.1 201"))
                {
                    printf(" SUCCESS Utilizator înregistrat cu succes!\n");
                }
                else
                {
                    if (!strcmp(response_copy, "HTTP/1.1 400"))
                    {
                        printf(" ERROR Utilizator deja exista!\n");
                    }
                    else
                    {
                        printf(" ERROR Alt tip de eroare\n");
                    }
                }
                free(body_data);
            }

            free(username);
            free(password);
            free(response_copy);
            close_connection(sockfd);
        }
        // verificam daca input este login
        if (!strcmp(input, "login"))
        {
            // deschidem socketul
            sockfd = open_connection("34.246.184.49", 8080, AF_INET, SOCK_STREAM, 0);

            char *username = calloc(BUFLEN, sizeof(char));
            char *password = calloc(BUFLEN, sizeof(char));
            char *response_copy;
            char *response_copy2;

            // citim user si parola in aceeasi maniera pentru a rezova cerintele legate de spatiu
            printf("username=");
            fgets(username, 120, stdin);
            username[120] = '\0';

            printf("password=");
            fgets(password, 120, stdin);
            password[120] = '\0';

            if (contains_space(username) || contains_space(password))
            {
                printf(" ERROR Nu ai voie cu spatii\n");
            }
            else
            {

                // cream un obiect json cu user si parola pentru parametrul de la functia de post
                JSON_Value *root = json_value_init_object();
                JSON_Object *rootObject = json_value_get_object(root);
                json_object_set_string(rootObject, "username", username);
                json_object_set_string(rootObject, "password", password);

                char *body_data = json_serialize_to_string_pretty(root);

                // facem request de tip post la server pentru a obtine cookie -ul de sesiune a contului in care dorim sa ne logam
                message = compute_post_request("34.246.184.49", "/api/v1/tema/auth/login", "application/json", &body_data, 1, NULL, 0);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);

                // verificam daca server-ul intoarec codul 200
                if (!strncmp(response, "HTTP/1.1 200", 12))
                {
                    // daca serverul a intors status ok inseamna ca acesta are si campul connect.sid acest avand cookie-ul nostru
                    // extragem acest string din raspuns si setam variabila noasta cookies cu el pentru al folosi
                    // in viitoare comenzi atribuite contului legat
                    response_copy = strstr(response, "connect.sid");
                    response_copy2 = strstr(response_copy, ";");

                    int nr = response_copy2 - response_copy;
                    char *cookie = calloc(200, sizeof(char));

                    strcpy(cookie, "connect.sid=");
                    strncpy(cookie + 12, response_copy + 12, nr - 12);

                    printf(" SUCCESS Te-ai logat\n");

                    memcpy(cookies, cookie, strlen(cookie));
                    free(cookie);
                }
                else
                {
                    printf(" ERROR Nu te-ai logat\n");
                }
            }
            free(username);
            free(password);
            close_connection(sockfd);
        }
        if (!strcmp(input, "enter_library"))
        {
            sockfd = open_connection("34.246.184.49", 8080, AF_INET, SOCK_STREAM, 0);

            // pentru a obtine token-ul cu get am nevoie de cookie-ul contului
            // deorece acesta este un vector de string-uri iar la mine este initializat cu char*
            // astefel initializez un vector cu un singur element
            char **cookie = malloc(sizeof(char *));
            for (int i = 0; i < 1; i++)
            {
                cookie[i] = malloc(sizeof(char) * LINELEN);
            }

            if (cookies == NULL)
            {
                printf(" ERROR Nu esti logat\n");
            }
            else
            {
                // copiem in vectorul cu un singur string cookiel curent
                // pentru a ne putea conecta la contul conectat
                strcpy(cookie[0], cookies);

                // trimetem un request de tip get care va intoarce token necesar
                message = compute_get_request("34.246.184.49", "/api/v1/tema/library/access", NULL, cookie, 1);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);

                if (!strncmp(response, "HTTP/1.1 200", 12))
                {
                    // daca codul de status este bun inseamna ca serverula intors jwt token
                    // pe care il retinem in varialbila token
                    printf(" SUCCESS Ai accesat libraria\n");

                    char *response_copy;

                    response_copy = strstr(response, "{\"token\":");
                    response_copy = response_copy + 10;
                    response_copy[strlen(response_copy) - 2] = '\0';

                    memcpy(token, response_copy, strlen(response_copy));
                }
                else
                {
                    printf(" ERROR Nu ai reusit sa accesezi libraria\n");
                }
            }
            free(cookie[0]);
            close_connection(sockfd);
        }
        if (!strcmp(input, "get_books"))
        {
            sockfd = open_connection("34.246.184.49", 8080, AF_INET, SOCK_STREAM, 0);

            // creeam vectorul cu un singur string
            char **cookie = malloc(sizeof(char *));
            for (int i = 0; i < 1; i++)
            {
                cookie[i] = malloc(sizeof(char) * LINELEN);
            }

            strcpy(cookie[0], cookies);

            // facem  un request de tip get dar folosim o functie updatata a lui compute_get_request
            // acesta adauaga inaintea field-ului cookie si Authorization: Bearer "token-ul nostru"
            message = compute_get_request_jwt("34.246.184.49", "/api/v1/tema/library/books", NULL, cookie, 1, token);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            if (cookies == NULL)
            {
                printf(" ERROR Nu esti logat\n");
            }
            else
            {
                if (token == NULL)
                {
                    printf(" ERROR Nu ai jwt token\n");
                }
                else
                {
                    if (!strncmp(response, "HTTP/1.1 200", 12))
                    {
                        // in cazul in  care statusul primit este ok afisam raspunsul de la server
                        printf(" SUCCESS ai luat carti\n");
                        char *response_copy;
                        response_copy = strstr(response, "[");
                        printf("%s\n", response_copy);
                    }
                    else
                    {
                        printf(" ERROR Nu ai reusit sa accesezi cartile\n");
                    }
                }
            }
            free(cookie[0]);
            close_connection(sockfd);
        }
        if (!strcmp(input, "get_book"))
        {
            sockfd = open_connection("34.246.184.49", 8080, AF_INET, SOCK_STREAM, 0);

            // initialzam si creeam vectorul cu un singur string
            char **cookie = malloc(sizeof(char *));
            for (int i = 0; i < 1; i++)
            {
                cookie[i] = malloc(sizeof(char) * LINELEN);
            }

            strcpy(cookie[0], cookies);

            if (cookies == NULL)
            {
                printf(" ERROR Nu esti logat\n");
            }
            else
            {
                if (token == NULL)
                {
                    printf(" ERROR Nu ai jwt token\n");
                }
                else
                {
                    // citim o variabila id si verificam daca aceasta este numar
                    char *id = calloc(100, sizeof(char));
                    printf("id=");
                    fgets(id, 100, stdin);
                    id[strlen(id) - 1] = '\0';

                    // daca variabila nu este numar nu facem get catre server
                    // folsim atoi care intoarce 0 daca nu avem numar in id
                    // nu puteam avea o carte cu 0 sau negativ numar ca si id
                    if (atoi(id) <= 0)
                    {
                        printf(" ERROR Nu ai introdus un numar de pagini valid\n");
                    }
                    else
                    {
                        // initializam un string de tipul url care este calea pentru functia de get
                        char *url = calloc(BUFLEN, sizeof(char));
                        // la url-ul nostru initial trebuie adaugat in continuare si /id aceasta fiind calea carti
                        sprintf(url, "/api/v1/tema/library/books/%d", atoi(id));

                        // pentru compunerea mesajului folosim din nou functia nou care se ocupa si de jwt token
                        message = compute_get_request_jwt("34.246.184.49", url, NULL, cookie, 1, token);
                        send_to_server(sockfd, message);
                        response = receive_from_server(sockfd);

                        if (!strncmp(response, "HTTP/1.1 200", 12))
                        {
                            // in caz de succes afisam raspunsul de la server
                            printf(" SUCCESS Carte gasita si afisata\n");
                            response = basic_extract_json_response(response);

                            // am observat ca la mine cand adaug carti imi este pus si un newline ("\n") la final
                            // dupa fiecare parametru al cartii
                            // checker-ul nu are un handler pentru asta
                            // astfel creez un string nou identic cu raspunsul fara \n
                            char *response_modified = calloc(BUFLEN, sizeof(char));
                            int j = 0;
                            for (int i = 0; response[i] != '\0'; i++)
                            {
                                if (response[i] == '\\' && response[i + 1] == 'n')
                                {
                                    i++;
                                }
                                else
                                {
                                    response_modified[j++] = response[i];
                                }
                            }
                            response_modified[j] = '\0';

                            printf("%s\n", response_modified);
                            free(response_modified);
                        }
                        else
                        {
                            printf(" ERROR Nu s-a gasit cartea\n");
                        }
                        free(url);
                    }
                    free(id);
                }
            }
            free(cookie[0]);
            close_connection(sockfd);
        }
        if (!strcmp(input, "add_book"))
        {
            sockfd = open_connection("34.246.184.49", 8080, AF_INET, SOCK_STREAM, 0);

            // identic ca la toate functionalitatile pe cont cand esti logat
            char **cookie = malloc(sizeof(char *));
            for (int i = 0; i < 1; i++)
            {
                cookie[i] = malloc(sizeof(char) * LINELEN);
            }

            strcpy(cookie[0], cookies);

            if (cookies == NULL)
            {
                printf(" ERROR Nu esti logat\n");
            }
            else
            {
                if (token == NULL)
                {
                    printf(" ERROR Nu ai jwt token\n");
                }
                else
                {
                    // declaram variabile pentru toate field-urile unei carti si le citim
                    char *title = calloc(100, sizeof(char));
                    char *author = calloc(100, sizeof(char));
                    char *genre = calloc(100, sizeof(char));
                    char *page_count = calloc(100, sizeof(char));
                    char *publisher = calloc(100, sizeof(char));

                    // este posibil ca la citirea unor elemente
                    // la toate fara page_count sa se ia orice caracter si nu am stiut in ce maniera sa tratez aceste cazuri
                    // intrucat nu era foarte bine explicat in enuntul temei
                    // daca erau si limitari la celelate field-uri atunci vericam si daca indeplineam conditiile faceat request-ul
                    printf("title=");
                    fgets(title, 100, stdin);
                    title[100] = '\0';

                    printf("author=");
                    fgets(author, 100, stdin);
                    author[100] = '\0';

                    printf("genre=");
                    fgets(genre, 100, stdin);
                    genre[100] = '\0';

                    printf("page_count=");
                    fgets(page_count, 100, stdin);
                    page_count[100] = '\0';

                    printf("publisher=");
                    fgets(publisher, 100, stdin);
                    publisher[100] = '\0';

                    // daca variabila nu este numar nu facem post catre server
                    // folsim atoi care intoarce 0 daca nu avem numar in page_count
                    // nu puteam avea o carte cu 0 sau negativ numar ca si numar de pagini
                    if (atoi(page_count) <= 0)
                    {
                        printf(" ERROR Nu ai introdus un numar de pagini valid\n");
                    }
                    else
                    {
                        // daca aven un numar valid de pagini creeam obiectul json cu field-urile necesare
                        JSON_Value *root = json_value_init_object();
                        JSON_Object *rootObject = json_value_get_object(root);

                        json_object_set_string(rootObject, "title", title);
                        json_object_set_string(rootObject, "author", author);
                        json_object_set_string(rootObject, "genre", genre);
                        json_object_set_string(rootObject, "page_count", page_count);
                        json_object_set_string(rootObject, "publisher", publisher);

                        // body data pentru functia de post
                        char *body_data = json_serialize_to_string_pretty(root);

                        // am modificat si aceasta functie de post pentru a introduce jwt token inaintea cookiel-ului de sesiune
                        message = compute_post_request_jwt("34.246.184.49", "/api/v1/tema/library/books", "application/json", &body_data, 1, cookie, 1, token);
                        send_to_server(sockfd, message);
                        response = receive_from_server(sockfd);

                        if (!strncmp(response, "HTTP/1.1 200", 12))
                        {
                            // daca avem status ok afisam un mesaj care indica asta
                            printf(" SUCCESS Carte adaugata cu succes\n");
                        }
                        else
                        {
                            printf(" ERROR Nu ai sa adaugi cartea ai eroare\n");
                        }
                        free(body_data);
                    }
                    free(title);
                    free(author);
                    free(genre);
                    free(page_count);
                    free(publisher);
                }
            }
            free(cookie[0]);
            close_connection(sockfd);
        }
        if (!strcmp(input, "delete_book"))
        {
            sockfd = open_connection("34.246.184.49", 8080, AF_INET, SOCK_STREAM, 0);

            // initializam si cream vectorul cu cookie ul de sesiune
            char **cookie = malloc(sizeof(char *));
            for (int i = 0; i < 1; i++)
            {
                cookie[i] = malloc(sizeof(char) * LINELEN);
            }

            strcpy(cookie[0], cookies);

            if (cookies == NULL)
            {
                printf(" ERROR Nu esti logat\n");
            }
            else
            {
                if (token == NULL)
                {
                    printf(" ERROR Nu ai jwt token\n");
                }
                else
                {

                    // citim id-ul carti pe care dorim sa o stergem si verificam sa fie un numar valid
                    char *id = calloc(100, sizeof(char));
                    printf("id=");
                    fgets(id, 100, stdin);
                    id[strlen(id) - 1] = '\0';

                    if (atoi(id) <= 0)
                    {
                        printf(" ERROR Nu ai introdus un numar de pagini valid\n");
                    }
                    else
                    {
                        // url -ul necasar unde se adauga si id-ul carti de sters
                        char *url = calloc(BUFLEN, sizeof(char));

                        // repetam ceea ce am facut la add_book
                        // adica calea carti de sters trebuie sa aiba la final id-ul carti
                        sprintf(url, "/api/v1/tema/library/books/%d", atoi(id));

                        // apelam functia de delete creata de noi
                        // functie asemanatoare cu cea de get
                        // eu personal am copiat functia de get facuta de mine ceea cu jwt token
                        // unde am adaugat DELETE in loc de GET
                        message = compute_del_request_jwt("34.246.184.49", url, NULL, cookie, 1, token);
                        send_to_server(sockfd, message);
                        response = receive_from_server(sockfd);

                        if (!strncmp(response, "HTTP/1.1 200", 12))
                        {
                            // intoarcem un mesaj in cazul unui status ok primit de la server
                            printf(" SUCCESS Cartea cu id %d a fost stearsa cu succes\n", atoi(id));
                        }
                        else
                        {
                            printf(" ERROR Nu s-a gasit cartea\n");
                        }
                        free(url);
                    }
                    free(id);
                }
            }
            free(cookie[0]);
            close_connection(sockfd);
        }
        if (!strcmp(input, "logout"))
        {
            sockfd = open_connection("34.246.184.49", 8080, AF_INET, SOCK_STREAM, 0);

            // initialziam vectorul cu cookie-ul de sesiune
            char **cookie = malloc(sizeof(char *));
            for (int i = 0; i < 1; i++)
            {
                cookie[i] = malloc(sizeof(char) * LINELEN);
            }

            strcpy(cookie[0], cookies);

            if (cookies == NULL)
            {
                printf(" ERROR Nu esti logat\n");
            }
            else
            {
                if (token == NULL)
                {
                    printf(" ERROR Nu ai jwt token\n");
                }
                else
                {

                    // facem get catre server pentru a ne deloga
                    message = compute_get_request_jwt("34.246.184.49", "/api/v1/tema/auth/logout", NULL, cookie, 1, token);
                    send_to_server(sockfd, message);
                    response = receive_from_server(sockfd);

                    if (!strncmp(response, "HTTP/1.1 200", 12))
                    {
                        printf(" SUCCESS Ai reusit sa te deloghezi\n");
                    }
                    else
                    {
                        printf(" ERROR Nu te-ai delogat\n");
                    }
                }
            }
            free(cookie[0]);

            // dupa delogare setam cookie ul de sesiune si token ul de 0 pentru a ne mai conecta pana la o noua logare
            memset(cookies, 0, LINELEN);
            memset(token, 0, LINELEN);
            close_connection(sockfd);
        }
        if (!strcmp(input, "exit"))
        {
            // la exit apelam break si iesim din while-ul infinit
            break;
        }
    }

    free(cookies);
    free(token);
    free(input);

    return 0;
}
