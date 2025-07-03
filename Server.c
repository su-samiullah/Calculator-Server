#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define PORT 8080

// Function to store address and info of each client
struct sockaddr_in* createAddress() {
    struct sockaddr_in *address = malloc(sizeof(struct sockaddr_in));
    address->sin_port = htons(PORT);
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    return address;
}

// Function to get the first number from the client
int FirstNum(int sock) {
    int n1;
    recv(sock, &n1, sizeof(n1), 0); // Receiving the 1st number from the client
    return n1; // Returning the 1st number
}

// Function to get the second number from the client
int SecondNum(int sock) {
    int n2;
    recv(sock, &n2, sizeof(n2), 0); // Receiving the 2nd number from the client
    return n2; // Returning the 2nd number
}

// Addition function
int ADD(int n1, int n2) {
    return n1 + n2;
}

// Subtraction function
int SUB(int n1, int n2) {
    return n1 - n2;
}

// Multiplication function
int MUL(int n1, int n2) {
    return n1 * n2;
}

// Division function
int DIV(int n1, int n2) {
    return (n2 != 0) ? n1 / n2 : 0;  // Check for division by zero
}

// Function to send menu and receive user choice
void* RecieveData(void *str) {
    int clientSocketFD = atoi((char *)str); // Converting string to int.
    char clientName[50];
    int choice = 0;
    char menu[1024];
    char result[1024];

    // Receive client's name
    memset(clientName, 0, sizeof(clientName));
    recv(clientSocketFD, clientName, sizeof(clientName), 0);
    printf("Client connected: %s\n", clientName);  // Print client's name on connection

    while(1) {
        // Send menu to the client
        memset(menu, 0, sizeof(menu));
        strcpy(menu, "Press:\n1. For Addition\n2. For Subtraction\n3. For Multiplication\n4. For Division\n5. Exit\n");
        send(clientSocketFD, menu, strlen(menu), 0);

        ssize_t bytes_recv = recv(clientSocketFD, &choice, sizeof(choice), 0); // Receiving the choice from client
        if (bytes_recv <= 0) {
            printf("Client disconnected unexpectedly.\n");
            break;
        }

        int num1 = FirstNum(clientSocketFD); // Get the first number
        int num2 = SecondNum(clientSocketFD); // Get the second number

        switch (choice) {
            case 1: {
                int cal = ADD(num1, num2);
                sprintf(result, "Result of Addition: %d\n--------------------------------\n", cal);
                send(clientSocketFD, result, strlen(result), 0);
                break;
            }
            case 2: {
                int cal = SUB(num1, num2);
                sprintf(result, "Result of Subtraction: %d\n--------------------------------\n", cal);
                send(clientSocketFD, result, strlen(result), 0);
                break;
            }
            case 3: {
                int cal = MUL(num1, num2);
                sprintf(result, "Result of Multiplication: %d\n--------------------------------\n", cal);
                send(clientSocketFD, result, strlen(result), 0);
                break;
            }
            case 4: {
                int cal = DIV(num1, num2);
                sprintf(result, "Result of Division: %d\n--------------------------------\n", cal);
                send(clientSocketFD, result, strlen(result), 0);
                break;
            }
            case 5:  // Exit option
                printf("Client %s disconnected.\n", clientName);
                close(clientSocketFD);
                return NULL;

            default: 
                sprintf(result, "Invalid choice. Please try again.\n");
                send(clientSocketFD, result, strlen(result), 0);
                break;
        }
    }
    return NULL;
}

// Function to create a new thread to handle client communication
void CreatingThread(int clientSocketFD) {
    char str[20];
    sprintf(str, "%d", clientSocketFD); 
    pthread_t thread;
    pthread_create(&thread, NULL, RecieveData, str); // Create a thread for each client
}

// Accepting connections from clients
void acceptingConnections(int serverSocketFD) {
    while (1) {
        struct sockaddr_in clientAddress;
        int clientAddressSize = sizeof(struct sockaddr_in);
        int clientSocketFD = accept(serverSocketFD, (struct sockaddr *)&clientAddress, &clientAddressSize);

        if (clientSocketFD < 0) {
            printf("Client connection failed\n");
            continue;
        }

        // Create a new thread to handle the client
        CreatingThread(clientSocketFD);
    }
}

// Main function
int main() {
    int serverSocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocketFD < 0) {
        printf("Socket creation failed\n");
        exit(1);
    }

    struct sockaddr_in *serveraddress = createAddress();

    // Binding server
    int result = bind(serverSocketFD, (struct sockaddr *)serveraddress, sizeof(*serveraddress));
    if (result < 0) {
        printf("Bind failed\n");
        close(serverSocketFD);
        exit(1);
    }

    printf("Server listening on port %d\n", PORT);

    // Listening for clients
    int listenResult = listen(serverSocketFD, 10);  // Max 10 clients
    if (listenResult < 0) {
        printf("Listen failed\n");
        close(serverSocketFD);
        exit(1);
    }

    // Accept and handle client connections
    acceptingConnections(serverSocketFD);

    free(serveraddress);
    close(serverSocketFD);
}
