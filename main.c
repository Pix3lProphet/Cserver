#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 5555
#define WEB_ROOT "./website"  // Directory where your website files are located

void handle_request(int client_socket) {
    char response[1024];
    FILE *fp = fopen("website/index.html", "r");  // Open your index.html (or other file)
    if (fp == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    // Read contents of index.html
    fseek(fp, 0L, SEEK_END);
    long file_size = ftell(fp);
    rewind(fp);
    fread(response, sizeof(char), file_size, fp);
    fclose(fp);

    // Send HTTP response
    char http_header[2048] = "HTTP/1.1 200 OK\r\n\n";
    strcat(http_header, response);
    send(client_socket, http_header, strlen(http_header), 0);
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket to port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    // Accept incoming connections and handle requests
    while (1) {
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        printf("New connection accepted\n");

        // Handle incoming request
        handle_request(client_socket);

        // Close the socket
        close(client_socket);
    }

    return 0;
}
