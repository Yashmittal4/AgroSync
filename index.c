#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include<time.h>
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

// Function to get user input for plant details
void getUserInput(float *pH, float *EC, int *PPM) {
    printf("Enter pH: ");
    scanf("%f", pH);
    printf("Enter EC (mS/cm): ");
    scanf("%f", EC);
    printf("Enter PPM: ");
    scanf("%d", PPM);
}

// saving a new user to the file using system calls 
void saveUserToFile(const char *username, const char *password) {
    int file = open("user_data.txt", O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if (file < 0) {
        perror("Error opening file");
        return;
    }

    char buffer[MAX_LINE_LENGTH];
    int len = snprintf(buffer, sizeof(buffer), "%s,%s\n", username, password);//The sprintf function in C is used for formatting strings and storing them in a specified character buffer. 
    if (write(file, buffer, len) != len) {
        perror("Error writing to file");
    }

    close(file);
    printf("User registered successfully.\n");
}

// Check conditions that entered values lie in the range of optimal values or not!
void checkOptimalConditions(struct Plant *plant, float pH, float EC, int PPM, CheckCondition checkCondition) {
    printf("\nOptimal conditions for %s:\n", plant->name);
    printf("PH range: %.2f - %.2f\n", plant->optimal_pH_min, plant->optimal_pH_max);
    printf("EC range: %.2f - %.2f mS/cm\n", plant->optimal_EC_min, plant->optimal_EC_max);
    printf("PPM range: %d - %d\n", plant->optimal_PPM_min, plant->optimal_PPM_max);

    if (checkCondition(pH, plant->optimal_pH_min, plant->optimal_pH_max)) {
        printf("PH is within the optimal range.\n");
    } else {
        printf("PH is out of the optimal range!\n");
    }

    if (checkCondition(EC, plant->optimal_EC_min, plant->optimal_EC_max)) {
        printf("EC is within the optimal range.\n");
    } else {
        printf("EC is out of the optimal range!\n");
    }

    if (checkCondition(PPM, plant->optimal_PPM_min, plant->optimal_PPM_max)) {
        printf("PPM is within the optimal range.\n");
    } else {
        printf("PPM is out of the optimal range!\n");
    }
}

// Function to calculate PPM from EC
int ECtoPPM(float EC, int scale) {
    if (scale == 500)
        return (int)(EC * 500);
    else if (scale == 700)
        return (int)(EC * 700);
    return 0;
}

// Function to calculate EC from PPM
float PPMtoEC(int PPM, int scale) {
    if (scale == 500)
        return (float)PPM / 500;
    else if (scale == 700)
        return (float)PPM / 700;
    return 0;
}

// Function to save data to a file using system calls
void saveDataToFile(const char *plantName, float pH, float EC, int PPM) {
    int file = open("plant_data.txt", O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if (file < 0) {
        perror("Error opening file");
        return;
    }

    char buffer[MAX_LINE_LENGTH];
    int len = snprintf(buffer, sizeof(buffer), "Plant Name: %s\npH: %.2f\nEC: %.2f mS/cm\nPPM: %d\n\n", plantName, pH, EC, PPM);
    if (write(file, buffer, len) != len) {
        perror("Error writing to file");
    }

    close(file);
    printf("Data saved successfully to plant_data.txt.\n");
}

// Function pointer types for condition checking
typedef int (*CheckCondition)(float, float, float);

// Function to check if a value is within a range
int checkWithinRange(float value, float min, float max) {
    return (value >= min && value <= max);
}

// Find plant present in the database
struct Plant* findPlant(struct Plant plants[], int numPlants, const char *name) {
	int i;
    for ( i = 0; i < numPlants; i++) {
        if (strcasecmp(plants[i].name, name) == 0) {//case insensitive string comparision
            return &plants[i];
        }
    }
    return NULL;
}



// Function to load plant data from a file
int loadPlantsFromFile(struct Plant plants[], const char *filename) {
    int file = open(filename, O_RDONLY);
    if (file < 0) {
        perror("Error opening file");
        return 0;
    }

    int numPlants = 0;
    char buffer[MAX_LINE_LENGTH];
    ssize_t bytesRead;

    while ((bytesRead = read(file, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0';
        char *line = strtok(buffer, "\n");
        while (line) {
            if (sscanf(line, "%[^,],%f,%f,%f,%f,%d,%d",
                       plants[numPlants].name,
                       &plants[numPlants].optimal_pH_min,
                       &plants[numPlants].optimal_pH_max,
                       &plants[numPlants].optimal_EC_min,
                       &plants[numPlants].optimal_EC_max,
                       &plants[numPlants].optimal_PPM_min,
                       &plants[numPlants].optimal_PPM_max) == 7) {
                toLowerCase(plants[numPlants].name); // Convert to lowercase for consistency
                numPlants++;
            }
            line = strtok(NULL, "\n");
        }
    }

    if (bytesRead < 0) {
        perror("Error reading file");
    }

    close(file);
    return numPlants;
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

// Function to read data from a file using system calls
void readDataFromFile() {
    int file = open("plant_data.txt", O_RDONLY);
    if (file < 0) {
        perror("Error opening file");
        return;
    }

    char buffer[MAX_LINE_LENGTH];
    ssize_t bytesRead;
    while ((bytesRead = read(file, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0'; // Null-terminate the buffer
        write(STDOUT_FILENO, buffer, bytesRead);
    }

    if (bytesRead < 0) {
        perror("Error reading file");
    }

    close(file);
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

//Function to compare resources and another things from traditional farming
void compareYieldAndWaterUsage() {
    float landArea;
    printf("\nEnter the land area (in square meters): ");
    scanf("%f", &landArea);

    
    float hydroYieldPerSqM = 4.5;  // kg
    float hydroWaterUsagePerSqM = 50.0;  // liters
    float traditionalYieldPerSqM = 2.5;  // kg
    float traditionalWaterUsagePerSqM = 200.0;  // liters

    float hydroYield = hydroYieldPerSqM * landArea;
    float hydroWaterUsage = hydroWaterUsagePerSqM * landArea;
    float traditionalYield = traditionalYieldPerSqM * landArea;
    float traditionalWaterUsage = traditionalWaterUsagePerSqM * landArea;

    printf("\nYield Comparison:\n");
    printf("Hydroponic Farming: %.2f kg\n", hydroYield);
    printf("Traditional Farming: %.2f kg\n", traditionalYield);

    printf("\nWater Usage Comparison:\n");
    printf("Hydroponic Farming: %.2f liters\n", hydroWaterUsage);
    printf("Traditional Farming: %.2f liters\n", traditionalWaterUsage);
}

int main(){
	clock_t start_time, end_time;
    double cpu_time_used;
	
	struct Plant plants[MAX_PLANTS];
    int numPlants = loadPlantsFromFile(plants, "plant_database.txt");
	
	struct User users[MAX_USERS];
    int numUsers = loadUsersFromFile(users, "user_data.txt");
    
    if (numPlants == 0) {
        printf("No plant data loaded. Exiting...\n");
        return 1;
    }
    
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
	    end_time = clock();
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
    
char plantName[50];
    float pH, EC;
    int PPM, scale;
    struct Plant *selectedPlant = NULL;

    while (1) {
        printf("Enter the plant type: ");
        scanf("%s", plantName);
        toLowerCase(plantName); 
        selectedPlant = findPlant(plants, numPlants, plantName);

        if (selectedPlant == NULL) {
            printf("Plant not found!\n");
            continue; 
        } else {
            printf("Plant '%s' found.\n", selectedPlant->name);
            getUserInput(&pH, &EC, &PPM);
            checkOptimalConditions(selectedPlant, pH, EC, PPM, checkWithinRange);
        }

        int choice;
        do {
            displayMenu();
            printf("Enter your choice: ");
            scanf("%d", &choice);

            switch (choice) {
                case 1:
                    printf("\nOptimal conditions for %s:\n", selectedPlant->name);
                    printf("pH: %.1f - %.1f\n", selectedPlant->optimal_pH_min, selectedPlant->optimal_pH_max);
                    printf("EC: %.1f - %.1f mS/cm\n", selectedPlant->optimal_EC_min, selectedPlant->optimal_EC_max);
                    printf("PPM: %d - %d\n", selectedPlant->optimal_PPM_min, selectedPlant->optimal_PPM_max);
                    break;
                case 2:
                    printf("Enter EC value (mS/cm): ");
                    scanf("%f", &EC);
                    printf("Enter scale (500 or 700): ");
                    scanf("%d", &scale);
                    printf("PPM: %d\n", ECtoPPM(EC, scale));
                    printf("Enter PPM value: ");
                    scanf("%d", &PPM);
                    printf("EC: %.2f mS/cm\n", PPMtoEC(PPM, scale));
                    break;
                case 3:
		    start_time = clock();
                    saveDataToFile(plantName, pH, EC, PPM);
		    end_time = clock();
                    cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
                    printf("CPU Time Used for saving data into file: %f seconds\n", cpu_time_used);
                    break;
                case 4:
                    readDataFromFile();
                    break;
                case 5:
                    
                    break;
                case 6:
                    compareYieldAndWaterUsage();
                    break;

                case 7:
                    printf("Exiting program.\n");
                    return 0;
                default:
                    printf("Invalid choice! Please try again.\n");
            }
        } while (choice != 5); 
    }
	return 0;
}
