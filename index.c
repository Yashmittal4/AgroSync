#include <stdio.h>
#include <string.h>
#include <stdlib.h>


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
	
	
	
	return 0;
}
