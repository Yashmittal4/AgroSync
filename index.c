#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>


#define MAX_LINE_LENGTH 256
#define MAX_PLANTS 100
#define MAX_USERS 100

//Defining plant struct
struct Plant {
    char name[50];
    float optimal_pH_min;
    float optimal_pH_max;
    float optimal_EC_min;
    float optimal_EC_max;
    int optimal_PPM_min;
    int optimal_PPM_max;
};


//Defining User struct
struct User {
    char username[50];
    char password[50];
};


// Function to convert string to lowercase
void toLowerCase(char *str) {
    while (*str) {
        *str = tolower((unsigned char)*str);
        str++;
    }
}

// saving a new user to the file using system calls 
void saveUserToFile(const char *username, const char *password) {
    int file = open("user_data.txt", O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if (file < 0) {
        perror("Error opening file");
        return;
    }

    char buffer[MAX_LINE_LENGTH];
    int len = snprintf(buffer, sizeof(buffer), "%s,%s\n", username, password);
    if (write(file, buffer, len) != len) {
        perror("Error writing to file");
    }

    close(file);
    printf("User registered successfully.\n");
}

// loading users file to match user is right
int loadUsersFromFile(struct User users[], const char *filename) {
    int file = open(filename, O_RDONLY);
    if (file < 0) {
        perror("Error opening file");
        return 0;
    }

    int numUsers = 0;
    char buffer[MAX_LINE_LENGTH];
    ssize_t bytesRead;

    while ((bytesRead = read(file, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0';
        char *line = strtok(buffer, "\n");//strtok is a function used to tokenize a string, which means it breaks the string into smaller segments (tokens) based on specified delimiters.
        while (line) {
            if (sscanf(line, "%[^,],%s", users[numUsers].username, users[numUsers].password) == 2) {
                toLowerCase(users[numUsers].username); // Convert to lowercase 
                numUsers++;
            }//sscanf is a function used to read formatted input from a string. It allows you to extract data from a string based on a specified format.(separate username and password)
            line = strtok(NULL, "\n");
        }
    }

    if (bytesRead < 0) {
        perror("Error reading file");
    }

    close(file);
    return numUsers;
}


// authenticate A user
int authenticateUser(struct User users[], int numUsers, const char *username, const char *password) {
	int i;
	
    for ( i = 0; i < numUsers; i++) {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0) {
            return 1; // Authentication successful
        }
    }
    return 0; // Authentication failed
}




// Function to display the Features menu
void displayMenu() {
    printf("\nAgroSync: Enhancing Hydroponic Farming with Precision \n");
    printf("1. View Recommendations\n");
    printf("2. Conversion Calculator\n");
    printf("3. Save Data to File\n");
    printf("4. Read Data from File\n");
    printf("5. Search for Another Plant\n");
    printf("6. Yield and Water Usage Comparison\n");
    printf("7. Exit\n");
}
int main(){
	
	struct User users[MAX_USERS];
    int numUsers = loadUsersFromFile(users, "user_data.txt");
    char username[50], password[50];
    int authenticated = 0;

    printf("1. Login\n");
    printf("2. Create a new user\n");
    printf("Enter your choice: ");
    int authChoice;
    scanf("%d", &authChoice);

    if (authChoice == 1) {
        printf("Enter username: ");
        scanf("%s", username);
        toLowerCase(username);

        printf("Enter password: ");
        scanf("%s", password);

        if (authenticateUser(users, numUsers, username, password)) {
            printf("Login successful.\n");
            authenticated = 1;
        } else {
            printf("Invalid username or password.\n");
        }
    } else if (authChoice == 2) {
        printf("Enter new username: ");
        scanf("%s", username);
        toLowerCase(username);

        printf("Enter new password: ");
        scanf("%s", password);

        saveUserToFile(username, password);
        authenticated = 1; // Go to main code after succesfull resgistration
    } else {
        printf("Invalid choice. Exiting...\n");
        return 1;
    }

    if (!authenticated) {
        return 1;
    }

	
	
	
	return 0;
}
