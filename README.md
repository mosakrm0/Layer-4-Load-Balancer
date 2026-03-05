
# Layer 4 TCP Load Balancer

## To begin With
Used Unix Socket Programming with C language to open socket, listen  to connections, bind and fork, look at LoadBalancer.c if you are intrested in the coding area but here is a brief:
```c
// Used libraries:
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
```
```c
// Signal used to prevent zombie proccess
    signal(SIGCHLD, SIG_IGN);

//Creating the socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed"); exit(EXIT_FAILURE);
    }

//Allowing port reuse to avoid "Address already in use" errors
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt failed"); exit(EXIT_FAILURE);
    }
```
```c
// bind and listen functions
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed"); exit(EXIT_FAILURE);
    }
    
    if (listen(server_fd, 10) < 0) {
        perror("Listen failed"); exit(EXIT_FAILURE);
    }

    printf("Load Balancer is listening CONCURRENTLY on port %d...\n", PORT);
```


## Compilation

```bash
gcc LoadBalancer.c -o LoadBalancer
```

## Docker Compose

```yaml
services:
  backend1:
    image: nginx:alpine
    ports:
      - "8081:80"

  backend2:
    image: nginx:alpine
    ports:
      - "8082:80"

  backend3:
    image: nginx:alpine
    ports:
      - "8083:80"

```

## Docker Compose Up

```bash
docker compose up -d
```

## Running the Code we wrote

```bash
./LoadBalancer
```

## Testing

```bash
curl http://localhost:8080 
```
or
```bash
nc 127.0.0.1 8080
```

<img width="942" height="437" alt="image" src="https://github.com/user-attachments/assets/0d36372d-681c-4e93-a58f-ed4c41fbe44c" />
