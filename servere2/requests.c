#include <stdlib.h> /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

char *compute_get_request(char *host, char *url, char *query_params,
                          char **cookies, int cookies_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL)
    {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    }
    else
    {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    if (cookies != NULL && cookies_count > 0)
    {
        sprintf(line, "Cookie: ");
        for (int i = 0; i < cookies_count; i++)
        {
            strcat(line, cookies[i]);
            if (i < cookies_count - 1)
            {
                strcat(line, "; ");
            }
        }
        compute_message(message, line);
    }
    // Step 4: add final new line
    compute_message(message, "");
    return message;
}

char *compute_get_request_jwt(char *host, char *url, char *query_params,
                              char **cookies, int cookies_count, char *jwt)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL)
    {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    }
    else
    {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    if (jwt != NULL)
    {
        // memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", jwt);
        compute_message(message, line);
    }

    // memset(line, 0, LINELEN);

    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    if (cookies != NULL && cookies_count > 0)
    {
        sprintf(line, "Cookie: ");
        for (int i = 0; i < cookies_count; i++)
        {
            strcat(line, cookies[i]);
            if (i < cookies_count - 1)
            {
                strcat(line, "; ");
            }
        }
        compute_message(message, line);
    }
    // Step 4: add final new line
    compute_message(message, "");
    return message;
}

char *compute_post_request(char *host, char *url, char *content_type, char **body_data,
                           int body_data_fields_count, char **cookies, int cookies_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    /* Step 3: add necessary headers (Content-Type and Content-Length are mandatory)
            in order to write Content-Length you must first compute the message size
    */
    size_t content_length = 0;
    for (int i = 0; i < body_data_fields_count; i++)
    {
        content_length += strlen(body_data[i]);
    }
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);
    sprintf(line, "Content-Length: %zu", content_length);
    compute_message(message, line);

    // Step 4 (optional): add cookies
    if (cookies != NULL && cookies_count > 0)
    {
        char *cookie_header = calloc(BUFLEN, sizeof(char));
        sprintf(cookie_header, "Cookie: ");
        for (int i = 0; i < cookies_count; i++)
        {
            strcat(cookie_header, cookies[i]);
            if (i < cookies_count - 1)
            {
                strcat(cookie_header, "; ");
            }
        }
        compute_message(message, cookie_header);
        free(cookie_header);
    }

    // Step 5: add new line at end of header

    compute_message(message, "");

    // Step 6: add the actual payload data
    for (int i = 0; i < body_data_fields_count; i++)
    {
        strcat(body_data_buffer, body_data[i]);
    }
    strcat(message, body_data_buffer);

    // Free allocated memory
    free(line);
    free(body_data_buffer);

    return message;
}

char *compute_post_request_jwt(char *host, char *url, char *content_type, char **body_data,
                               int body_data_fields_count, char **cookies, int cookies_count, char *jwt)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    /* Step 3: add necessary headers (Content-Type and Content-Length are mandatory)
            in order to write Content-Length you must first compute the message size

    */

    if (jwt != NULL)
    {
        sprintf(line, "Authorization: Bearer %s", jwt);
        compute_message(message, line);
    }

    size_t content_length = 0;
    for (int i = 0; i < body_data_fields_count; i++)
    {
        content_length += strlen(body_data[i]);
    }
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);
    sprintf(line, "Content-Length: %zu", content_length);
    compute_message(message, line);

    // Step 4 (optional): add cookies
    if (cookies != NULL && cookies_count > 0)
    {
        char *cookie_header = calloc(BUFLEN, sizeof(char));
        sprintf(cookie_header, "Cookie: ");
        for (int i = 0; i < cookies_count; i++)
        {
            strcat(cookie_header, cookies[i]);
            if (i < cookies_count - 1)
            {
                strcat(cookie_header, "; ");
            }
        }
        compute_message(message, cookie_header);
        free(cookie_header);
    }

    // Step 5: add new line at end of header

    compute_message(message, "");

    // Step 6: add the actual payload data
    for (int i = 0; i < body_data_fields_count; i++)
    {
        strcat(body_data_buffer, body_data[i]);
    }
    strcat(message, body_data_buffer);

    // Free allocated memory
    free(line);
    free(body_data_buffer);

    return message;
}
char *compute_del_request_jwt(char *host, char *url, char *query_params,
                              char **cookies, int cookies_count, char *jwt)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL)
    {
        sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
    }
    else
    {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    if (jwt != NULL)
    {
        // memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", jwt);
        compute_message(message, line);
    }

    // memset(line, 0, LINELEN);

    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    if (cookies != NULL && cookies_count > 0)
    {
        sprintf(line, "Cookie: ");
        for (int i = 0; i < cookies_count; i++)
        {
            strcat(line, cookies[i]);
            if (i < cookies_count - 1)
            {
                strcat(line, "; ");
            }
        }
        compute_message(message, line);
    }
    // Step 4: add final new line
    compute_message(message, "");
    return message;
}