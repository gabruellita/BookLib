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

bool contains_space(char *string)
{
    if (strchr(string, ' ') != NULL)
    {
        return true;
    }
    return false;
}

void register_function(int sockfd)
{
    sockfd = open_connection("34.246.184.49", 8080, AF_INET, SOCK_STREAM, 0);

    char *username = calloc(BUFLEN, sizeof(char));
    char *password = calloc(BUFLEN, sizeof(char));
    char *message = calloc(BUFLEN, sizeof(char));
    char *response = calloc(BUFLEN, sizeof(char));
    char *response_copy = calloc(BUFLEN, sizeof(char));

    // printf("username=");
    // scanf("%s", username);
    // printf("password=");
    // scanf("%s", password);

    // getchar();
    // fflush(stdin);
    printf("username=");
    // scanf("%s", username);
    fgets(username, 120, stdin);
    username[120] = '\0';

    printf("password=");
    // scanf("%s", password);
    fgets(password, 120, stdin);
    password[120] = '\0';

    if (contains_space(username) || contains_space(password))
    {
        printf("  ERROR Nu ai voie cu spatii\n");
        return;
    }
    // printf("%s\n%s", username, password);

    JSON_Value *root = json_value_init_object();
    JSON_Object *rootObject = json_value_get_object(root);
    json_object_set_string(rootObject, "username", username);
    json_object_set_string(rootObject, "password", password);

    char *body_data = json_serialize_to_string_pretty(root);

    message = compute_post_request("34.246.184.49", "/api/v1/tema/auth/register", "application/json", &body_data, 1, NULL, 0);

    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    // printf("%s", response);

    strncpy(response_copy, response, 12);
    response_copy[12] = '\0';
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
            printf(" ERROR alt tip de eroare\n");
        }
    }

    free(username);
    free(password);
    free(message);
    free(response);
    free(response_copy);
    free(body_data);
    // close_connection(sockfd);
    return;
}

char *login_function(int sockfd)
{
    sockfd = open_connection("34.246.184.49", 8080, AF_INET, SOCK_STREAM, 0);
    // printf("%d\n", sockfd);

    char *username = calloc(BUFLEN, sizeof(char));
    char *password = calloc(BUFLEN, sizeof(char));
    char *message = calloc(BUFLEN, sizeof(char));
    char *response = calloc(BUFLEN, sizeof(char));
    char *response_copy = calloc(BUFLEN, sizeof(char));
    char *response_copy2 = calloc(BUFLEN, sizeof(char));
    // getchar();
    // fflush(stdin);
    printf("username=");
    // scanf("%s", username);
    fgets(username, 120, stdin);
    username[120] = '\0';

    printf("password=");
    // scanf("%s", password);
    fgets(password, 120, stdin);
    password[120] = '\0';

    if (contains_space(username) || contains_space(password))
    {
        printf(" ERROR Nu ai voie cu spatii\n");
        free(username);
        free(password);
        free(message);
        free(response);
        free(response_copy);
        free(response_copy2);
        // close_connection(sockfd);
        return NULL;
    }
    // printf("%s\n%s", username, password);

    JSON_Value *root = json_value_init_object();
    JSON_Object *rootObject = json_value_get_object(root);
    json_object_set_string(rootObject, "username", username);
    json_object_set_string(rootObject, "password", password);

    char *body_data = json_serialize_to_string_pretty(root);

    message = compute_post_request("34.246.184.49", "/api/v1/tema/auth/login", "application/json", &body_data, 1, NULL, 0);

    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    // printf("%s", response);

    // response_copy = strstr(response, "connect.sid");
    // response_copy2 = strstr(response_copy, ";");
    // int nr = response_copy2 - response_copy;
    // char *cookie = calloc(BUFLEN, sizeof(char));
    // strncpy(cookie, response_copy + 12, nr - 12);

    if (!strncmp(response, "HTTP/1.1 200", 12))
    {
        response_copy = strstr(response, "connect.sid");
        response_copy2 = strstr(response_copy, ";");
        int nr = response_copy2 - response_copy;
        char *cookie = calloc(BUFLEN, sizeof(char));
        strcpy(cookie, "connect.sid=");
        strncpy(cookie + 12, response_copy + 12, nr - 12);
        printf(" SUCCESS Te-ai logat\n");
        // printf("cu acest cookie: %s", cookie);
        // close_connection(sockfd);
        return cookie;
    }
    else
    {
        printf(" ERROR Nu te-ai logat\n");
        // close_connection(sockfd);
        return NULL;
    }
}

char *enter_library(int sockfd, char *cookies)
{
    sockfd = open_connection("34.246.184.49", 8080, AF_INET, SOCK_STREAM, 0);
    // printf("%d\n", sockfd);
    char *message = calloc(BUFLEN, sizeof(char));
    char *response = calloc(BUFLEN, sizeof(char));
    // printf("%s\n", cookies);
    char **cookie = malloc(sizeof(char *) * 2);
    for (int i = 0; i < 2; i++)
    {
        cookie[i] = malloc(sizeof(char) * LINELEN);
    }

    if (cookies == NULL)
    {
        printf(" ERROR Nu esti logat\n");
        return NULL;
    }

    strcpy(cookie[0], cookies);
    // printf("%s\n", cookie[0]);
    message = compute_get_request("34.246.184.49", "/api/v1/tema/library/access", NULL, cookie, 1);
    // printf("%s", message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    // printf("%s", response);

    if (!strncmp(response, "HTTP/1.1 200", 12))
    {
        printf(" SUCCESS Ai accesat libraria\n");
        char *response_copy = calloc(BUFLEN, sizeof(char));
        // char *response_copy2 = calloc(BUFLEN, sizeof(char));
        response_copy = strstr(response, "{\"token\":");
        response_copy = response_copy + 10;
        response_copy[strlen(response_copy) - 2] = '\0';
        // printf("\n%s", response_copy);
        return response_copy;
    }
    else
    {
        printf(" ERROR Nu ai reusit sa accesezi libraria\n");
        return NULL;
    }
}

void get_books(int sockfd, char *cookies, char *jwt)
{
    sockfd = open_connection("34.246.184.49", 8080, AF_INET, SOCK_STREAM, 0);
    // printf("%d\n", sockfd);
    char *message = calloc(BUFLEN, sizeof(char));
    char *response = calloc(BUFLEN, sizeof(char));
    char **cookie = malloc(sizeof(char *) * 2);
    for (int i = 0; i < 2; i++)
    {
        cookie[i] = malloc(sizeof(char) * LINELEN);
    }

    strcpy(cookie[0], cookies);

    message = compute_get_request_jwt("34.246.184.49", "/api/v1/tema/library/books", NULL, cookie, 1, jwt);
    // printf("%s\n", message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    // printf("%s\n", response);

    if (cookies == NULL)
    {
        printf(" ERROR Nu esti logat\n");
        return;
    }

    if (jwt == NULL)
    {
        printf(" ERROR Nu ai jwt token\n");
    }
    else
    {
        if (!strncmp(response, "HTTP/1.1 200", 12))
        {
            printf(" SUCCESS ai luat carti\n");
            char *response_copy = calloc(BUFLEN, sizeof(char));
            response_copy = strstr(response, "[");
            printf("%s\n", response_copy);
            return;
        }
        else
        {
            printf(" ERROR Nu ai reusit sa accesezi cartile\n");
            return;
        }
    }
}

void add_book(int sockfd, char *cookies, char *jwt)
{
    sockfd = open_connection("34.246.184.49", 8080, AF_INET, SOCK_STREAM, 0);
    // printf("%d\n", sockfd);
    char *message = calloc(BUFLEN, sizeof(char));
    char *response = calloc(BUFLEN, sizeof(char));
    char **cookie = malloc(sizeof(char *) * 2);
    for (int i = 0; i < 2; i++)
    {
        cookie[i] = malloc(sizeof(char) * LINELEN);
    }

    strcpy(cookie[0], cookies);

    if (cookies == NULL)
    {
        printf(" ERROR Nu esti logat\n");
        return;
    }

    if (jwt == NULL)
    {
        printf(" ERROR Nu ai jwt token\n");
        return;
    }
    // printf("%s\n", cookies);

    char *title = calloc(100, sizeof(char));
    char *author = calloc(100, sizeof(char));
    char *genre = calloc(100, sizeof(char));
    char *page_count = calloc(100, sizeof(char));
    char *publisher = calloc(100, sizeof(char));

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

    // printf("%d %d\n", atoi(page_count), isNumber(page_count));

    if (atoi(page_count) <= 0)
    {
        printf(" ERROR Nu ai introdus un numar de pagini valid\n");
        return;
    }

    JSON_Value *root = json_value_init_object();
    JSON_Object *rootObject = json_value_get_object(root);
    json_object_set_string(rootObject, "title", title);
    json_object_set_string(rootObject, "author", author);
    json_object_set_string(rootObject, "genre", genre);
    json_object_set_string(rootObject, "page_count", page_count);
    json_object_set_string(rootObject, "publisher", publisher);
    char *body_data = json_serialize_to_string_pretty(root);

    message = compute_post_request_jwt("34.246.184.49", "/api/v1/tema/library/books", "application/json", &body_data, 1, cookie, 1, jwt);
    // printf("%s\n", message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    // printf("%s\n", response);
    if (!strncmp(response, "HTTP/1.1 200", 12))
    {
        printf(" SUCCESS Carte adaugata cu succes\n");
    }
    else
    {
        printf(" ERROR Nu ai sa adaugi cartea ai eroare\n");
    }
}

void get_book(int sockfd, char *cookies, char *jwt)
{
    sockfd = open_connection("34.246.184.49", 8080, AF_INET, SOCK_STREAM, 0);
    // printf("%d\n", sockfd);
    char *message = calloc(BUFLEN, sizeof(char));
    char *response = calloc(BUFLEN, sizeof(char));
    char **cookie = malloc(sizeof(char *) * 2);
    for (int i = 0; i < 2; i++)
    {
        cookie[i] = malloc(sizeof(char) * LINELEN);
    }

    strcpy(cookie[0], cookies);

    if (cookies == NULL)
    {
        printf(" ERROR Nu esti logat\n");
        return;
    }

    if (jwt == NULL)
    {
        printf(" ERROR Nu ai jwt token\n");
        return;
    }

    char *id = calloc(100, sizeof(char));
    printf("id=");
    fgets(id, 100, stdin);
    id[strlen(id) - 1] = '\0';

    if (atoi(id) <= 0)
    {
        printf(" ERROR Nu ai introdus un numar de pagini valid\n");
        return;
    }
    // printf("hh\n");
    char *url = calloc(BUFLEN, sizeof(char));
    // url = "/api/v1/tema/library/books/";
    // printf("%s\n", url);
    sprintf(url, "/api/v1/tema/library/books/%d", atoi(id));
    // printf("%s\n", url);

    message = compute_get_request_jwt("34.246.184.49", url, NULL, cookie, 1, jwt);
    // printf("%s\n", message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    // printf("%s\n", response);

    if (!strncmp(response, "HTTP/1.1 200", 12))
    {
        printf(" SUCCESS Carte gasita si afisata\n");
        // response = strstr(response, "{");
        response = basic_extract_json_response(response);
        // printf("%s\n", response);
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
    }
    else
    {
        printf(" ERROR Nu s-a gasit cartea\n");
    }
}

void delete_book(int sockfd, char *cookies, char *jwt)
{
    sockfd = open_connection("34.246.184.49", 8080, AF_INET, SOCK_STREAM, 0);
    // printf("%d\n", sockfd);
    char *message = calloc(BUFLEN, sizeof(char));
    char *response = calloc(BUFLEN, sizeof(char));
    char **cookie = malloc(sizeof(char *) * 2);
    for (int i = 0; i < 2; i++)
    {
        cookie[i] = malloc(sizeof(char) * LINELEN);
    }

    strcpy(cookie[0], cookies);

    if (cookies == NULL)
    {
        printf(" ERROR Nu esti logat\n");
        return;
    }

    if (jwt == NULL)
    {
        printf(" ERROR Nu ai jwt token\n");
        return;
    }

    char *id = calloc(100, sizeof(char));
    printf("id=");
    fgets(id, 100, stdin);
    id[strlen(id) - 1] = '\0';

    if (atoi(id) <= 0)
    {
        printf(" ERROR Nu ai introdus un numar de pagini valid\n");
        return;
    }
    char *url = calloc(BUFLEN, sizeof(char));
    sprintf(url, "/api/v1/tema/library/books/%d", atoi(id));
    message = compute_del_request_jwt("34.246.184.49", url, NULL, cookie, 1, jwt);
    // printf("%s\n", message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    // printf("%s\n", response);
    if (!strncmp(response, "HTTP/1.1 200", 12))
    {
        printf(" SUCCESS Cartea cu id %d a fost stearsa cu succes\n", atoi(id));
    }
    else
    {
        printf(" ERROR Nu s-a gasit cartea\n");
    }
}
void logout(int sockfd, char *cookies, char *jwt)
{
    sockfd = open_connection("34.246.184.49", 8080, AF_INET, SOCK_STREAM, 0);
    // printf("%d\n", sockfd);
    char *message = calloc(BUFLEN, sizeof(char));
    char *response = calloc(BUFLEN, sizeof(char));
    char **cookie = malloc(sizeof(char *) * 2);
    for (int i = 0; i < 2; i++)
    {
        cookie[i] = malloc(sizeof(char) * LINELEN);
    }

    strcpy(cookie[0], cookies);

    if (cookies == NULL)
    {
        printf(" ERROR Nu esti logat\n");
        return;
    }

    if (jwt == NULL)
    {
        printf(" ERROR Nu ai jwt token\n");
        return;
    }

    message = compute_get_request_jwt("34.246.184.49", "/api/v1/tema/auth/logout", NULL, cookie, 1, jwt);
    // printf("%s\n", message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    // printf("%s\n", response);
    if (!strncmp(response, "HTTP/1.1 200", 12))
    {
        printf(" SUCCESS Ai reusit sa te deloghezi\n");
    }
    else
    {
        printf(" ERROR Nu te-ai delogat\n");
    }
}

int main(int argc, char *argv[])
{
    char *input = calloc(LINELEN, sizeof(char));
    char *cookies = calloc(LINELEN, sizeof(char));
    char *token = calloc(LINELEN, sizeof(char));
    int sockfd;

    sockfd = open_connection("34.246.184.49", 8080, AF_INET, SOCK_STREAM, 0);
    close_connection(sockfd);
    // printf("%d\n", sockfd);
    while (1)
    {
        // sockfd = open_connection("34.246.184.49", 8080, AF_INET, SOCK_STREAM, 0);
        // printf("Introdu o comanda\n");
        scanf("%s", input);
        getchar();

        if (!strcmp(input, "register"))
        {
            register_function(sockfd);
            close_connection(sockfd);
        }
        if (!strcmp(input, "login"))
        {
            char *tmp_cookie = login_function(sockfd);
            if (tmp_cookie != NULL)
            {
                strcpy(cookies, tmp_cookie);
            }
            close_connection(sockfd);
        }
        if (!strcmp(input, "enter_library"))
        {
            char *tmp_jwt = enter_library(sockfd, cookies);
            if (tmp_jwt != NULL)
            {
                strcpy(token, tmp_jwt);
            }
            close_connection(sockfd);
        }
        if (!strcmp(input, "get_books"))
        {
            get_books(sockfd, cookies, token);
            close_connection(sockfd);
        }
        if (!strcmp(input, "get_book"))
        {
            get_book(sockfd, cookies, token);
            close_connection(sockfd);
        }
        if (!strcmp(input, "add_book"))
        {
            add_book(sockfd, cookies, token);
            close_connection(sockfd);
        }
        if (!strcmp(input, "delete_book"))
        {
            delete_book(sockfd, cookies, token);
            close_connection(sockfd);
        }
        if (!strcmp(input, "logout"))
        {
            logout(sockfd, cookies, token);
            memset(cookies, 0, LINELEN);
            memset(token, 0, LINELEN);
            close_connection(sockfd);
        }
        if (!strcmp(input, "exit"))
        {
            break;
        }
    }

    close_connection(sockfd);

    return 0;
}