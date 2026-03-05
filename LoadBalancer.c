#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h> 
#define PORT 8080
#define BACKEND_IP "127.0.0.1"
#define NUM_BACKENDS 3

int backend_ports[NUM_BACKENDS] = {8081, 8082, 8083};
int current_backend = 0;

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    signal(SIGCHLD, SIG_IGN);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed"); exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt failed"); exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed"); exit(EXIT_FAILURE);
    }
    
    if (listen(server_fd, 10) < 0) {
        perror("Listen failed"); exit(EXIT_FAILURE);
    }

    printf("Load Balancer is listening on port %d...\n", PORT);

    while(1) {
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed"); continue;
        }
        
        int target_port = backend_ports[current_backend];
        current_backend = (current_backend + 1) % NUM_BACKENDS;


        pid_t pid = fork();

        if (pid == 0) {
 
            close(server_fd);

            printf("[+] Child %d: Forwarding to Backend %d\n", getpid(), target_port);

            int backend_socket;
            struct sockaddr_in backend_address;

            backend_socket = socket(AF_INET, SOCK_STREAM, 0);
            backend_address.sin_family = AF_INET;
            backend_address.sin_port = htons(target_port);
            inet_pton(AF_INET, BACKEND_IP, &backend_address.sin_addr);

            if (connect(backend_socket, (struct sockaddr *)&backend_address, sizeof(backend_address)) < 0) {
                printf("[-] Backend %d offline!\n", target_port);
                close(client_socket);
                exit(0); 
            }

            char buffer[4096] = {0};
            int bytes_read = read(client_socket, buffer, sizeof(buffer));
            if (bytes_read > 0) {
                send(backend_socket, buffer, bytes_read, 0);
            }

            memset(buffer, 0, sizeof(buffer));
            bytes_read = read(backend_socket, buffer, sizeof(buffer));
            if (bytes_read > 0) {
                send(client_socket, buffer, bytes_read, 0);
            }

            close(backend_socket);
            close(client_socket);
            printf("[+] Child %d: Done and exiting.\n", getpid());
            
            exit(0);
        } 
        else if (pid > 0) {
  
            close(client_socket);
        } 
        else {
            perror("Fork failed");
        }
    }

    return 0;
}
