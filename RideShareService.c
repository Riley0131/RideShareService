/*
Name:Riley O'Shea
Date: May 2024
Description of Program: Represents a rideshare program where the admin can setup different rideshares and the user can pick from the rideshares and it will simulate the ride, and will also retain user surveys from the rideshares.
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MIN_RAND_MINUTES_FACTOR 1.2
#define MAX_RAND_MINUTES_FACTOR 1.5
#define SENTINEL_VALUE -1
#define STRING_SIZE 80
#define MAX_ATTEMPTS 4
#define SURVEY_RIDER_ROWS 5
#define SURVEY_CATEGORIES 3
const char *surveyCategories[SURVEY_CATEGORIES] = {"Safety", "Cleanliness", "Comfort"};

#define MIN_SET_UP 0.1
#define MAX_SET_UP 50
#define MIN_MILES 1
#define MAX_MILES 100

#define CORRECT_ID "id1"
#define CORRECT_PASSCODE "ABCD"

/*
 Structures
 */

struct rideShare {
    double baseFare;
    double costPerMinute;
    double costPerMile;
    double minFlatRate;
    char rideShareName[STRING_SIZE];
    
    //holds rideShares survey
    unsigned int survey [SURVEY_RIDER_ROWS][SURVEY_CATEGORIES];
    double surveyAverages [SURVEY_CATEGORIES];
    int totalRiders;
    unsigned int surveyCount;
    
    //stores totals for rideshare
    double totalMiles;
    double totalMinutes;
    double totalFares;
};

// node with a rideshare datatype and a pointer to the next node
struct Node {
    struct rideShare rideshare;
    struct Node *nextNodePtr;
};

/*
 FUNCTION PROTOTYPES
 */

double getValidDoubleSentinel(int min, int max, int sentinel);
int calculateRandomNumber(int min, int max);
double calculateFare(double base, double cost, double mileCost, double minCharge, double miles, int minutes);
void printFare(int riderCount, double miles, int minutes, double fare);
// ITER 02 VV
bool loginAdmin(const char *correctID, const char *correctPasscode, unsigned int maxAttempts);
bool scanDouble(const char *buffer, double *output);
void fgetsRemoveEL(char *str);
double getValidDouble(int min, int max);
void getRatings(unsigned int survey[][SURVEY_CATEGORIES],unsigned int *surveyCount, size_t totalCat, const char *categoryNames[]);
void displayRideShareRatings(size_t surveyCount,unsigned int survey[][SURVEY_CATEGORIES]);
void calculateAverages(unsigned int survey[][SURVEY_CATEGORIES], double surveyCount, double *averages);
void setupRideShares(struct Node **headRef);
void riderMode(struct Node *headRef);
bool getYN(void);
// ITER02 ^^

// ITER 03 VV
void addNode(struct Node **headRef, struct rideShare rideshareNode);
void displayRideShares(struct Node *head);
// ITER 03 ^^

int main(void) {
    bool initialLogin = loginAdmin(CORRECT_ID, CORRECT_PASSCODE, MAX_ATTEMPTS);
    if (initialLogin) {
        // setup rideshares and linked list
        struct Node *head = NULL; // initiaize header
        setupRideShares(&head);
        
        //program will only continue if there are rideshares in the list
        if (head != NULL)
        {
            // Rideshares and linked list setup complete
            
            // start riderMode
            riderMode(head);
            
            //ridermode should be complete here
            
            
            
            
            //calculate averages for survey categories
            struct Node *currentPtr = head;
            while (currentPtr != NULL)
            {
                calculateAverages(currentPtr->rideshare.survey, currentPtr->rideshare.surveyCount, currentPtr->rideshare.surveyAverages);
                currentPtr = currentPtr->nextNodePtr; //increment to next node
            }
            //begin printing of rideShares to file / adminReport
            
            
            currentPtr = head; //reset currentPtr to head
            char startName[STRING_SIZE];
            strcpy(startName, "/Users/rileyoshea/CS project Files/"); //i had to change this to match so files could be created on mac

            char endName[STRING_SIZE];
            strcpy(endName, "\\.Uber.txt");

            while (currentPtr != NULL)
            {
                FILE *filePtr;
                char currentStructName[STRING_SIZE];
                
                // Makes the fileName and path for the file opening
                char currentName[STRING_SIZE];
                strcpy(currentName, startName); // Resets currentName to startName
                
                strcpy(currentStructName, currentPtr->rideshare.rideShareName); // Copies the name of the rideshare to the currentStructName variable
                
                
                
                strcat(currentName, currentStructName); // Adds the rideshare name to the string
                strcat(currentName, endName); // Adds the last part of the file name to the string
                
                //replace spaces in name with dashes
                char toReplace = ' ';
                char replaceWith = '_';
                for (size_t i = 0; i < sizeof(currentName); i++)
                {
                    if (currentName[i] == toReplace)
                    {
                        currentName[i] = replaceWith;
                    }
                }
            
                // Case where the file could be opened
                if ((filePtr = fopen(currentName, "w")) != NULL)
                {
                    if (currentPtr->rideshare.totalRiders == 0)
                    {
                        fprintf(filePtr, "There were no riders");
                    }
                    else{
                        
                        //displays totals for fare
                        fprintf(filePtr, "Riders\t  Number of Miles\t   Number of Minutes\t Ride Fare Total\n"); //prints header
                        fprintf(filePtr, "%d\t  \%.2lf\t\t   %.2f\t    %.2lf\n",currentPtr->rideshare.totalRiders, currentPtr->rideshare.totalMiles, currentPtr->rideshare.totalMinutes, currentPtr->rideshare.totalFares);
                        
                        //display category averages
                        for (int i = 0; i < SURVEY_CATEGORIES; i++)
                        {
                            double currentCategoryAverage = currentPtr->rideshare.surveyAverages[i];
                            fprintf(filePtr, "Category Averages\n");
                            fprintf(filePtr, "%d.%s\n %.2f\n",i+1, surveyCategories[i], currentCategoryAverage);
                        }
                        
                    }
                    fclose(filePtr); // Close the file after writing
                }
                else
                {
                    puts("The file could not be opened");
                }
                
                currentPtr = currentPtr->nextNodePtr; // Move to the next node
            }

            
            //clears all memory from nodes
            currentPtr = head;
            struct Node* next;
            while (currentPtr != NULL)
            {
                next = currentPtr->nextNodePtr;
                free(currentPtr);
                currentPtr = next;
            }
        }
        
        
    }
} // main

/*
Returns: a double within range or the sentinel value
Parameters: needs a minimum - maximum value along with the sentinel value passed to the function
Description of Function: gets a double from the user and checks if that double is the sentinel value
*/
double getValidDoubleSentinel(int min, int max, int sentinel) {
    bool isValid = false;
    int scanfReturn = 0;
    double validDouble = 0;
    
    // iterates until isValid is true
    while (isValid != true) {
        scanfReturn = scanf("%lf", &validDouble);
        
        while (getchar() != '\n'); // clear input buffer
        
        if (scanfReturn == 1) {
            if ((validDouble >= min && validDouble <= max) ||
                validDouble == sentinel) {
                // set flag to true since it is in range
                isValid = true;
            } else {
                printf("Error: not in range %d - %d\n please try again:", min, max);
            }
        } // if (scanfReturn ==1)
        else {
            puts("Error: not an integer value. Please try again:");
        }
    } // while
    return validDouble;
} // getValidDouble

/*
Returns: a random integer value between the min and max
Parameters: minimum value and maximum value
Description of Function: calculates a random number between the min and max
*/
int calculateRandomNumber(int min, int max) {
    int randNum = rand() % (max - min + 1) + min;
    return randNum;
} // calculateRandomNumber

/*
Returns:a double which represents the fare cost
Parameters:needs to be sent the baseFare, costPerMinute, costPerMile, minimumCharge, miles, and minutes for the trip
 calculates the fare based on the miles and minutes from the rider
Description of Function:
*/
double calculateFare(double base, double minuteCost, double mileCost, double minCharge, double miles, int minutes) {
    double fare = base + (minuteCost * minutes) + (mileCost * miles);
    
    // if the fare calculates is less than the mincharge it sets the fare to the
    // minimum charge
    if (fare <= minCharge) {
        fare = minCharge;
    }
    return fare;
} // calculateFare


/*
Returns: Count for how many riders have been on the rideShare
Parameters: needs to be sent all the values which are gonna be printed
Description of Function: displays the fare to the user and also will display the total fares to the admin in a formatted output
*/
void printFare(int riderCount, double miles, int minutes, double fare) {
    puts("");
    
    // case if there are no riders
    if (riderCount == 0) {
        puts("There were no riders");
    } else {
        puts("Rider     \tNumber of miles       \tNumber of minutes         \tRide "
             "Fare Ammount");
        printf("%d      \t\t%.1lf                     %d                           "
               "   %.2lf\n",
               riderCount, miles, minutes, fare);
    }
} // printFare


/*
Returns: the value of if a login by the admin was successful
Parameters: pass a pointer to the correct id and passcode and how many attempts are allowed.
Description of Function: has admin login and checks if the login was successful
*/
bool loginAdmin(const char *correctID, const char *correctPasscode,
                unsigned int maxAttempts) {
    bool attemptResult = false;
    unsigned int attempts = 0;
    
    // runs for as many attempts are allowed or until the login is successful
    while (attemptResult != true && attempts < maxAttempts) {
        printf("Please enter the admin ID: "); // used printf to keep user entry in
        // line
        char userID[STRING_SIZE];
        fgetsRemoveEL(userID); // read id from user
        
        // password
        printf("Please enter the passcode: ");
        char userPass[STRING_SIZE];
        
        fgetsRemoveEL(userPass);
        
        // compare the id to the correctID
        int idComp = strcmp(userID, correctID);
        // compare password to correctPassowrd
        int passComp = strcmp(userPass, correctPasscode);
        
        // if both ID and Pass match
        if (idComp == 0 && passComp == 0) {
            puts("Login successful");
            attemptResult = true;
            
            attempts++; // increment attempts
        }
        // if id is incorrect but pass is correct
        else if (idComp != 0 && passComp == 0) {
            puts("ID Incorrect...Try again");
            attempts++; // increment attempts
        }
        // if id is correct but pass is incorrect
        else if (idComp == 0 && passComp != 0) {
            puts("Password incorrect...Try again");
            attempts++; // increment attempts
        } else {
            puts("ID and Password incorrect... Try again");
            attempts++; // increment attempts
        }
    } // while
    
    // tells user that there were too many invalid attempts
    if (attempts == 4 && attemptResult != true) {
        puts("Too many invalid attempts.");
    }
    
    return attemptResult;
} // adminLogin func

//

/*
Returns:boolean flag that a double was found
Parameters: string that is being adjusted
Description of Function:gets a double from the user and checks that it is a double then changes the double it was passed
*/
bool scanDouble(const char *buffer, double *output) {
    
    char *end;
    double testNum = strtod(buffer, &end);
    
    bool isValid = true;
    if (buffer == end) {
        isValid = false;
    } else if (*end != '\0') {
        isValid = false;
    }
    
    if (isValid) {
        *output = testNum;
    }
    return isValid;
} // scanDouble func


/*
Returns:nothing
Parameters:send a string to change
Description of Function: gets a string and removes the endline character from the string
*/
void fgetsRemoveEL(char *str) {
    if (fgets(str, STRING_SIZE, stdin) != NULL) {
        size_t length = strlen(str);
        if (str[length - 1] == '\n') {
            str[length - 1] = '\0'; // replace newline with endline
        }
    }
} // fgetsRemoveEL


/*
Returns:a double
Parameters:send the min and max for the double
Description of Function: gets a double and checks that it is in range
*/
double getValidDouble(int min, int max) {
    double validDouble = 0;
    char input[STRING_SIZE];
    bool isValid = false;
    
    // loops until the double is valid
    while (!isValid) {
        fgetsRemoveEL(input);
        
        if (scanDouble(input, &validDouble)) {
            // case if the double is in the desired range
            if (validDouble >= min && validDouble <= max) {
                isValid = true;
            } else {
                printf("Error: not in range %d - %d", min, max);
            }
        } else {
            puts("Error: not a valid number");
        }
    } // while
    return validDouble;
} // getValidDouble


/*
Returns:none
Parameters: send the survey to change and the category names
Description of Function:prompts the user for their rating for each category and stores it in the survey array
*/
void getRatings(unsigned int survey[][SURVEY_CATEGORIES], unsigned int *surveyCount, size_t totalCat, const char *categoryNames[])
{
    puts("We want to know how your experience was on your ride today. Please rate the following categories:");
    for (size_t categories = 0; categories < totalCat; categories++) 
    {
        printf("Please enter your rating for %s (1-5): ", categoryNames[categories]);
        survey[*surveyCount][categories] = getValidDouble(1, 5);
    }
    
} // getRatings func


/*
Returns:none
Parameters: send the survey count and the survey array for printing
Description of Function:will print all of the ratings that there are in the survey array to the user
*/
void displayRideShareRatings(size_t surveyCount, unsigned int survey[][SURVEY_CATEGORIES]) {
    // case if there are no surveys yet
    if (surveyCount <= 0) {
        puts("No surveys");
    } 
    else {
        // display categories
        for (size_t z = 0; z < SURVEY_CATEGORIES; z++) {
            printf("\t%s", surveyCategories[z]);
        }
        puts("");
        
        // display ratings for each survey
        for (size_t i = 0; i < surveyCount; i++) {
            printf("Survey %zu: ", i + 1);
            for (size_t j = 0; j < SURVEY_CATEGORIES; j++) {
                printf("%.2u\t\t", survey[i][j]);
            }
            puts("");
        }
        puts("\n");
    }
}

/*
Returns:None
Parameters:send the survey and the averages array to change
Description of Function:calculates the averages of each category by the columns in the survey array
*/
void calculateAverages(unsigned int survey[][SURVEY_CATEGORIES], double surveyCount, double *averages) {
    // Set averages to 0
    for (size_t i = 0; i < SURVEY_CATEGORIES; i++) {
        averages[i] = 0;
    }
    
    // Calculate sum of ratings and store in averages temporarily
    for (size_t i = 0; i < surveyCount; i++) {
        for (size_t j = 0; j < SURVEY_CATEGORIES; j++) {
            averages[j] += survey[i][j];
        }
    }
    
    // Calculate average
    for (size_t i = 0; i < SURVEY_CATEGORIES; i++) {
        averages[i] /= surveyCount;
    }
}
// calculateAverages func


/*
Returns:none
Parameters:send the head of the list
Description of Function: sets up each rideshare node and adds it to a linked list
*/
void setupRideShares(struct Node **headRef) {
    puts("\nWould you like to set up rideShares? (Y or N)");
    bool addRideShares = getYN();
    struct rideShare currentRideShare; // create a rideshare instance
    while (addRideShares != false) {
        while (getchar() != '\n')
            ; // clear input buffer
        puts("Enter the base fare: ");
        double baseFare = getValidDouble(MIN_SET_UP, MAX_SET_UP);
        currentRideShare.baseFare = baseFare;
        
        puts("Enter the cost per minute: ");
        double costPerMinute = getValidDouble(MIN_SET_UP, MAX_SET_UP);
        currentRideShare.costPerMinute = costPerMinute;
        
        puts("Enter the cost per mile: ");
        double costPerMile = getValidDouble(MIN_SET_UP, MAX_SET_UP);
        currentRideShare.costPerMile = costPerMile;
        
        puts("Enter the minimum flat rate: ");
        double minFlatRate = getValidDouble(MIN_SET_UP, MAX_SET_UP);
        currentRideShare.minFlatRate = minFlatRate;
        
        puts("Enter the rideshare name: ");
        fgetsRemoveEL(currentRideShare.rideShareName);
        
        //sets the other values to 0
        currentRideShare.totalRiders = 0;
        currentRideShare.surveyCount = 0;
        currentRideShare.totalMiles = 0;
        currentRideShare.totalMinutes = 0;
        currentRideShare.totalFares = 0;
        
        // adds the currentRideShare to the linkedList
        addNode(headRef, currentRideShare);
        
        // displays rideshare information
        printf("\n%s\nbaseFare=%.2f\ncostPerMinute=%.2f\ncostPerMile=%."
               "2f\nminFlatRate=%.2f\n",
               currentRideShare.rideShareName, currentRideShare.baseFare,
               currentRideShare.costPerMinute, currentRideShare.costPerMile,
               currentRideShare.minFlatRate);
        
        // asks the admin if they would like to continue creating rideshares
        puts("Would you like to add another rideshare? (Y or N)");
        addRideShares = getYN();
        
    } // while loop
} // setupRideShares func


/*
Returns:none
Parameters:send the reference to the head of the linked list
Description of Function: simulates the riderMode by having the user select a rideshare and define how many miles they are traveling, then it calculates the cost and asks them to add survey ratings
*/
void riderMode(struct Node *headRef) {
    bool rideShareSelected = false;
    
    struct Node *currentNode = headRef;
    bool adminLogin = false;
    
    //continues until the admin logs in
    while (!adminLogin){
        while (getchar() != '\n'); //clear input buffer
        
        // displays all rideshares to the user like a menu
        puts("\n\nPlease Select a Rideshare from the following:\n\n");
        displayRideShares(headRef);
        
        
        
        puts("Enter the name of the rideshare you would like to use:");
        //while (getchar() != '\n');// clear input buffer
        
        
        //clear the string it had some issues where it was storing some values from the previous entry
        char selectedRideShareName[STRING_SIZE];
        
        
        fgetsRemoveEL(selectedRideShareName); // gets user input for rideshare
        currentNode = headRef;
        
        
        //searches the linked list for the rideshare the user entered
        while (!rideShareSelected) {
            
            
            // iterates through the list
            while (currentNode != NULL && rideShareSelected != true) {
                
                
                
                // sets boolean flag if the name matches the current element in list
                if (strcasecmp(selectedRideShareName, currentNode->rideshare.rideShareName) == 0) 
                {
                    rideShareSelected = true;
                }
                // continues on if the current node doesnt match
                else {
                    if (currentNode->nextNodePtr != NULL) 
                    {
                        currentNode = currentNode->nextNodePtr;
                    } 
                    //case where there are no more nodes in the list
                    else
                    {
                        puts("Invalid RideShare name, please try again");
                        fgetsRemoveEL(selectedRideShareName);
                        currentNode = headRef;
                    }
                }
            }
            
        } // finds the rideShare the user wants to select
        

        // At this point a rideShare has been selected
        
        // get users miles and other calculations
        printf("Enter the number of miles for your trip (%d - %d): ", MIN_MILES,
               MAX_MILES);
        double miles = getValidDoubleSentinel(MIN_MILES, MAX_MILES, SENTINEL_VALUE);
        if (miles != SENTINEL_VALUE) {
            currentNode->rideshare.totalRiders++; // increments riders
            unsigned int randMin = MIN_RAND_MINUTES_FACTOR * miles;
            unsigned int randMax = MAX_RAND_MINUTES_FACTOR * miles;
            int minutes = calculateRandomNumber(randMin, randMax);
            double currentFare = calculateFare(currentNode->rideshare.baseFare, currentNode->rideshare.costPerMinute, currentNode->rideshare.costPerMile, currentNode->rideshare.minFlatRate, miles, minutes);
            
            printFare(currentNode->rideshare.totalRiders, miles, minutes, currentFare);
            
            // update totals
            currentNode->rideshare.totalMiles += miles;
            currentNode->rideshare.totalMinutes += minutes;
            currentNode->rideshare.totalFares += currentFare;
            
            puts("Would you like to enter survey ratings? (Y/N):");
            
            bool enterRatings = getYN();
            if (enterRatings == true) {
                
                while (getchar() != '\n'); // clear input buffer
                
                getRatings(currentNode->rideshare.survey, &currentNode->rideshare.surveyCount, SURVEY_CATEGORIES, surveyCategories);
                
                currentNode->rideshare.surveyCount++;
            }
        }
        if (miles == SENTINEL_VALUE)
        {
            adminLogin = loginAdmin(CORRECT_ID, CORRECT_PASSCODE, MAX_ATTEMPTS);
        }
        rideShareSelected = false; //resets rideShareselected to false so that it will find the users rideshare again
    }
} // riderMode Func


/*
Returns:boolean for if the user selected yes
Parameters:none
Description of Function: gets the user to answer yes or no to the question being asked
*/
bool getYN(void) {
    bool answer = false;
    char input = getchar();
    bool yn = false; //tracks if the user actually entered a y or a n
   
    while (!yn)
    {
        if (input == '\n') 
        {
            input = getchar();
        }
        
        // case where user answers yes
        if (input == 'y' || input == 'Y') {
            answer = true;
            yn = true;
        }
        //case where user answers no
        else if (input == 'n' || input == 'N')
        {
            answer = false;
            yn = true;
        }
        //repeats question until user answers y or n
        else{
            puts("You did not enter a y or n");
            input = getchar();
        }
        
    }
    
    
    
    return answer;
} // getYN func

/*
Returns:none
Parameters: send the head of the list and the rideshare node to add
Description of Function: adds a rideshare node to the linked list in alphabetical order
*/
void addNode(struct Node **headRef, struct rideShare rideshareNode) {
    struct Node *newNodePtr = (struct Node *)malloc(
                                                    sizeof(struct Node)); // allocates memory for the new node
    
    // runs if there is memory for the newNode
    if (newNodePtr != NULL) {
        newNodePtr->rideshare =
        rideshareNode; // sets the data in the node to the rideshare being sent
        // to this function
        newNodePtr->nextNodePtr = NULL; // sets next to null
        
        struct Node *previousPtr = NULL; // not sure if i need to keep this if the
        // list needs to be doublylinked
        struct Node *currentPtr =
        *headRef; // sets the current pointer to the head of the list?
        
        // ensures the node is added to the correct position based on alphabethical
        // order used strcasecmp so that rideshares are ordered by alphabetical and
        // not by case
        while (currentPtr != NULL && strcasecmp(currentPtr->rideshare.rideShareName,
                                                rideshareNode.rideShareName) < 0) {
            previousPtr = currentPtr;
            currentPtr = currentPtr->nextNodePtr;
        }
        if (previousPtr == NULL) {
            *headRef = newNodePtr;
        } else {
            previousPtr->nextNodePtr = newNodePtr;
        }
        
        newNodePtr->nextNodePtr = currentPtr;
        
    } // if
    
    // displays error to user if there is no memory for the new node
    else {
        puts("No memory for new node");
    }
    
} // addnode func

/*
Returns:none
Parameters: head of the linked list
Description of Function: displays all of the rideshares their costs and their ratings
*/
void displayRideShares(struct Node *head) {
    struct Node *currentPtr = head;
    
    while (currentPtr != NULL) {
        printf("Ride Share: %s\n", currentPtr->rideshare.rideShareName);
        printf("Base Fare: %.2f\n", currentPtr->rideshare.baseFare);
        printf("Cost Per Minute: %.2f\n", currentPtr->rideshare.costPerMinute);
        printf("Cost Per Mile: %.2f\n", currentPtr->rideshare.costPerMile);
        printf("Minimum Flat Rate: %.2f\n\n", currentPtr->rideshare.minFlatRate);
        
        
        
        
        //display ratings for current rideShare
        size_t tempCount = currentPtr->rideshare.surveyCount;
        displayRideShareRatings(tempCount, currentPtr->rideshare.survey);
        
        currentPtr = currentPtr->nextNodePtr; //increment to the next node in the list
        
        puts("\n"); //print space to format rideshare
    }
} // displayRideShares func
