#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int sock;
    struct sockaddr_in server_address;
    char buffer[1024];
    int choice, num1, num2;
    char clientName[50];

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation failed\n");
        return -1;
    }

    // Set server address
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to the server
    if (connect(sock, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        printf("Connection failed\n");
        close(sock);
        return -1;
    }

    // Ask for client's name
    printf("Enter your name: ");
    fgets(clientName, sizeof(clientName), stdin);
    clientName[strcspn(clientName, "\n")] = 0;  // Remove trailing newline

    // Send client's name to the server
    send(sock, clientName, strlen(clientName) + 1, 0);  // Send name with null terminator

    while (1) {
        // Clear the buffer to avoid garbage value
        memset(buffer, 0, sizeof(buffer));

        // Receive and print the menu from the server
        if (recv(sock, buffer, sizeof(buffer), 0) <= 0) {
            printf("Failed to receive menu\n");
            break; // Exit loop on error
        }
        printf("%s", buffer); // Print the menu on the console

        // Get user's choice
        printf("Enter Your Choice\n");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            continue;
        }
        
        // Send the choice to the server
        send(sock, &choice, sizeof(choice), 0);

        if (choice == 5) {
            // Exit option
            printf("GoodBye Exiting...\n");
            break;
        }

        // Handle arithmetic operation
        if (choice >= 1 && choice <= 4) {
            printf("Enter 1st Number: ");
            if (scanf("%d", &num1) != 1) {
                printf("Invalid input. Please enter a valid number.\n");
                continue;
            }
            send(sock, &num1, sizeof(num1), 0);

            printf("Enter 2nd Number: ");
            if (scanf("%d", &num2) != 1) {
                printf("Invalid input. Please enter a valid number.\n");
                continue;
            }
            send(sock, &num2, sizeof(num2), 0);

            // Receive and display the result
            memset(buffer, 0, sizeof(buffer));
            if (recv(sock, buffer, sizeof(buffer), 0) <= 0) {
                printf("Failed to receive result\n");
                continue;
            }
            printf("%s\n", buffer);
        } else {
            // Handle invalid choices
            printf("Invalid Choice. Please try again.\n");
        }
    }

    // Close the socket at the end
    close(sock);
    return 0;
}
