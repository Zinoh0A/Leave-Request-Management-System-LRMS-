/*use this programme for inspo and some good ideas to take from it (the creator: Zaynab Ait Addi) some of my infos in this code ,U can find it easily >o<*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>   // For seeding rand()
// Function to create the data directory if it doesn't exist
#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#ifdef _WIN32
#include <windows.h>
#endif

#include <ctype.h>
// Define ANSI color codes for styling
#define RESET "\033[0m"
// Text colors
#define BLACK "\033[30m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define GRAY "\033[90m"
#define BRIGHT_RED "\033[91m"
#define BRIGHT_GREEN "\033[92m"
#define BRIGHT_YELLOW "\033[93m"
#define BRIGHT_BLUE "\033[94m"
#define BRIGHT_MAGENTA "\033[95m"
#define BRIGHT_CYAN "\033[96m"
#define BRIGHT_WHITE "\033[97m"

// Background colors
#define BG_BLACK "\033[40m"
#define BG_RED "\033[41m"
#define BG_GREEN "\033[42m"
#define BG_YELLOW "\033[43m"
#define BG_BLUE "\033[44m"
#define BG_MAGENTA "\033[45m"
#define BG_CYAN "\033[46m"
#define BG_WHITE "\033[47m"
#define BG_GRAY "\033[100m"
#define BG_BRIGHT_RED "\033[101m"
#define BG_BRIGHT_GREEN "\033[102m"
#define BG_BRIGHT_YELLOW "\033[103m"
#define BG_BRIGHT_BLUE "\033[104m"
#define BG_BRIGHT_MAGENTA "\033[105m"
#define BG_BRIGHT_CYAN "\033[106m"
#define BG_BRIGHT_WHITE "\033[107m"
#define BG_DEFAULT "\033[49m"


#define MAX_USERS 100
#define DATA_DIR "data"  // Directory(Dossier) to store all files
#define USER_FILE "data/users.dat"  // User file inside the 'data' directory
#define LEAVE_FILE "data/leave_requests.dat"  // Leave request file inside the 'data' directory
#define PENDING_USER_FILE "data/pending_users.dat"  // Pending users file inside the 'data' directory

// Function for masking password input
#ifdef _WIN32
#include <conio.h> // For getch() on Windows
#else
#include <termios.h>
#include <unistd.h>
char getch() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
#endif

#define Annual_LEAVE_LIMIT 22

 struct User {
    char ID[50];               // User ID
    char password[20];         // Password
    char first_name[50];       // First name
    char last_name[50];        // Last name
    char gender[2];            // Gender ("M" or "F" with null terminator)
    char adress[100];          // Address
    char birth_day[11];        // Birth date (format: DD/MM/YYYY + '\0')
    char recruitment_day[11];  // Recruitment date (format: DD/MM/YYYY + '\0')
    char departement[20];      // Department
    char tele_num[20];         // Phone number
    char email[100];           // Email
    int Annual_leave_bonus;    // Annual leave bonus
    char question1[100];       // Security question 1
    char answer1[100];         // Answer 1
    char question2[100];       // Security question 2
    char answer2[100];         // Answer 2
    int signupstate;           // 3 for still under process .1 for Approved .0 for Rejected
};


// Leave request structure
struct LeaveRequest {
    int ID_leave;
    char type[20]; // Annual or Urgent
    char title[50];
    char date[15];
    char content[200];
    char ID_Employee[20];
    char first_name[50];       // First name
    char last_name[50];        // Last name
    char status[20]; // e.g., "under process", "Approved", "Rejected"
    int days; // Number of days requested
};

// Function to clear the screen
void clearScreen() {
    // Cross-platform way to clear the screen
    #ifdef _WIN32
        system("cls");
    #else
        printf("\033[2J\033[H");  // ANSI escape codes for clearing the screen
    #endif
}
#include <windows.h>
// Function to center text on the screen and adapt to terminal size
void printCentered(const char *text, const char *textColor, const char *bgColor) {
    #ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        int width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    #else
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        int width = w.ws_col;
    #endif

    int len = strlen(text);
    int padding = (width - len) / 2;
    padding = padding < 0 ? 0 : padding; // Ensure padding isn't negative

    // Print spaces for centering
    for (int i = 0; i < padding; i++) {
        printf(" ");
    }

    // Print the text with the specified colors
    printf("%s%s%s%s\n", textColor, bgColor, text, RESET);
}

void createDirectoryIfNotExist() {
    struct stat st = {0};

    // Check if the directory exists
    if (stat(DATA_DIR, &st) == -1) {
        #ifdef _WIN32
            if ( mkdir(DATA_DIR) == -1) {  // Windows version of mkdir takes one argument
                printf("Error creating data directory.\n");
                exit(1);
            }
        #else
            if (mkdir(DATA_DIR, 0700) == -1) {  // Unix/Linux version with permissions
                printf("Error creating data directory.\n");
                exit(1);
            }
        #endif
        printf("Data directory created: %s\n", DATA_DIR);
    }
}

// Function to create the files if they don't exist
void createFilesIfNotExist() {
    FILE *file;

    // Ensure the directory exists before creating files
    createDirectoryIfNotExist();

    // Check and create the USER_FILE if it doesn't exist
    file = fopen(USER_FILE, "rb");  // Try to open in read binary mode
    if (file == NULL) {
        file = fopen(USER_FILE, "wb");  // Create the file in write binary mode if it doesn't exist
        if (file == NULL) {
            printf("Error creating user file.\n");
            exit(1);  // Exit if file creation fails
        }
        printf("User file created: %s\n", USER_FILE);
    } else {
        fclose(file);  // Close the file if it already exists
    }

    // Check and create the LEAVE_FILE if it doesn't exist
    file = fopen(LEAVE_FILE, "rb");  // Try to open in read binary mode
    if (file == NULL) {
        file = fopen(LEAVE_FILE, "wb");  // Create the file in write binary mode if it doesn't exist
        if (file == NULL) {
            printf("Error creating leave request file.\n");
            exit(1);  // Exit if file creation fails
        }
        printf("Leave request file created: %s\n", LEAVE_FILE);
    } else {
        fclose(file);  // Close the file if it already exists
    }

    // Check and create the PENDING_USER_FILE if it doesn't exist
    file = fopen(PENDING_USER_FILE, "rb");  // Try to open in read binary mode
    if (file == NULL) {
        file = fopen(PENDING_USER_FILE, "wb");  // Create the file in write binary mode if it doesn't exist
        if (file == NULL) {
            printf("Error creating pending user file.\n");
            exit(1);  // Exit if file creation fails
        }
        printf("Pending user file created: %s\n", PENDING_USER_FILE);
    } else {
        fclose(file);  // Close the file if it already exists
    }
}
// Function to ensure the Manager exists in the file
void ensureManagerExists() {
    FILE *file = fopen(USER_FILE, "ab+");  // Open the user file in append-binary mode
    if (file == NULL) {
        printf("Error opening user data file.\n");
        return;
    }

    struct User tempUser;
    int ManagerExists = 0;

    // Check if the Manager already exists in the file
    while (fread(&tempUser, sizeof(struct User), 1, file)) {
        if (strcmp(tempUser.ID, "Manager12345") == 0) {
            ManagerExists = 1;  // Manager exists
            break;
        }
    }

    // If the Manager doesn't exist, add their details
    if (!ManagerExists) {
        struct User newManager = {
            .ID = "Manager12345",
            .password = "12345",
            .first_name = "John",
            .last_name = "Doe",
            .gender = "M",
            .adress = "123 Main Street",
            .birth_day = "01/01/1980",
            .recruitment_day = "01/01/2024",
            .departement = "HR",
            .tele_num = "+1234567890",
            .email = "Manager@example.com",
            .Annual_leave_bonus = Annual_LEAVE_LIMIT,
            .question1 = "what is your best friend name?",
            .answer1 = "Joe",
            .question2 = "what is favourite movie?",
            .answer2 = "Totoro",
            .signupstate = 1
        };
        fwrite(&newManager, sizeof(struct User), 1, file);
    }

    fclose(file);
}


int generateUniqueNumber() {
    FILE *userFile = fopen(USER_FILE, "rb");
    FILE *pendingFile = fopen(PENDING_USER_FILE, "rb");
    int uniqueNumber;
    int isUnique;

    do {
        uniqueNumber = rand() % 90000 + 10000;  // Generate a random 5-digit number
        isUnique = 1;

        // Check for uniqueness in both files
        if (userFile != NULL) {
            struct User tempUser;
            while (fread(&tempUser, sizeof(struct User), 1, userFile)) {
                if (atoi(tempUser.ID + strlen(tempUser.ID) - 5) == uniqueNumber) {
                    isUnique = 0;
                    break;
                }
            }
        }

        if (pendingFile != NULL) {
            struct User tempUser;
            while (fread(&tempUser, sizeof(struct User), 1, pendingFile)) {
                if (atoi(tempUser.ID + strlen(tempUser.ID) - 5) == uniqueNumber) {
                    isUnique = 0;
                    break;
                }
            }
        }

    } while (!isUnique);

    fclose(userFile);
    fclose(pendingFile);

    return uniqueNumber;
}

void writeUserToFile(FILE *file, struct User *user) {
    fwrite(user, sizeof(struct User), 1, file);
}

int readUserFromFile(FILE *file, struct User *user) {
    return fread(user, sizeof(struct User), 1, file);
}

// Function to delete a user from the file by ID
void deleteUser() {
    FILE *userFile = fopen(USER_FILE, "r+b");
    FILE *tempFile = fopen("temp_users.dat", "wb");
    struct User tempUser;
    char userIDToDelete[50];
    int found = 0;

    if (userFile == NULL || tempFile == NULL) {
        printCentered("Error opening user files.", RED, BG_DEFAULT);
        if (userFile) fclose(userFile);
        if (tempFile) fclose(tempFile);
        return;
    }

    // Ask for the ID of the user to delete
    printCentered("\nEnter the User ID to delete(0 to cancel): ", CYAN, BG_DEFAULT);
    scanf("%s", userIDToDelete);

    // Rewind the file and search for the user with the matching ID
    rewind(userFile);
    while (fread(&tempUser, sizeof(struct User), 1, userFile) == 1) {
        if (strcmp(tempUser.ID, userIDToDelete) == 0) {
            found = 1;
            // Check if the user is the manager before proceeding
            if (strcmp(userIDToDelete, "Manager12345") == 0) {
                printCentered("You cannot delete the Manager (ID: Manager12345).", RED, BG_DEFAULT);
                fclose(userFile);
                fclose(tempFile);
                pauseForEnter();
                return;  // Exit the function if trying to delete Manager12345
            }

            // User found, delete this user (don't write to temp file)
            printCentered("User Found. Deleting now...", GREEN, BG_DEFAULT);
            pauseForEnter();
        } else {
            // Write all users back to the temp file except the deleted one
            fwrite(&tempUser, sizeof(struct User), 1, tempFile);
        }
    }

    if (!found) {
        char notFoundMsg[100];
        snprintf(notFoundMsg, sizeof(notFoundMsg), "No user found with ID: %s", userIDToDelete);
        printCentered(notFoundMsg, RED, BG_DEFAULT);
        pauseForEnter();
    }

    // Close both files
    fclose(userFile);
    fclose(tempFile);

    // Replace the original user file with the updated temp file
    remove(USER_FILE);
    rename("temp_users.dat", USER_FILE);
}

// Function to check if a user already exists in the file
int doesUserExist(FILE *file, const char *userID) {
    struct User tempUser;
    rewind(file); // Go to the beginning of the file

    while (fread(&tempUser, sizeof(struct User), 1, file)) {
        if (strcmp(tempUser.ID, userID) == 0) {
            return 1; // User exists
        }
    }
    return 0; // User does not exist
}

// Function to check if a leave request already exists for a user
int doesLeaveExist(FILE *file, const char *userID, int leaveID) {
    struct LeaveRequest tempLeave;
    rewind(file); // Go to the beginning of the file

    while (fread(&tempLeave, sizeof(struct LeaveRequest), 1, file)) {
        if (strcmp(tempLeave.ID_Employee, userID) == 0 && tempLeave.ID_leave == leaveID) {
            return 1; // Leave request exists
        }
    }
    return 0; // Leave request does not exist
}

// Function to add users and leave requests only if they do not exist
void addUsersAndLeaves() {
    FILE *userFile = fopen(USER_FILE, "ab+");
    FILE *leaveFile = fopen(LEAVE_FILE, "ab+");

    if (!userFile || !leaveFile) {
        printf("Error opening user or leave files.\n");
        if (userFile) fclose(userFile);
        if (leaveFile) fclose(leaveFile);
        return;
    }

    // Define new users
    struct User newUsers[] = {
        {"Employee11111", "12345", "Zaynab", "AitAddi", "F", "Ouled Teima", "15/08/2004", "10/12/2024", "TDIA Engineer", "+212 642786170", "Zaynabaitaddi@outlook.com", 22, "Who are u?", "The creator of this programme,Use this programme for inspo and making something more impressive", "Your favorite color?", "Blue", 1},
        {"Employee12347", "12345", "Bob", "Smith", "M", "456 Orange St", "03/04/1985", "15/02/2023", "Finance", "0987654321", "bob.smith@example.com", 22, "Your best friend?", "Jim", "Your birthplace?", "LA", 1},
        {"Employee12348", "12345", "Carol", "Davis", "F", "789 Peach St", "05/06/1988", "20/03/2023", "Marketing", "1122334455", "carol.davis@example.com", 22, "Your hobby?", "Reading", "Your school?", "Highland", 1},
        {"Employee12349", "12345", "David", "Jones", "M", "101 Pine St", "12/05/1992", "01/06/2023", "Sales", "4455667788", "david.jones@example.com", 22, "Your first car?", "Toyota", "Your favorite food?", "Pizza", 1},
        {"Employee12350", "12345", "Emma", "White", "F", "202 Oak St", "23/07/1987", "15/07/2023", "IT", "5566778899", "emma.white@example.com", 22, "Your favorite movie?", "Matrix", "Your first pet?", "Max", 1},
        {"Employee12351", "12345", "Frank", "Miller", "M", "303 Elm St", "14/09/1991", "01/08/2023", "Finance", "6677889900", "frank.miller@example.com", 22, "Your favorite sport?", "Soccer", "Your hometown?", "Miami", 1},
        {"Employee12352", "12345", "Grace", "Lee", "F", "404 Cedar St", "30/11/1989", "15/09/2023", "Marketing", "7788990011", "grace.lee@example.com", 22, "Your favorite teacher?", "Mrs. Smith", "Your first job?", "Cashier", 1},
        {"Employee12353", "12345", "Henry", "Clark", "M", "505 Maple St", "25/03/1993", "01/10/2023", "Sales", "8899001122", "henry.clark@example.com", 22, "Your favorite place?", "Paris", "Your mother's name?", "Mary", 1},
        {"Employee12354", "12345", "Iris", "Anderson", "F", "606 Birch St", "17/08/1986", "15/11/2023", "IT", "9900112233", "iris.anderson@example.com", 22, "Your dream vacation?", "Hawaii", "Your first phone?", "Nokia", 1},
        {"Employee12355", "12345", "Jack", "Martin", "M", "707 Walnut St", "09/12/1990", "01/12/2023", "Finance", "0011223344", "jack.martin@example.com", 22, "Your favorite band?", "Beatles", "Your lucky number?", "7", 1},
        {"Employee12356", "12345", "Kelly", "Moore", "F", "808 Pine St", "28/02/1988", "15/01/2024", "Marketing", "1122334455", "kelly.moore@example.com", 22, "Your favorite season?", "Summer", "Your idol?", "Einstein", 1},
        {"Employee12357", "12345", "Leo", "Taylor", "M", "909 Oak St", "19/06/1992", "01/02/2024", "Sales", "2233445566", "leo.taylor@example.com", 22, "Your favorite game?", "Chess", "Your first school?", "Lincoln", 1},
        {"Employee12358", "12345", "Maya", "Wilson", "F", "1010 Elm St", "11/10/1987", "15/03/2024", "IT", "3344556677", "maya.wilson@example.com", 22, "Your role model?", "Mom", "Your favorite book?", "1984", 1},
        {"Employee12359", "12345", "Noah", "Brown", "M", "1111 Cedar St", "03/04/1991", "01/04/2024", "Finance", "4455667788", "noah.brown@example.com", 22, "Your dream car?", "Tesla", "Your favorite city?", "NYC", 1},
        {"Employee12360", "12345", "Olivia", "Davis", "F", "1212 Maple St", "22/09/1989", "15/05/2024", "Marketing", "5566778899", "olivia.davis@example.com", 22, "Your favorite actor?", "Pitt", "Your first pet?", "Kitty", 1},
        {"Manager12346", "54321", "Peter", "Johnson", "M", "1313 Birch St", "14/07/1980", "01/06/2024", "IT", "6677889900", "peter.johnson@example.com", 22, "Your mentor?", "Dad", "Your favorite sport?", "Golf", 1},
        {"Manager12347", "54321", "Quinn", "Roberts", "F", "1414 Walnut St", "05/11/1978", "15/07/2024", "Operations", "7788990011", "quinn.roberts@example.com", 22, "Your first car?", "Honda", "Your favorite food?", "Sushi", 1},
        {"Manager12348", "54321", "Ryan", "Thompson", "M", "1515 Pine St", "16/01/1982", "01/08/2024", "Executive", "8899001122", "ryan.thompson@example.com", 22, "Your favorite place?", "London", "Your hobby?", "Reading", 1}
    };
    struct LeaveRequest leaveRequests[] = {
        {1, "Urgent", "Vacation", "2018-01-15", "Urgent vacation need", "Employee12346", "Alice", "Brown", "Approved", 5},
        {2, "Urgent", "Medical", "2019-06-15", "Medical appointment", "Employee12347", "Bob", "Smith", "Approved", 2},
        {3, "Urgent", "Family", "2020-02-20", "Family emergency", "Employee12348", "Carol", "Davis", "Approved", 3},
        {4, "Urgent", "Training", "2020-02-20", "Immediate training need", "Employee12349", "David", "Jones", "Approved", 4},
        {5, "Urgent", "Emergency", "2021-03-10", "Personal emergency", "Employee12350", "Emma", "White", "Approved", 2},
        {6, "Annual", "Vacation", "2021-09-10", "Planned vacation", "Employee12351", "Frank", "Miller", "Approved", 10},
        {7, "Urgent", "Medical", "2022-04-05", "Medical checkup", "Employee12352", "Grace", "Lee", "Under Process", 3},
        {8, "Urgent", "Family", "2022-04-25", "Family crisis", "Employee12353", "Henry", "Clark", "Approved", 5},
        {9, "Urgent", "Training", "2022-05-12", "Critical training", "Employee12354", "Iris", "Anderson", "Approved", 4},
        {10, "Urgent", "Emergency", "2022-05-12", "Family emergency", "Employee12355", "Jack", "Martin", "Approved", 3},
        {11, "Urgent", "Medical", "2023-06-18", "Urgent health matter", "Employee12356", "Kelly", "Moore", "Under Process", 7},
        {12, "Urgent", "Medical", "2023-06-18", "Dental emergency", "Employee12357", "Leo", "Taylor", "Approved", 1},
        {13, "Urgent", "Family", "2023-07-22", "Urgent family matter", "Employee12358", "Maya", "Wilson", "Under Process", 5},
        {14, "Annual", "Training", "2023-07-22", "Scheduled conference", "Employee12359", "Noah", "Brown", "Approved", 3},
        {15, "Urgent", "Emergency", "2023-08-30", "Personal matter", "Employee12360", "Olivia", "Davis", "Approved", 2},
        {16, "Urgent", "Medical", "2023-08-30", "Health emergency", "Employee12346", "Alice", "Brown", "Rejected", 4},
        {17, "Urgent", "Medical", "2023-09-14", "Hospital visit", "Employee12347", "Bob", "Smith", "Approved", 2},
        {18, "Urgent", "Family", "2023-09-14", "Urgent family situation", "Employee12348", "Carol", "Davis", "Approved", 3},
        {19, "Urgent", "Training", "2024-01-27", "Urgent certification", "Employee12349", "David", "Jones", "Under Process", 5},
        {20, "Urgent", "Emergency", "2024-01-27", "Home emergency", "Employee12350", "Emma", "White", "Approved", 2},
        {21, "Urgent", "Medical", "2024-02-08", "Medical emergency", "Employee12351", "Frank", "Miller", "Approved", 6},
        {22, "Urgent", "Medical", "2024-02-08", "Surgery", "Employee12352", "Grace", "Lee", "Approved", 14},
        {23, "Annual", "Family", "2024-03-19", "Planned family event", "Employee12353", "Henry", "Clark", "Under Process", 4},
        {24, "Urgent", "Training", "2024-03-19", "Urgent training need", "Employee12354", "Iris", "Anderson", "Approved", 5},
        {25, "Urgent", "Emergency", "2024-03-19", "Urgent matter", "Employee12355", "Jack", "Martin", "Approved", 3}
    };

    // Add users to the file if they don't exist
    for (int i = 0; i < sizeof(newUsers)/sizeof(newUsers[0]); i++) {
        if (!doesUserExist(userFile, newUsers[i].ID)) {
            fwrite(&newUsers[i], sizeof(struct User), 1, userFile);
        }
    }

    // Add leave requests to the file if they don't exist
    for (int i = 0; i < sizeof(leaveRequests)/sizeof(leaveRequests[0]); i++) {
        if (!doesLeaveExist(leaveFile, leaveRequests[i].ID_Employee, leaveRequests[i].ID_leave)) {
            fwrite(&leaveRequests[i], sizeof(struct LeaveRequest), 1, leaveFile);
        }
    }

    fclose(userFile);
    fclose(leaveFile);
}

void generateLeaveStatistics() {
    FILE *leaveFile = fopen(LEAVE_FILE, "rb");
    if (!leaveFile) {
        printCentered("Error: Unable to open leave file.", RED, BG_DEFAULT);
        return;
    }

    struct LeaveRequest leave;
    int dailyApproved[31] = {0}, dailyRejected[31] = {0}, dailyPending[31] = {0};
    int monthlyApproved[12] = {0}, monthlyRejected[12] = {0}, monthlyPending[12] = {0};
    int yearlyApproved[10] = {0}, yearlyRejected[10] = {0}, yearlyPending[10] = {0};
    int totalApproved = 0, totalRejected = 0, totalPending = 0;

    // Collect statistics
    while (fread(&leave, sizeof(struct LeaveRequest), 1, leaveFile)) {
        int day, month, year;
        sscanf(leave.date, "%d-%d-%d", &year, &month, &day);

        if (day >= 1 && day <= 31 && month >= 1 && month <= 12 && year >= 2018 && year <= 2027) {
            if (strcmp(leave.status, "Approved") == 0) {
                dailyApproved[day - 1]++;
                monthlyApproved[month - 1]++;
                yearlyApproved[year - 2018]++;
                totalApproved++;
            }
            else if (strcmp(leave.status, "Rejected") == 0) {
                dailyRejected[day - 1]++;
                monthlyRejected[month - 1]++;
                yearlyRejected[year - 2018]++;
                totalRejected++;
            }
            else if (strcmp(leave.status, "Under Process") == 0) {
                dailyPending[day - 1]++;
                monthlyPending[month - 1]++;
                yearlyPending[year - 2018]++;
                totalPending++;
            }
        }
    }

    fclose(leaveFile);

    // Display Summary with centered formatting
    printCentered("\nLeave Summary:", CYAN, BG_DEFAULT);
    printCentered("+-------------------+---------+", WHITE, BG_DEFAULT);
    printCentered("| Status            | Count   |", YELLOW, BG_DEFAULT);
    printCentered("+-------------------+---------+", WHITE, BG_DEFAULT);

    char buffer[100];
    snprintf(buffer, sizeof(buffer), "| Approved          | %-7d |", totalApproved);
    printCentered(buffer, GREEN, BG_DEFAULT);
    snprintf(buffer, sizeof(buffer), "| Rejected          | %-7d |", totalRejected);
    printCentered(buffer, RED, BG_DEFAULT);
    snprintf(buffer, sizeof(buffer), "| Pending           | %-7d |", totalPending);
    printCentered(buffer, YELLOW, BG_DEFAULT);
    printCentered("+-------------------+---------+", WHITE, BG_DEFAULT);

    // Create pipes for gnuplot
    FILE *gnupipe = NULL;
    #ifdef _WIN32
        gnupipe = _popen("gnuplot -persist", "w");
    #else
        gnupipe = popen("gnuplot -persist", "w");
    #endif

    if (gnupipe) {
        // Configure gnuplot
        fprintf(gnupipe, "set terminal pngcairo enhanced font 'Arial,10' size 800,600\n");
        fprintf(gnupipe, "set style data histograms\n");
        fprintf(gnupipe, "set style fill solid\n");
        fprintf(gnupipe, "set boxwidth 0.8\n");
        fprintf(gnupipe, "set key outside\n");
        fprintf(gnupipe, "set auto x\n");

        // Daily stats
        fprintf(gnupipe, "set output 'daily_stats.png'\n");
        fprintf(gnupipe, "set title 'Daily Leave Statistics'\n");
        fprintf(gnupipe, "set xlabel 'Day'\n");
        fprintf(gnupipe, "set ylabel 'Number of Leaves'\n");
        fprintf(gnupipe, "plot '-' using 2:xtic(1) title 'Approved' lc rgb '#4CAF50',");
        fprintf(gnupipe, "     '-' using 2 title 'Rejected' lc rgb '#F44336',");
        fprintf(gnupipe, "     '-' using 2 title 'Pending' lc rgb '#FFC107'\n");

        // Write data directly through pipe
        for (int i = 0; i < 31; i++) {
            fprintf(gnupipe, "%d %d\n", i+1, dailyApproved[i]);
        }
        fprintf(gnupipe, "e\n");
        for (int i = 0; i < 31; i++) {
            fprintf(gnupipe, "%d %d\n", i+1, dailyRejected[i]);
        }
        fprintf(gnupipe, "e\n");
        for (int i = 0; i < 31; i++) {
            fprintf(gnupipe, "%d %d\n", i+1, dailyPending[i]);
        }
        fprintf(gnupipe, "e\n");

        // Monthly stats
        fprintf(gnupipe, "set output 'monthly_stats.png'\n");
        fprintf(gnupipe, "set title 'Monthly Leave Statistics'\n");
        fprintf(gnupipe, "set xlabel 'Month'\n");
        fprintf(gnupipe, "plot '-' using 2:xtic(1) title 'Approved' lc rgb '#4CAF50',");
        fprintf(gnupipe, "     '-' using 2 title 'Rejected' lc rgb '#F44336',");
        fprintf(gnupipe, "     '-' using 2 title 'Pending' lc rgb '#FFC107'\n");

        for (int i = 0; i < 12; i++) {
            fprintf(gnupipe, "%d %d\n", i+1, monthlyApproved[i]);
        }
        fprintf(gnupipe, "e\n");
        for (int i = 0; i < 12; i++) {
            fprintf(gnupipe, "%d %d\n", i+1, monthlyRejected[i]);
        }
        fprintf(gnupipe, "e\n");
        for (int i = 0; i < 12; i++) {
            fprintf(gnupipe, "%d %d\n", i+1, monthlyPending[i]);
        }
        fprintf(gnupipe, "e\n");

        // Yearly stats
        fprintf(gnupipe, "set output 'yearly_stats.png'\n");
        fprintf(gnupipe, "set title 'Yearly Leave Statistics'\n");
        fprintf(gnupipe, "set xlabel 'Year'\n");
        fprintf(gnupipe, "plot '-' using 2:xtic(1) title 'Approved' lc rgb '#4CAF50',");
        fprintf(gnupipe, "     '-' using 2 title 'Rejected' lc rgb '#F44336',");
        fprintf(gnupipe, "     '-' using 2 title 'Pending' lc rgb '#FFC107'\n");

        for (int i = 0; i < 10; i++) {
            fprintf(gnupipe, "%d %d\n", 2018+i, yearlyApproved[i]);
        }
        fprintf(gnupipe, "e\n");
        for (int i = 0; i < 10; i++) {
            fprintf(gnupipe, "%d %d\n", 2018+i, yearlyRejected[i]);
        }
        fprintf(gnupipe, "e\n");
        for (int i = 0; i < 10; i++) {
            fprintf(gnupipe, "%d %d\n", 2018+i, yearlyPending[i]);
        }
        fprintf(gnupipe, "e\n");

        #ifdef _WIN32
            _pclose(gnupipe);
        #else
            pclose(gnupipe);
        #endif

        printCentered("Graphs generated successfully!", GREEN, BG_DEFAULT);

        // Open generated images
        #ifdef _WIN32
            system("start daily_stats.png");
            system("start monthly_stats.png");
            system("start yearly_stats.png");
        #elif __APPLE__
            system("open daily_stats.png");
            system("open monthly_stats.png");
            system("open yearly_stats.png");
        #else
            system("xdg-open daily_stats.png");
            system("xdg-open monthly_stats.png");
            system("xdg-open yearly_stats.png");
        #endif
    } else {
        printCentered("Error: Failed to create pipe to gnuplot", RED, BG_DEFAULT);
    }

    printCentered("\nPress Enter to return to the main menu...", CYAN, BG_DEFAULT);
    getchar();
}



void ManagerInterface(char* userID) {
    int choice;

    while (1) {
        // Header
        printCentered("+============================================================+", WHITE, BG_GRAY);
        printCentered("|                     MANAGER INTERFACE                      |", WHITE, BG_GRAY);
        printCentered("+============================================================+", WHITE, BG_GRAY);

        // Menu options with a decorative box
        printf("\n");
        printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);
        printCentered("|                        MAIN MENU                           |", CYAN, BG_DEFAULT);
        printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);
        printCentered("|                    1. View Profile                         |", GREEN, BG_DEFAULT);
        printCentered("|               2. Review Pending Sign-Ups                   |", GREEN, BG_DEFAULT);
        printCentered("|                3. Review Leave Requests                    |", GREEN, BG_DEFAULT);
        printCentered("|           4. Review Employees and Their Leaves             |", GREEN, BG_DEFAULT);
        if (strcmp(userID, "Manager12345") == 0) {
            printCentered("|       5. Review All Users and Possibly Delete a User       |", GREEN, BG_DEFAULT);
        } else {
            printCentered("|  5. Review All Users and Possibly Delete a User (HR Only)  |", RED, BG_DEFAULT);
        }
        printCentered("|                6. View Leave Statistics                    |", GREEN, BG_DEFAULT);
        printCentered("|                         0. Logout                          |", RED, BG_DEFAULT);
        printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);


        // Footer with input box
        printf("\n");
        printCentered("+----------------------------+", WHITE, BG_DEFAULT);
        printCentered("|     Enter Your Choice:     |", CYAN, BG_DEFAULT);
        printCentered("|                            |", WHITE, BG_DEFAULT);
        printCentered("+----------------------------+", WHITE, BG_DEFAULT);

        // Get terminal width using platform-specific code
        #ifdef _WIN32
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            GetConsoleScreenBufferInfo(hConsole, &csbi);
            int termWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        #else
            struct winsize w;
            ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
            int termWidth = w.ws_col;
        #endif

        // Calculate center position
        int boxWidth = 30;  // Width of the box (including borders)
        int leftPadding = (termWidth - boxWidth) / 2;
        int centerPadding = leftPadding + (boxWidth / 2);

        // Position cursor inside the box for centered input
        printf("\033[2A"); // Move up 2 lines
        printf("\033[%dC", centerPadding); // Move cursor to center
        scanf("%d", &choice);
        getchar();  // Handle leftover newline
        printf("\033[2B"); // Move cursor back down

        // Handle the user's choice
        switch (choice) {
            case 1:
                clearScreen();
                printCentered("+============================================================+", WHITE, BG_GRAY);
                printCentered("|                       VIEW PROFILE                         |", WHITE, BG_GRAY);
                printCentered("+============================================================+", WHITE, BG_GRAY);
                viewProfile(userID);  // Call the function to view profile

                // Modify profile choice input with box
                {
                    int modifyChoice;
                    printCentered("Modify it? (Enter 1 for Yes, any other number for No): ", CYAN, BG_DEFAULT);
                    printCentered("+-------------------------------------------------------------+", WHITE, BG_DEFAULT);
                    printCentered("|                      Enter Your Choice:                     |", CYAN, BG_DEFAULT);
                    printCentered("|                                                             |", WHITE, BG_DEFAULT);
                    printCentered("+-------------------------------------------------------------+", WHITE, BG_DEFAULT);

                    // Position cursor inside the box for input
                    printf("\033[2A");
                    printf("\033[%dC", leftPadding + 15); // Adjust position inside the box
                    scanf("%d", &modifyChoice);
                    getchar();  // Handle leftover newline
                    printf("\033[2B"); // Move cursor back down

                    if (modifyChoice == 1) {
                        updateProfile(userID);  // Call the function to update profile
                    }
                }
                pauseForEnter();
                break;

            case 2:
                clearScreen();
                printCentered("+============================================================+", WHITE, BG_GRAY);
                printCentered("|                 REVIEW PENDING SIGN-UPS                    |", WHITE, BG_GRAY);
                printCentered("+============================================================+", WHITE, BG_GRAY);
                reviewPendingSignUps();  // Call the function to review sign-ups
                pauseForEnter();
                break;

            case 3:
                clearScreen();
                printCentered("+============================================================+", WHITE, BG_GRAY);
                printCentered("|                  REVIEW LEAVE REQUESTS                     |", WHITE, BG_GRAY);
                printCentered("+============================================================+", WHITE, BG_GRAY);
                reviewLeaveRequests();  // Call the function to review leave requests
                pauseForEnter();
                break;

            case 4:
                clearScreen();
                printCentered("+============================================================+", WHITE, BG_GRAY);
                printCentered("|          REVIEW EMPLOYEES AND THEIR SPECIFIC LEAVES        |", WHITE, BG_GRAY);
                printCentered("+============================================================+", WHITE, BG_GRAY);
                displayUsersAndLeaves();  // Call the function to display users and leaves
                pauseForEnter();
                break;

            case 5:
                clearScreen();
                // Display all users (same as reviewUsers() logic)
                FILE *userFile = fopen(USER_FILE, "rb");
                struct User tempUser;
                if (userFile == NULL) {
                    printCentered("Error opening user file.", RED, BG_DEFAULT);
                    pauseForEnter();
                    break;
                }

                printCentered("+------------------------------------------------------------+", GREEN, BG_DEFAULT);
                printCentered("| ID            | First Name | Last Name                     |", GREEN, BG_DEFAULT);
                printCentered("+------------------------------------------------------------+", GREEN, BG_DEFAULT);

                while (fread(&tempUser, sizeof(struct User), 1, userFile) == 1) {
                    char displayInfo[150];
                    snprintf(displayInfo, sizeof(displayInfo), "%-14s | %-11s | %-9s", tempUser.ID, tempUser.first_name, tempUser.last_name);
                    printCentered(displayInfo, WHITE, BG_DEFAULT);
                }

                fclose(userFile);

                // Only allow Manager12345 to delete users
                if (strcmp(userID, "Manager12345") == 0) {
                    deleteUser();  // Only Manager12345 can delete users
                    pauseForEnter();
                } else {
                    printCentered("Only HR Manager with ID Manager12345 can delete users.", RED, BG_DEFAULT);
                    pauseForEnter();
                }
                break;
            case 6:  // Option for viewing statistics
                clearScreen();
                printCentered("+============================================================+", WHITE, BG_GRAY);
                printCentered("|                   LEAVE STATISTICS                         |", WHITE, BG_GRAY);
                printCentered("+============================================================+", WHITE, BG_GRAY);
                generateLeaveStatistics();  // Call the statistics function
                pauseForEnter();
                break;
            case 0:
                clearScreen();
                printCentered("+============================================================+", WHITE, BG_GRAY);
                printCentered("|                     LOGGING OUT...                         |", RED, BG_GRAY);
                printCentered("+============================================================+", WHITE, BG_GRAY);
                pauseForEnter();
                return;  // Exit the loop to log out

            default:
                printCentered("Invalid choice. Try again.", RED, BG_DEFAULT);
                pauseForEnter();
        }

        clearScreen();  // Clear screen after handling each choice
    }
}


// Fonction pour vérifier que le nom ne contient que des lettres
int isValidName(const char *name) {
    for (size_t i = 0; i < strlen(name); i++) {
        if (!isalpha(name[i]) && name[i] != ' ') {
            return 0; // Caractère invalide
        }
    }
    return 1; // Nom valide
}

// Fonction pour vérifier une date au format DD/MM/YYYY
int isValidDate(const char *date) {
    int day, month, year;
    if (sscanf(date, "%d/%d/%d", &day, &month, &year) != 3) {
        return 0; // Format incorrect
    }

    // Vérifier les limites logiques des jours et des mois
    if (month < 1 || month > 12 || day < 1 || year < 1900 || year > 2023) {
        return 0;
    }

    // Vérification des jours max par mois
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Ajuster pour les années bissextiles
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        daysInMonth[1] = 29; // Février a 29 jours
    }

    return day <= daysInMonth[month - 1];
}
int isValidPhoneNumber(const char *phone) {
    for (size_t i = 0; i < strlen(phone); i++) {
        if (!isdigit(phone[i])) {
            return 0; // Contient un caractère non numérique
        }
    }
    return 1; // Numéro valide
}
int isValidEmail(const char *email) {
    const char *at = strchr(email, '@');
    const char *dot = strrchr(email, '.');

    // Vérifier qu'il y a un '@' avant un '.'
    if (at != NULL && dot != NULL && at < dot) {
        return 1; // Email valide
    }
    return 0; // Email invalide
}
int isValidDepartment(const char *department) {
    for (size_t i = 0; i < strlen(department); i++) {
        if (!isalpha(department[i]) && department[i] != ' ') {
            return 0; // Contient un caractère non valide
        }
    }
    return 1; // Département valide
}


void signUp() {
    FILE *pendingFile = fopen(PENDING_USER_FILE, "ab"); // Open file for binary append
    if (pendingFile == NULL) {
        printCentered("Error opening pending sign-ups file.", RED, BG_DEFAULT);
        return;
    }

    struct User newUser;
    char role[10];
    int uniqueNumber;

    clearScreen();
    printCentered("============================================================", WHITE, BG_GRAY);
    printCentered("                        SIGN UP                             ", WHITE, BG_GRAY);
    printCentered("============================================================", WHITE, BG_GRAY);

    // Get terminal width
    #ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        int termWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    #else
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        int termWidth = w.ws_col;
    #endif

    int boxWidth = 60;
    int leftPadding = (termWidth - boxWidth) / 2;
    int centerPadding = leftPadding + (boxWidth / 2) - 10;

    // Get the role
    do {
        printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);
        printCentered("|                    Enter your role                         |", CYAN, BG_DEFAULT);
        printCentered("|                  (Employee or Manager)                     |", CYAN, BG_DEFAULT);
        printCentered("|                                                            |", WHITE, BG_DEFAULT);
        printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);

        printf("\033[2A");
        printf("\033[%dC", centerPadding);
        scanf("%9s", role);
        getchar();

        if (strcmp(role, "Employee") != 0 && strcmp(role, "Manager") != 0) {
            printCentered("Invalid role. Please enter 'Employee' or 'Manager'.", RED, BG_DEFAULT);
            printf("\n");
        }
    } while (strcmp(role, "Employee") != 0 && strcmp(role, "Manager") != 0);
    printf("\n");
    // Generate unique ID
    uniqueNumber = generateUniqueNumber();
    snprintf(newUser.ID, sizeof(newUser.ID), "%s%d", role, uniqueNumber);
    printCentered("Your unique ID is: ", GREEN, BG_DEFAULT);
    printCentered(newUser.ID, GREEN, BG_DEFAULT);
    printf("\n");
    // Get user details with validations
    do {
        printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);
        printCentered("|                  Enter your first name                     |", CYAN, BG_DEFAULT);
        printCentered("|                                                            |", WHITE, BG_DEFAULT);
        printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);

        printf("\033[2A");
        printf("\033[%dC", centerPadding);
        fgets(newUser.first_name, sizeof(newUser.first_name), stdin);
        newUser.first_name[strcspn(newUser.first_name, "\n")] = '\0';

        if (!isValidName(newUser.first_name)) {
            printCentered("Invalid name. Please use only letters.", RED, BG_DEFAULT);
            printf("\n");
        }
    } while (!isValidName(newUser.first_name));
    printf("\n");
    // Similar pattern for each field...
    // Last name
    do {
        printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);
        printCentered("|                  Enter your last name                      |", CYAN, BG_DEFAULT);
        printCentered("|                                                            |", WHITE, BG_DEFAULT);
        printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);

        printf("\033[2A");
        printf("\033[%dC", centerPadding);
        fgets(newUser.last_name, sizeof(newUser.last_name), stdin);
        newUser.last_name[strcspn(newUser.last_name, "\n")] = '\0';

        if (!isValidName(newUser.last_name)) {
            printCentered("Invalid name. Please use only letters.", RED, BG_DEFAULT);
            printf("\n");
        }
    } while (!isValidName(newUser.last_name));
    printf("\n");
    // Password
    printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);
    printCentered("|                  Enter your password                       |", CYAN, BG_DEFAULT);
    printCentered("|                                                            |", WHITE, BG_DEFAULT);
    printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);

    printf("\033[2A");
    printf("\033[%dC", centerPadding);
    scanf("%49s", newUser.password);
    getchar();
    printf("\n");

    // Gender
    do {
        printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);
        printCentered("|                  Enter your gender (M/F)                   |", CYAN, BG_DEFAULT);
        printCentered("|                                                            |", WHITE, BG_DEFAULT);
        printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);

        printf("\033[2A");
        printf("\033[%dC", centerPadding);
        scanf("%1s", newUser.gender);
        getchar();

        if (newUser.gender[0] != 'M' && newUser.gender[0] != 'F') {
            printCentered("Invalid gender. Please enter 'M' or 'F'.", RED, BG_DEFAULT);
            printf("\n");
        }
    } while (newUser.gender[0] != 'M' && newUser.gender[0] != 'F');
    printf("\n");
    // Continue similar pattern for remaining fields...
    // Birthday
    do {
        printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);
        printCentered("|            Enter your birthday (DD/MM/YYYY)                |", CYAN, BG_DEFAULT);
        printCentered("|                                                            |", WHITE, BG_DEFAULT);
        printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);

        printf("\033[2A");
        printf("\033[%dC", centerPadding);
        scanf("%10s", newUser.birth_day);
        getchar();

        if (!isValidDate(newUser.birth_day)) {
            printCentered("Invalid date. Please enter a logical date in DD/MM/YYYY format.", RED, BG_DEFAULT);
            printf("\n");
        }
    } while (!isValidDate(newUser.birth_day));
    printf("\n");
    // Address
    printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);
    printCentered("|                  Enter your address                        |", CYAN, BG_DEFAULT);
    printCentered("|                                                            |", WHITE, BG_DEFAULT);
    printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);

    printf("\033[2A");
    printf("\033[%dC", centerPadding);
    fgets(newUser.adress, sizeof(newUser.adress), stdin);
    newUser.adress[strcspn(newUser.adress, "\n")] = '\0';
    printf("\n");

    // Phone number
    do {
        printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);
        printCentered("|            Enter your phone number (only digits)           |", CYAN, BG_DEFAULT);
        printCentered("|                                                            |", WHITE, BG_DEFAULT);
        printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);

        printf("\033[2A");
        printf("\033[%dC", centerPadding);
        scanf("%19s", newUser.tele_num);
        getchar();

        if (!isValidPhoneNumber(newUser.tele_num)) {
            printCentered("Invalid phone number. Please enter only digits.", RED, BG_DEFAULT);
            printf("\n");
        }
    } while (!isValidPhoneNumber(newUser.tele_num));
    printf("\n");
    // Email
    do {
        printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);
        printCentered("|                  Enter your email                          |", CYAN, BG_DEFAULT);
        printCentered("|                                                            |", WHITE, BG_DEFAULT);
        printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);

        printf("\033[2A");
        printf("\033[%dC", centerPadding);
        scanf("%99s", newUser.email);
        getchar();

        if (!isValidEmail(newUser.email)) {
            printCentered("Invalid email format. Please enter a valid email.", RED, BG_DEFAULT);
            printf("\n");
        }
    } while (!isValidEmail(newUser.email));
    printf("\n");
    // Recruitment day
    do {
        printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);
        printCentered("|        Enter your recruitment day (DD/MM/YYYY)             |", CYAN, BG_DEFAULT);
        printCentered("|                                                            |", WHITE, BG_DEFAULT);
        printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);

        printf("\033[2A");
        printf("\033[%dC", centerPadding);
        scanf("%10s", newUser.recruitment_day);
        getchar();

        if (!isValidDate(newUser.recruitment_day)) {
            printCentered("Invalid date. Please enter a logical date in DD/MM/YYYY format.", RED, BG_DEFAULT);
            printf("\n");
        }
    } while (!isValidDate(newUser.recruitment_day));
    printf("\n");
    // Department
    do {
        printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);
        printCentered("|                  Enter your department                     |", CYAN, BG_DEFAULT);
        printCentered("|                                                            |", WHITE, BG_DEFAULT);
        printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);

        printf("\033[2A");
        printf("\033[%dC", centerPadding);
        fgets(newUser.departement, sizeof(newUser.departement), stdin);
        newUser.departement[strcspn(newUser.departement, "\n")] = '\0';

        if (!isValidDepartment(newUser.departement)) {
            printCentered("Invalid department. Please enter only letters.", RED, BG_DEFAULT);
            printf("\n");
        }
    } while (!isValidDepartment(newUser.departement));
    printf("\n");
    // Security questions
    printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);
    printCentered("|        Enter your first security question                  |", CYAN, BG_DEFAULT);
    printCentered("|        (e.g., Your childhood best friend?)                |", CYAN, BG_DEFAULT);
    printCentered("|                                                            |", WHITE, BG_DEFAULT);
    printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);

    printf("\033[2A");
    printf("\033[%dC", centerPadding);
    fgets(newUser.question1, sizeof(newUser.question1), stdin);
    newUser.question1[strcspn(newUser.question1, "\n")] = '\0';
    printf("\n");
    printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);
    printCentered("|        Enter the answer to your first question             |", CYAN, BG_DEFAULT);
    printCentered("|                                                            |", WHITE, BG_DEFAULT);
    printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);

    printf("\033[2A");
    printf("\033[%dC", centerPadding);
    fgets(newUser.answer1, sizeof(newUser.answer1), stdin);
    newUser.answer1[strcspn(newUser.answer1, "\n")] = '\0';
    printf("\n");
    printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);
    printCentered("|        Enter your second security question                 |", CYAN, BG_DEFAULT);
    printCentered("|        (e.g., Your father's profession?)                  |", CYAN, BG_DEFAULT);
    printCentered("|                                                            |", WHITE, BG_DEFAULT);
    printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);

    printf("\033[2A");
    printf("\033[%dC", centerPadding);
    fgets(newUser.question2, sizeof(newUser.question2), stdin);
    newUser.question2[strcspn(newUser.question2, "\n")] = '\0';
    printf("\n");
    printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);
    printCentered("|        Enter the answer to your second question            |", CYAN, BG_DEFAULT);
    printCentered("|                                                            |", WHITE, BG_DEFAULT);
    printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);

    printf("\033[2A");
    printf("\033[%dC", centerPadding);
    fgets(newUser.answer2, sizeof(newUser.answer2), stdin);
    newUser.answer2[strcspn(newUser.answer2, "\n")] = '\0';
    printf("\n");
    newUser.Annual_leave_bonus = Annual_LEAVE_LIMIT;
    newUser.signupstate = 2;

    // Write new user to the pending file
    writeUserToFile(pendingFile, &newUser);

    clearScreen();
    printCentered("============================================================", WHITE, BG_GRAY);
    printCentered("Sign-up request submitted successfully!", GREEN, BG_GRAY);
    printCentered("Your ID is:", CYAN, BG_GRAY);
    printCentered(newUser.ID, BLUE, BG_GRAY);
    printCentered("============================================================", WHITE, BG_GRAY);

    fclose(pendingFile);
    pauseForEnter();
}



int logIn() {
    struct User tempUser;
    char enteredID[50], enteredPassword[50];
    int passwordIndex = 0;
    char ch;

    clearScreen();
    printCentered("+============================================================+", WHITE, BG_GRAY);
    printCentered("|                          LOG IN                            |", WHITE, BG_GRAY);
    printCentered("+============================================================+", WHITE, BG_GRAY);

    // Decorative input box for User ID
    printf("\n");
    printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);
    printCentered("|                       Enter Your ID                        |", CYAN, BG_DEFAULT);
    printCentered("|                                                            |", WHITE, BG_DEFAULT);
    printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);

    // Get terminal width using platform-specific code
    #ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        int termWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    #else
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        int termWidth = w.ws_col;
    #endif

    int boxWidth = 60;
    int leftPadding = (termWidth - boxWidth) / 2;
    int centerPadding = leftPadding + (boxWidth / 2) - 10; // Adjust for input field width

    // Move cursor to centered input position
    printf("\033[2A"); // Move up 2 lines
    printf("\033[%dC", centerPadding);
    scanf("%49s", enteredID);
    getchar(); // Clear trailing newline

    // Decorative input box for Password
    printf("\n\n");
    printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);
    printCentered("|                    Enter Your Password                     |", CYAN, BG_DEFAULT);
    printCentered("|                                                            |", WHITE, BG_DEFAULT);
    printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);

    // Position cursor in center of password box
    printf("\033[2A"); // Move up 2 lines
    printf("\033[%dC", centerPadding);

    // Input password with masking
    while (1) {
        ch = getch(); // Use getch for masked input
        if (ch == '\n' || ch == '\r') { // Enter key
            enteredPassword[passwordIndex] = '\0';
            break;
        } else if (ch == 127 || ch == '\b') { // Backspace key
            if (passwordIndex > 0) {
                passwordIndex--;
                printf("\b \b"); // Erase the last character on the screen
            }
        } else {
            if (passwordIndex < sizeof(enteredPassword) - 1) {
                enteredPassword[passwordIndex++] = ch;
                printf("*"); // Print asterisks for password masking
            }
        }
    }
    printf("\n");

    pauseForEnter();
    clearScreen();

    // Check credentials in the USER_FILE
    if (checkUserCredentials(enteredID, enteredPassword)) {
        return 1; // Successful login
    }

    // Check if the user exists in the pending sign-ups
    if (checkPendingUser(enteredID)) {
        printCentered("Your account is awaiting approval. Please try again later.", YELLOW, BG_DEFAULT);
        pauseForEnter();
        return 0;
    }

    // Handle invalid login
    handleInvalidLogin();
    return 0;
}


// Function to check user credentials in USER_FILE
int checkUserCredentials(const char *enteredID, const char *enteredPassword) {
    FILE *file = fopen(USER_FILE, "rb");
    if (!file) {
        printCentered("Error opening user file.", RED, BG_DEFAULT);
        return 0;
    }

    struct User tempUser;
    while (fread(&tempUser, sizeof(struct User), 1, file) == 1) {
        if (strcmp(tempUser.ID, enteredID) == 0 && strcmp(tempUser.password, enteredPassword) == 0) {
            fclose(file);

            // Successful login
            clearScreen();
            printCentered("============================================================", WHITE, BG_GRAY);
            char welcomeMessage[100];
            snprintf(welcomeMessage, sizeof(welcomeMessage), "Welcome, %s %s! Department: %s",
                     tempUser.first_name, tempUser.last_name, tempUser.departement);
            printCentered(welcomeMessage, GREEN, BG_DEFAULT);
            printCentered("============================================================", WHITE, BG_GRAY);

            // Role-based interface
            if (strncmp(tempUser.ID, "Manager", 7) == 0) {
                printCentered("Manager", GREEN, BG_DEFAULT);
                ManagerInterface(tempUser.ID); // Proceed to Manager interface
            } else if (strncmp(tempUser.ID, "Employee", 8) == 0) {
                printCentered("Employee", GREEN, BG_DEFAULT);
                EmployeeInterface(tempUser.ID); // Proceed to Employee interface
            } else {
                printCentered("Unknown Role", RED, BG_DEFAULT);
            }
            return 1; // Successful login
        }
    }

    fclose(file);
    return 0; // No match found
}

// Function to check if user exists in pending accounts
int checkPendingUser(const char *enteredID) {
    FILE *file = fopen(PENDING_USER_FILE, "rb");
    if (!file) {
        printCentered("Error opening pending user file.", RED, BG_DEFAULT);
        return 0;
    }

    struct User tempUser;
    while (fread(&tempUser, sizeof(struct User), 1, file) == 1) {
        if (strcmp(tempUser.ID, enteredID) == 0 && tempUser.signupstate == 2) {
            fclose(file);
            return 1; // Pending account found
        }
    }

    fclose(file);
    return 0; // No pending account found
}

// Function to handle invalid login attempts
void handleInvalidLogin() {
    int choix;

    printCentered("Invalid ID or password. Please try again.", RED, BG_DEFAULT);
    printCentered("If you want to recover your password, type 1:", CYAN, BG_DEFAULT);
    scanf("%d", &choix);
    if (choix == 1) {
        clearScreen();
        printCentered("============================================================", WHITE, BG_GRAY);
        printCentered("                   Forgot Password Page                     ", WHITE, BG_GRAY);
        printCentered("============================================================", WHITE, BG_GRAY);
        recoverPassword(); // Call password recovery
        pauseForEnter();
        clearScreen();
    }
}

void writeLeaveToFile(FILE *file, struct LeaveRequest leave) {
    fwrite(&leave, sizeof(struct LeaveRequest), 1, file);
}

int readLeaveFromFile(FILE *file, struct LeaveRequest *leave) {
    return fread(leave, sizeof(struct LeaveRequest), 1, file);
}

void displayUsersAndLeaves() {
    FILE *usersFile = fopen(USER_FILE, "rb");
    FILE *leavesFile = fopen(LEAVE_FILE, "rb");

    if (!usersFile) {
        printCentered("Error: Unable to open users file.", RED, BG_DEFAULT);
        return;
    }
    if (!leavesFile) {
        printCentered("Error: Unable to open leave requests file.", RED, BG_DEFAULT);
        fclose(usersFile);
        return;
    }

    struct User user;
    struct LeaveRequest leave;
    int userCount = 0;

    clearScreen();
    printCentered("============================================================", WHITE, BG_GRAY);
    printCentered("                       ALL EMPLOYEES                        ", WHITE, BG_GRAY);
    printCentered("============================================================", WHITE, BG_GRAY);

    // Display all users in an organized table format
    printCentered("+--------------------------------------------------------------------+", GREEN, BG_DEFAULT);
    printCentered("| ID            | Name                    | Department               |", GREEN, BG_DEFAULT);
    printCentered("+--------------------------------------------------------------------+", GREEN, BG_DEFAULT);

    rewind(usersFile);
    while (readUserFromFile(usersFile, &user)) {
        if (strncmp(user.ID, "Manager", 7) != 0) { // Skip managers
            userCount++;
            char userInfo[200];
            char fullName[50];
            snprintf(fullName, sizeof(fullName), "%s %s", user.first_name, user.last_name);
            snprintf(userInfo, sizeof(userInfo), "| %-13s | %-23s | %-22s |",
                    user.ID, fullName, user.departement);
            printCentered(userInfo, WHITE, BG_DEFAULT);
        }
    }

    printCentered("+--------------------------------------------------------------------+", GREEN, BG_DEFAULT);

    if (userCount == 0) {
        printCentered("No employees found in the system.", RED, BG_DEFAULT);
        fclose(usersFile);
        fclose(leavesFile);
        return;
    }

    char selectedEmployeeID[50];
    printCentered("\nEnter the ID of the employee to view their leaves (or 'exit' to go back):", CYAN, BG_DEFAULT);
    scanf("%49s", selectedEmployeeID);
    getchar();

    if (strcmp(selectedEmployeeID, "exit") == 0) {
        printCentered("Returning to manager menu...", CYAN, BG_DEFAULT);
        fclose(usersFile);
        fclose(leavesFile);
        return;
    }

    rewind(usersFile);
    int employeeFound = 0;
    while (readUserFromFile(usersFile, &user)) {
        if (strcmp(user.ID, selectedEmployeeID) == 0) {
            employeeFound = 1;
            break;
        }
    }

    if (!employeeFound) {
        printCentered("Error: No employee found with that ID.", RED, BG_DEFAULT);
        fclose(usersFile);
        fclose(leavesFile);
        return;
    }

    clearScreen();
    int leaveCount = 0;
    rewind(leavesFile);

    printCentered("============================================================", WHITE, BG_GRAY);
    printCentered("                    LEAVE RECORDS                           ", WHITE, BG_GRAY);
    printCentered("============================================================", WHITE, BG_GRAY);

    char employeeHeader[100];
    snprintf(employeeHeader, sizeof(employeeHeader), "Employee: %s %s (%s)",
             user.first_name, user.last_name, selectedEmployeeID);
    printCentered(employeeHeader, YELLOW, BG_DEFAULT);
    printCentered("+==============================================================================+", GREEN, BG_DEFAULT);

    while (readLeaveFromFile(leavesFile, &leave)) {
        if (strcmp(leave.ID_Employee, selectedEmployeeID) == 0) {
            leaveCount++;

            // Print header line for each leave record
            printCentered("+==============================================================================+", GREEN, BG_DEFAULT);

            // Format and print leave ID, type and status
            char leaveInfo[200];
            snprintf(leaveInfo, sizeof(leaveInfo),
                    "| Leave ID: %-3d | Type: %-12s | Status: %-12s |",
                    leave.ID_leave, leave.type, leave.status);
            printCentered(leaveInfo, WHITE, BG_DEFAULT);

            // Print separator line
            printCentered("+------------------------------------------------------------------------------+", GREEN, BG_DEFAULT);

            // Format and print title
            char titleInfo[200];
            snprintf(titleInfo, sizeof(titleInfo),
                    "| Title: %-61s |", leave.title);
            printCentered(titleInfo, WHITE, BG_DEFAULT);

            // Format and print date and duration
            char dateInfo[200];
            snprintf(dateInfo, sizeof(dateInfo),
                    "| Date: %-12s | Duration: %-3d days                              |",
                    leave.date, leave.days);
            printCentered(dateInfo, WHITE, BG_DEFAULT);

            // Format and print reason/content
            char reasonInfo[200];
            snprintf(reasonInfo, sizeof(reasonInfo),
                    "| Reason: %-60s |", leave.content);
            printCentered(reasonInfo, WHITE, BG_DEFAULT);

            // Print footer line for each leave record
            printCentered("+==============================================================================+", GREEN, BG_DEFAULT);

            // Add blank line between records
            printCentered("", WHITE, BG_DEFAULT);
        }
    }

    if (leaveCount == 0) {
        printCentered("No leave records found for this employee.", RED, BG_DEFAULT);
    }

    printCentered("\nOptions:", CYAN, BG_DEFAULT);
    printCentered("1. View another employee's leaves", GREEN, BG_DEFAULT);
    printCentered("0. Return to manager menu", GREEN, BG_DEFAULT);

    int choice;
    printCentered("\nEnter your choice (0-1):", CYAN, BG_DEFAULT);
    scanf("%d", &choice);
    getchar();

    if (choice == 1) {
        displayUsersAndLeaves();
    }

    fclose(usersFile);
    fclose(leavesFile);
    printCentered("Returning to manager menu...", CYAN, BG_DEFAULT);
}




void Update_pending_leaves(char *userID) {
    FILE *file = fopen(LEAVE_FILE, "rb");
    FILE *tempFile = fopen("temp.dat", "wb");

    if (file == NULL || tempFile == NULL) {
        printCentered("Error opening files.", RED, BG_DEFAULT);
        if (file) fclose(file);
        if (tempFile) fclose(tempFile);
        return;
    }

    struct LeaveRequest leave;
    int found = 0;
    int displayedAny = 0;
    int leaveIDToDelete;

    printCentered("--- Your 'Under Process' Leave Requests ---", YELLOW, BG_GRAY);

    // Display all 'under process' leave requests for the current Employee
    while (fread(&leave, sizeof(struct LeaveRequest), 1, file)) {
        if (strcmp(leave.ID_Employee, userID) == 0 && strcmp(leave.status, "under process") == 0) {
            char leaveInfo[100];
            snprintf(leaveInfo, sizeof(leaveInfo), "Leave ID: %d | Title: %s", leave.ID_leave, leave.title);
            printCentered(leaveInfo, WHITE, BG_DEFAULT);
            displayedAny = 1;
        }
    }

    if (!displayedAny) {
        printCentered("You have no leave requests under process.", RED, BG_DEFAULT);
        pauseForEnter();
        fclose(file);
        fclose(tempFile);
        remove("temp.dat");
        return;
    }

    // Prompt the user to select a leave request to delete
    printCentered("Enter the Leave ID you want to delete(0 To cancel):", CYAN, BG_DEFAULT);
    scanf("%d", &leaveIDToDelete); // Read integer input
    getchar(); // Clear leftover newline

    // Reset file pointer for re-reading
    rewind(file);

    // Process the file and write to temp
    while (fread(&leave, sizeof(struct LeaveRequest), 1, file)) {
        if (strcmp(leave.ID_Employee, userID) == 0 && leave.ID_leave == leaveIDToDelete && strcmp(leave.status, "under process") == 0) {
            found = 1;
            continue;  // Skip the matching leave ID
        }

        // Write the leave request to the temporary file
        fwrite(&leave, sizeof(struct LeaveRequest), 1, tempFile);
    }

    fclose(file);
    fclose(tempFile);

    // Replace the original file with the updated temp file
    if (found) {
        remove(LEAVE_FILE);
        rename("temp.dat", LEAVE_FILE);
        char successMessage[100];
        snprintf(successMessage, sizeof(successMessage), "Leave request with ID %d has been successfully deleted.", leaveIDToDelete);
        printCentered(successMessage, GREEN, BG_DEFAULT);
    } else {
        char errorMessage[100];
        snprintf(errorMessage, sizeof(errorMessage), "Leave request with ID %d not found or not under process.", leaveIDToDelete);
        printCentered(errorMessage, RED, BG_DEFAULT);
        remove("temp.dat"); // Cleanup temp file if no changes
    }
}



void viewProfile(char *userID) {
    FILE *file = fopen(USER_FILE, "rb");
    if (file == NULL) {
        printCentered("Error opening user file.", RED, BG_DEFAULT);
        return;
    }

    struct User user;
    int found = 0;

    // Get terminal width
    #ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        int termWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    #else
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        int termWidth = w.ws_col;
    #endif

    int boxWidth = 60;
    int leftPadding = (termWidth - boxWidth) / 2;
    int centerPadding = leftPadding + (boxWidth / 2) - 10;

    clearScreen();
    printCentered("============================================================", WHITE, BG_GRAY);
    printCentered("                      VIEW PROFILE                          ", WHITE, BG_GRAY);
    printCentered("============================================================", WHITE, BG_GRAY);

    // Search for the user in the file
    while (readUserFromFile(file, &user)) {
        if (strcmp(user.ID, userID) == 0) {
            found = 1;

            // Display user profile with formatting and colors
            printf("\n");
            printCentered("User Profile", CYAN, BG_DEFAULT);
            printf("------------------------------------------------------------\n");
            printf(" %-20s: %s\n", "User ID", user.ID);
            printf(" %-20s: %s\n", "First Name", user.first_name);
            printf(" %-20s: %s\n", "Last Name", user.last_name);
            printf(" %-20s: %s\n", "Gender", user.gender);
            printf(" %-20s: %s\n", "Birthday", user.birth_day);
            printf(" %-20s: %s\n", "Address", user.adress);
            printf(" %-20s: %s\n", "Department", user.departement);
            printf(" %-20s: %s\n", "Recruitment Day", user.recruitment_day);
            printf(" %-20s: %s\n", "Phone Number", user.tele_num);
            printf(" %-20s: %s\n", "Email", user.email);
            printf(" %-20s: %d\n", "Annual Leave Bonus", user.Annual_leave_bonus);
            printf(" %-20s: %s\n", "Security Question 1", user.question1);
            printf(" %-20s: %s\n", "Security Question 2", user.question2);
            printf("------------------------------------------------------------\n");
            break;
        }
    }

    if (!found) {
        printCentered("User not found.", RED, BG_DEFAULT);
    }

    fclose(file);
    pauseForEnter();
}

void updateProfile(char *userID) {
    FILE *file = fopen(USER_FILE, "rb");
    FILE *tempFile = fopen("temp.dat", "wb");
    if (file == NULL || tempFile == NULL) {
        printCentered("Error opening files.", RED, BG_DEFAULT);
        if (file) fclose(file);
        if (tempFile) fclose(tempFile);
        return;
    }

    struct User user;
    int found = 0;
    char buffer[1000];

    // Get terminal width
    #ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        int termWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    #else
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        int termWidth = w.ws_col;
    #endif

    int boxWidth = 80;
    int leftPadding = (termWidth - boxWidth) / 2;
    int centerPadding = leftPadding + (boxWidth / 2) - 0;

    while (readUserFromFile(file, &user)) {
        if (strcmp(user.ID, userID) == 0) {
            found = 1;
            printCentered("User found. You can now update your profile.", CYAN, BG_DEFAULT);

            int choice;
            while (1) {
                clearScreen();
                // Header
                printCentered("+==========================================================+", WHITE, BG_GRAY);
                printCentered("|                   UPDATE PROFILE MENU                    |", WHITE, BG_GRAY);
                printCentered("+==========================================================+", WHITE, BG_GRAY);

                // Menu Options
                printCentered("+----------------------------------------------------------+", WHITE, BG_DEFAULT);
                printCentered("|                  1. Change Password                      |", CYAN, BG_DEFAULT);
                printCentered("|                  2. Change Department                    |", CYAN, BG_DEFAULT);
                printCentered("|                  3. Change Address                       |", CYAN, BG_DEFAULT);
                printCentered("|                  4. Change Telephone Number              |", CYAN, BG_DEFAULT);
                printCentered("|                  5. Change Email                         |", CYAN, BG_DEFAULT);
                printCentered("|                  6. Change Security Questions            |", CYAN, BG_DEFAULT);
                printCentered("|                  0. Save and Exit                        |", RED, BG_DEFAULT);
                printCentered("+----------------------------------------------------------+", WHITE, BG_DEFAULT);

                // Input Box
                printf("\n");
                printCentered("+----------------------------------------------------------+", WHITE, BG_DEFAULT);
                printCentered("|                  Enter your choice:                      |", YELLOW, BG_DEFAULT);
                printCentered("|                                                          |", WHITE, BG_DEFAULT);
                printCentered("+----------------------------------------------------------+", WHITE, BG_DEFAULT);

                printf("\033[2A");
                printf("\033[%dC", centerPadding);
                if (scanf("%d", &choice) != 1) {
                    printCentered("Invalid input. Try again.", RED, BG_DEFAULT);
                    getchar();
                    continue;
                }
                getchar();

                switch (choice) {
                    case 1: {
                        char password[50];
                        int valid = 0;
                        while (!valid) {
                            clearScreen();
                            printCentered("+==========================================================+", WHITE, BG_GRAY);
                            printCentered("|                   CHANGE PASSWORD                        |", WHITE, BG_GRAY);
                            printCentered("+==========================================================+", WHITE, BG_GRAY);

                            printCentered("+----------------------------------------------------------+", WHITE, BG_DEFAULT);
                            printCentered("| Enter new password (min 8 chars, upper+lower+num+special)|", CYAN, BG_DEFAULT);
                            printCentered("|                                                          |", WHITE, BG_DEFAULT);
                            printCentered("+----------------------------------------------------------+", WHITE, BG_DEFAULT);

                            printf("\033[2A");
                            printf("\033[%dC", centerPadding);
                            fgets(password, sizeof(password), stdin);
                            password[strcspn(password, "\n")] = 0;

                            if (strlen(password) < 8) {
                                printf("\n");
                                printCentered("Password must be at least 8 characters long.", RED, BG_DEFAULT);
                                pauseForEnter();
                                continue;
                            }

                            int hasUpper = 0, hasLower = 0, hasDigit = 0, hasSpecial = 0;
                            for (int i = 0; password[i]; i++) {
                                if (isupper(password[i])) hasUpper = 1;
                                if (islower(password[i])) hasLower = 1;
                                if (isdigit(password[i])) hasDigit = 1;
                                if (ispunct(password[i])) hasSpecial = 1;
                            }

                            if (!hasUpper || !hasLower || !hasDigit || !hasSpecial) {
                                printf("\n");
                                printCentered("Password must contain uppercase, lowercase, number and special character.", RED, BG_DEFAULT);
                                pauseForEnter();
                                continue;
                            }

                            valid = 1;
                        }
                        strncpy(user.password, password, sizeof(user.password) - 1);
                        user.password[sizeof(user.password) - 1] = '\0';
                        printf("\n");
                        printCentered("Password updated successfully!", GREEN, BG_DEFAULT);
                        pauseForEnter();
                        break;
                    }

                    case 2: {
                        char dept[50];
                        int valid = 0;
                        while (!valid) {
                            clearScreen();
                            printCentered("+==========================================================+", WHITE, BG_GRAY);
                            printCentered("|                   CHANGE DEPARTMENT                      |", WHITE, BG_GRAY);
                            printCentered("+==========================================================+", WHITE, BG_GRAY);

                            printCentered("+----------------------------------------------------------+", WHITE, BG_DEFAULT);
                            printCentered("|                  Enter new department:                   |", CYAN, BG_DEFAULT);
                            printCentered("|                                                          |", WHITE, BG_DEFAULT);
                            printCentered("+----------------------------------------------------------+", WHITE, BG_DEFAULT);

                            printf("\033[2A");
                            printf("\033[%dC", centerPadding);
                            fgets(dept, sizeof(dept), stdin);
                            dept[strcspn(dept, "\n")] = 0;

                            valid = 1;
                            for(int i = 0; dept[i]; i++) {
                                if (!isalpha(dept[i]) && dept[i] != ' ') {
                                    valid = 0;
                                    break;
                                }
                            }

                            if (!valid) {
                                printf("\n");
                                printCentered("Invalid department. Please use only letters and spaces.", RED, BG_DEFAULT);
                                pauseForEnter();
                            }
                        }
                        strncpy(user.departement, dept, sizeof(user.departement) - 1);
                        user.departement[sizeof(user.departement) - 1] = '\0';
                        printf("\n");
                        printCentered("Department updated successfully!", GREEN, BG_DEFAULT);
                        pauseForEnter();
                        break;
                    }

                    case 3: {
                        char address[100];
                        int valid = 0;
                        while (!valid) {
                            clearScreen();
                            printCentered("+==========================================================+", WHITE, BG_GRAY);
                            printCentered("|                   CHANGE ADDRESS                         |", WHITE, BG_GRAY);
                            printCentered("+==========================================================+", WHITE, BG_GRAY);

                            printCentered("+----------------------------------------------------------+", WHITE, BG_DEFAULT);
                            printCentered("|        Enter new address (minimum 10 characters):        |", CYAN, BG_DEFAULT);
                            printCentered("|                                                          |", WHITE, BG_DEFAULT);
                            printCentered("+----------------------------------------------------------+", WHITE, BG_DEFAULT);

                            printf("\033[2A");
                            printf("\033[%dC", centerPadding);
                            fgets(address, sizeof(address), stdin);
                            address[strcspn(address, "\n")] = 0;

                            if (strlen(address) < 10) {
                                printf("\n");
                                printCentered("Address must be at least 10 characters long.", RED, BG_DEFAULT);
                                pauseForEnter();
                                continue;
                            }
                            valid = 1;
                        }
                        strncpy(user.adress, address, sizeof(user.adress) - 1);
                        user.adress[sizeof(user.adress) - 1] = '\0';
                        printf("\n");
                        printCentered("Address updated successfully!", GREEN, BG_DEFAULT);
                        pauseForEnter();
                        break;
                    }

                    case 4: {
                        char phone[20];
                        int valid = 0;
                        while (!valid) {
                            clearScreen();
                            printCentered("+==========================================================+", WHITE, BG_GRAY);
                            printCentered("|                   CHANGE PHONE NUMBER                    |", WHITE, BG_GRAY);
                            printCentered("+==========================================================+", WHITE, BG_GRAY);

                            printCentered("+----------------------------------------------------------+", WHITE, BG_DEFAULT);
                            printCentered("|        Enter new phone number (+XXX-XXXXXXXXX):          |", CYAN, BG_DEFAULT);
                            printCentered("|                                                          |", WHITE, BG_DEFAULT);
                            printCentered("+----------------------------------------------------------+", WHITE, BG_DEFAULT);

                            printf("\033[2A");
                            printf("\033[%dC", centerPadding);
                            fgets(phone, sizeof(phone), stdin);
                            phone[strcspn(phone, "\n")] = 0;

                            if (strlen(phone) != 14 || phone[0] != '+' || phone[4] != '-' ||
                                !isdigit(phone[1]) || !isdigit(phone[2]) || !isdigit(phone[3])) {
                                printf("\n");
                                printCentered("Invalid phone number format. Use +XXX-XXXXXXXXX", RED, BG_DEFAULT);
                                pauseForEnter();
                                continue;
                            }

                            int allDigitsValid = 1;
                            for(int i = 5; i < 14; i++) {
                                if (!isdigit(phone[i])) {
                                    allDigitsValid = 0;
                                    break;
                                }
                            }

                            if (!allDigitsValid) {
                                printf("\n");
                                printCentered("Invalid phone number format. Use +XXX-XXXXXXXXX", RED, BG_DEFAULT);
                                pauseForEnter();
                                continue;
                            }
                            valid = 1;
                        }
                        strncpy(user.tele_num, phone, sizeof(user.tele_num) - 1);
                        user.tele_num[sizeof(user.tele_num) - 1] = '\0';
                        printf("\n");
                        printCentered("Phone number updated successfully!", GREEN, BG_DEFAULT);
                        pauseForEnter();
                        break;
                    }

                    case 5: {
                        char email[100];
                        int valid = 0;
                        while (!valid) {
                            clearScreen();
                            printCentered("+==========================================================+", WHITE, BG_GRAY);
                            printCentered("|                   CHANGE EMAIL                           |", WHITE, BG_GRAY);
                            printCentered("+==========================================================+", WHITE, BG_GRAY);

                            printCentered("+----------------------------------------------------------+", WHITE, BG_DEFAULT);
                            printCentered("|                  Enter new email:                        |", CYAN, BG_DEFAULT);
                            printCentered("|                                                          |", WHITE, BG_DEFAULT);
                            printCentered("+----------------------------------------------------------+", WHITE, BG_DEFAULT);

                            printf("\033[2A");
                            printf("\033[%dC", centerPadding);
                            fgets(email, sizeof(email), stdin);
                            email[strcspn(email, "\n")] = 0;

                            char *at = strchr(email, '@');
                            char *dot = strrchr(email, '.');
                            if (!at || !dot || dot < at || dot - at < 2 || !dot[1] ||
                                at == email || strchr(email, ' ')) {
                                printf("\n");
                                printCentered("Invalid email format. Please enter a valid email address.", RED, BG_DEFAULT);
                                pauseForEnter();
                                continue;
                            }
                            valid = 1;
                        }
                        strncpy(user.email, email, sizeof(user.email) - 1);
                        user.email[sizeof(user.email) - 1] = '\0';
                        printf("\n");
                        printCentered("Email updated successfully!", GREEN, BG_DEFAULT);
                        pauseForEnter();
                        break;
                    }

                    case 6: {
                        clearScreen();
                        printCentered("+==========================================================+", WHITE, BG_GRAY);
                        printCentered("|                CHANGE SECURITY QUESTIONS                 |", WHITE, BG_GRAY);
                        printCentered("+==========================================================+", WHITE, BG_GRAY);

                        // Question 1
                        printCentered("+----------------------------------------------------------+", WHITE, BG_DEFAULT);
                        printCentered("|                Enter new question 1:                     |", CYAN, BG_DEFAULT);
                        printCentered("|                                                          |", WHITE, BG_DEFAULT);
                        printCentered("+----------------------------------------------------------+", WHITE, BG_DEFAULT);

                        printf("\033[2A");
                        printf("\033[%dC", centerPadding);
                        fgets(user.question1, sizeof(user.question1), stdin);
                        user.question1[strcspn(user.question1, "\n")] = 0;

                        // Answer 1
                        printCentered("+----------------------------------------------------------+", WHITE, BG_DEFAULT);
                        printCentered("|                Enter answer 1:                           |", CYAN, BG_DEFAULT);
                        printCentered("|                                                          |", WHITE, BG_DEFAULT);
                        printCentered("+----------------------------------------------------------+", WHITE, BG_DEFAULT);

                        printf("\033[2A");
                        printf("\033[%dC", centerPadding);
                        fgets(user.answer1, sizeof(user.answer1), stdin);
                        user.answer1[strcspn(user.answer1, "\n")] = 0;

                        // Question 2
                        printCentered("+----------------------------------------------------------+", WHITE, BG_DEFAULT);
                        printCentered("|                Enter new question 2:                     |", CYAN, BG_DEFAULT);
                        printCentered("|                                                          |", WHITE, BG_DEFAULT);
                        printCentered("+----------------------------------------------------------+", WHITE, BG_DEFAULT);

                        printf("\033[2A");
                        printf("\033[%dC", centerPadding);
                        fgets(user.question2, sizeof(user.question2), stdin);
                        user.question2[strcspn(user.question2, "\n")] = 0;

                        // Answer 2
                        printCentered("+----------------------------------------------------------+", WHITE, BG_DEFAULT);
                        printCentered("|                Enter answer 2:                           |", CYAN, BG_DEFAULT);
                        printCentered("|                                                          |", WHITE, BG_DEFAULT);
                        printCentered("+----------------------------------------------------------+", WHITE, BG_DEFAULT);

                        printf("\033[2A");
                        printf("\033[%dC", centerPadding);
                        fgets(user.answer2, sizeof(user.answer2), stdin);
                        user.answer2[strcspn(user.answer2, "\n")] = 0;

                        printf("\n");
                        printCentered("Security questions updated successfully!", GREEN, BG_DEFAULT);
                        pauseForEnter();
                        break;
                    }

                    case 0:
                        printf("\n");
                        printCentered("Saving changes and exiting...", YELLOW, BG_DEFAULT);
                        goto SAVE_CHANGES;

                    default:
                        printf("\n");
                        printCentered("Invalid choice. Try again.", RED, BG_DEFAULT);
                        pauseForEnter();
                        break;
                }
                pauseForEnter();
            }
        }

    SAVE_CHANGES:
        writeUserToFile(tempFile, &user);
    }

    fclose(file);
    fclose(tempFile);

    if (found) {
        remove(USER_FILE);
        rename("temp.dat", USER_FILE);
        printf("\n");
        printCentered("Profile updated successfully.", GREEN, BG_DEFAULT);
    } else {
        printCentered("User not found.", RED, BG_DEFAULT);
        remove("temp.dat");
    }
    pauseForEnter();
}




void EmployeeInterface(char* userID) {
    int choice;

    while (1) {
        // Header
        printCentered("+============================================================+", WHITE, BG_GRAY);
        printCentered("|                   EMPLOYEE INTERFACE                       |", WHITE, BG_GRAY);
        printCentered("+============================================================+", WHITE, BG_GRAY);

        // Menu options
        printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);
        printCentered("|                     1. View Profile                        |", CYAN, BG_DEFAULT);
        printCentered("|                    2. Request Leave                        |", CYAN, BG_DEFAULT);
        printCentered("|                  3. View Leave History                     |", CYAN, BG_DEFAULT);
        printCentered("|             4. Delete Pending Leave Requests               |", CYAN, BG_DEFAULT);
        printCentered("|                        0. Logout                           |", RED, BG_DEFAULT);
        printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);

        // Footer with input box
        printf("\n");
        printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);
        printCentered("|                      Enter Your Choice:                    |", YELLOW, BG_DEFAULT);
        printCentered("|                                                            |", WHITE, BG_DEFAULT);
        printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);

        // Calculate center position for input
        int termWidth = 165;  // Assuming 80 column terminal width
        int boxWidth = 75;   // Width of the box
        int leftPadding = (termWidth - boxWidth) / 2;

        // Position cursor inside the box for input
        printf("\033[2A"); // Move up 2 lines
        printf("\033[%dC", leftPadding + 30); // Adjust position inside the box
        scanf("%d", &choice);
        getchar();  // Handle leftover newline
        printf("\033[2B"); // Move cursor back down

        // Handle the user's choice
        switch (choice) {
            case 1:
                clearScreen();
                printCentered("+============================================================+", WHITE, BG_GRAY);
                printCentered("|                       VIEW PROFILE                         |", WHITE, BG_GRAY);
                printCentered("+============================================================+", WHITE, BG_GRAY);
                viewProfile(userID);  // Call the function to view profile

                // Modify profile choice input with box
                {
                    int modifyChoice;
                    printCentered("Modify it? (Enter 1 for Yes, any other number for No): ", CYAN, BG_DEFAULT);
                    printCentered("+-------------------------------------------------------------+", WHITE, BG_DEFAULT);
                    printCentered("|                      Enter Your Choice:                     |", CYAN, BG_DEFAULT);
                    printCentered("|                                                             |", WHITE, BG_DEFAULT);
                    printCentered("+-------------------------------------------------------------+", WHITE, BG_DEFAULT);

                    // Position cursor inside the box for input
                    printf("\033[2A");
                    printf("\033[%dC", leftPadding + 30); // Adjust position inside the box
                    scanf("%d", &modifyChoice);
                    getchar();  // Handle leftover newline
                    printf("\033[2B"); // Move cursor back down

                    if (modifyChoice == 1) {
                        updateProfile(userID);  // Call the function to update profile
                    }
                }
                pauseForEnter();
                break;

            case 2:
                clearScreen();
                requestLeave(userID);
                pauseForEnter();
                break;

            case 3:
                clearScreen();
                viewLeaveEmployeeHistory(userID);
                pauseForEnter();
                break;

            case 4:
                clearScreen();
                Update_pending_leaves(userID);
                pauseForEnter();
                break;

            case 0:
                clearScreen();
                printCentered("Logging out...", YELLOW, BG_DEFAULT);
                pauseForEnter();
                return;

            default:
                printCentered("Invalid choice. Please try again.", RED, BG_DEFAULT);
                pauseForEnter();
        }
        clearScreen();
    }
}


void viewLeaveEmployeeHistory(char *userID) {
    FILE *file = fopen(LEAVE_FILE, "rb");
    struct LeaveRequest tempRequest;
    int found = 0;

    if (file == NULL) {
        printCentered("Error opening leave request file.", RED, BG_DEFAULT);
        return;
    }

    clearScreen();
    printCentered("============================================================", WHITE, BG_GRAY);
    printCentered("                    LEAVE HISTORY                           ", WHITE, BG_GRAY);
    printCentered("============================================================", WHITE, BG_GRAY);

    while (fread(&tempRequest, sizeof(struct LeaveRequest), 1, file) == 1) {
        if (strcmp(tempRequest.ID_Employee, userID) == 0) {
            found = 1;
            printf("\n");
            printf("------------------------------------------------------------\n");
            printf(" %-15s: %d\n", "Leave ID", tempRequest.ID_leave);
            printf(" %-15s: %s\n", "Type", tempRequest.type);
            printf(" %-15s: %s\n", "Title", tempRequest.title);
            printf(" %-15s: %s\n", "Date", tempRequest.date);
            printf(" %-15s: %s\n", "Status", tempRequest.status);
            printf(" %-15s: %s\n", "Reason", tempRequest.content);
            printf(" %-15s: %d\n", "Leave days", tempRequest.days);
            printf("------------------------------------------------------------\n");
        }
    }

    if (!found) {
        printCentered("No leave history found.", RED, BG_DEFAULT);
    }

    fclose(file);
    pauseForEnter();
}

void reviewPendingSignUps() {
    FILE *file = fopen(PENDING_USER_FILE, "rb");
    FILE *tempFile = fopen("temp.dat", "wb");
    FILE *userFile = fopen(USER_FILE, "ab");

    if (file == NULL || tempFile == NULL || userFile == NULL) {
        printCentered("Error opening files. Please try again.", RED, BG_DEFAULT);
        if (file) fclose(file);
        if (tempFile) fclose(tempFile);
        if (userFile) fclose(userFile);
        return;
    }

    struct User user;
    int found = 0;

    clearScreen();
    printCentered("============================================================", WHITE, BG_GRAY);
    printCentered("                 PENDING SIGNUP REQUESTS                    ", WHITE, BG_GRAY);
    printCentered("============================================================", WHITE, BG_GRAY);

    while (readUserFromFile(file, &user)) {
        if (user.signupstate == 2) {
            found = 1;
            printf("\n");
            printCentered("User Details", CYAN, BG_DEFAULT);
            printf("------------------------------------------------------------\n");
            printf(" %-20s: %s\n", "ID", user.ID);
            printf(" %-20s: %s %s\n", "Name", user.first_name, user.last_name);
            printf(" %-20s: %s\n", "Gender", user.gender);
            printf(" %-20s: %s\n", "Address", user.adress);
            printf(" %-20s: %s\n", "Birth Date", user.birth_day);
            printf(" %-20s: %s\n", "Phone", user.tele_num);
            printf(" %-20s: %s\n", "Email", user.email);
            printf(" %-20s: %s\n", "Department", user.departement);
            printf(" %-20s: %s\n", "Recruitment Date", user.recruitment_day);
            printf("------------------------------------------------------------\n");

            int choice;
            do {
                printCentered("1. Approve  2. Reject  3. Skip", CYAN, BG_DEFAULT);
                printCentered("Enter your choice (1/2/3): ", YELLOW, BG_DEFAULT);
                scanf("%d", &choice);
                getchar();
            } while (choice < 1 || choice > 3);

            switch (choice) {
                case 1:
                    user.signupstate = 1;
                    printCentered("Request Approved", GREEN, BG_DEFAULT);
                    writeUserToFile(userFile, &user);
                    break;
                case 2:
                    user.signupstate = 0;
                    printCentered("Request Rejected", RED, BG_DEFAULT);
                    break;
                case 3:
                    user.signupstate = 2;
                    printCentered("Request Skipped", YELLOW, BG_DEFAULT);
                    break;
            }
        }
        writeUserToFile(tempFile, &user);
    }

    fclose(file);
    fclose(tempFile);
    fclose(userFile);

    if (found) {
        remove(PENDING_USER_FILE);
        rename("temp.dat", PENDING_USER_FILE);
        printCentered("All pending sign-ups have been reviewed successfully.", GREEN, BG_DEFAULT);
    } else {
        printCentered("No pending sign-up requests found.", YELLOW, BG_DEFAULT);
        remove("temp.dat");
    }
    pauseForEnter();
}

void requestLeave(char *userID) {
    struct LeaveRequest newRequest;
    FILE *leaveFile = fopen(LEAVE_FILE, "ab");
    FILE *userFile = fopen(USER_FILE, "rb");
    struct User tempUser;
    int userFound = 0;

    if (leaveFile == NULL || userFile == NULL) {
        printCentered("Error opening required files.", RED, BG_DEFAULT);
        if (leaveFile) fclose(leaveFile);
        if (userFile) fclose(userFile);
        return;
    }

    clearScreen();
    printCentered("+============================================================+", WHITE, BG_GRAY);
    printCentered("|                     REQUEST LEAVE                          |", WHITE, BG_GRAY);
    printCentered("+============================================================+", WHITE, BG_GRAY);

    // Input and validate leave type
    printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);
    printCentered("|                  Enter leave details:                      |", CYAN, BG_DEFAULT);
    printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);

    // Input and validate leave type
    do {
        printf("Enter leave type (Annual/Urgent): ");
        scanf("%s", newRequest.type);
        if (strcmp(newRequest.type, "Annual") != 0 && strcmp(newRequest.type, "Urgent") != 0) {
            printf("Invalid leave type. Please enter 'Annual' or 'Urgent'.\n");
        }
    } while (strcmp(newRequest.type, "Annual") != 0 && strcmp(newRequest.type, "Urgent") != 0);

    printf("Enter leave title: ");
    scanf(" %[^\n]", newRequest.title);

    // Get current date in YYYY-MM-DD format
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(newRequest.date, sizeof(newRequest.date), "%d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

    printf("Enter leave content (reason for leave): ");
    scanf(" %[^\n]", newRequest.content);

    // Locate user in the user file first to get available leave days
    while (fread(&tempUser, sizeof(struct User), 1, userFile)) {
        if (strcmp(tempUser.ID, userID) == 0) {
            userFound = 1;
            strcpy(newRequest.first_name, tempUser.first_name);
            strcpy(newRequest.last_name, tempUser.last_name);
            break;
        }
    }

    if (!userFound) {
        printCentered("Error: User not found.", RED, BG_DEFAULT);
        pauseForEnter();
        fclose(userFile);
        fclose(leaveFile);
        return;
    }

    // Input and validate leave days
    do {
        printf("Enter the number of leave days: ");
        scanf("%d", &newRequest.days);

        if (strcmp(newRequest.type, "Annual") == 0 && newRequest.days > tempUser.Annual_leave_bonus) {
            char message[100];
            snprintf(message, sizeof(message), "Note: You only have %d annual leave days available.", tempUser.Annual_leave_bonus);
            printCentered(message, YELLOW, BG_DEFAULT);
            printf("Please enter a number within your available leave days, ");
        }
    } while (strcmp(newRequest.type, "Annual") == 0 && newRequest.days > tempUser.Annual_leave_bonus);

    strcpy(newRequest.ID_Employee, userID);
    strcpy(newRequest.status, "under process");

    rewind(userFile);

    // Generate unique ID for leave request
    FILE *tempFile = fopen(LEAVE_FILE, "rb");
    int lastID = 0;

    if (tempFile != NULL) {
        struct LeaveRequest tempRequest;
        while (fread(&tempRequest, sizeof(struct LeaveRequest), 1, tempFile)) {
            if (tempRequest.ID_leave > lastID) {
                lastID = tempRequest.ID_leave;
            }
        }
        fclose(tempFile);
    }

    newRequest.ID_leave = lastID + 1;

    // Write the new leave request
    fwrite(&newRequest, sizeof(struct LeaveRequest), 1, leaveFile);
    fclose(leaveFile);
    fclose(userFile);

    printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);
    printCentered("|            Leave request submitted successfully!            |", GREEN, BG_DEFAULT);
    printCentered("+------------------------------------------------------------+", WHITE, BG_DEFAULT);
    pauseForEnter();
}


void approveLeave(int leaveID) {
    FILE *leaveFile = fopen(LEAVE_FILE, "rb");  // Open the leave file in read binary mode
    FILE *tempFile = fopen("temp_leaves.dat", "wb");  // Temporary file for updated leave requests
    struct LeaveRequest tempRequest;
    int found = 0;

    if (leaveFile == NULL || tempFile == NULL) {
        printCentered("Error opening leave file.", RED, BG_DEFAULT);
        if (leaveFile) fclose(leaveFile);
        if (tempFile) fclose(tempFile);
        return;
    }

    clearScreen();
    printCentered("============================================================", WHITE, BG_GRAY);
    printCentered("                   APPROVE LEAVE REQUEST                     ", YELLOW, BG_GRAY);
    printCentered("============================================================", WHITE, BG_GRAY);

    while (fread(&tempRequest, sizeof(struct LeaveRequest), 1, leaveFile)) {
        if (tempRequest.ID_leave == leaveID) {
            // Approve the leave request
            strcpy(tempRequest.status, "Approved");
            found = 1;

            printf("\n");
            printCentered("Leave Request Details:", CYAN, BG_DEFAULT);
            printf("------------------------------------------------------------\n");
            printf(" %-20s: %d\n", "Leave ID", tempRequest.ID_leave);
            printf(" %-20s: %s %s\n", "Employee Name", tempRequest.first_name, tempRequest.last_name);
            printf(" %-20s: %s\n", "Leave Type", tempRequest.type);
            printf(" %-20s: %d\n", "Leave Days", tempRequest.days);
            printf("------------------------------------------------------------\n");

            // Deduct leave bonus for Annual leaves
            if (strcmp(tempRequest.type, "Annual") == 0) {
                reduceAnnualLeaveBonus(tempRequest.ID_Employee, tempRequest.days);
            }
        }

        // Write the updated or unchanged request to the temp file
        fwrite(&tempRequest, sizeof(struct LeaveRequest), 1, tempFile);
    }

    fclose(leaveFile);
    fclose(tempFile);

    // Replace the original leave file with the updated file if leave was found
    if (found) {
        remove(LEAVE_FILE);
        rename("temp_leaves.dat", LEAVE_FILE);
        printCentered("Leave request approved and file updated successfully.", GREEN, BG_DEFAULT);
    } else {
        remove("temp_leaves.dat");  // Clean up temporary file
        char errorMsg[100];
        snprintf(errorMsg, sizeof(errorMsg), "Error: Leave ID %d not found.", leaveID);
        printCentered(errorMsg, RED, BG_DEFAULT);
    }
}

void reduceAnnualLeaveBonus(char *userID, int days) {
    FILE *userFile = fopen(USER_FILE, "rb");  // Open user file in read binary mode
    FILE *tempFile = fopen("temp_users.dat", "wb");  // Temporary file for updated user data
    struct User tempUser;
    int found = 0;

    if (userFile == NULL || tempFile == NULL) {
        printCentered("Error opening user file.", RED, BG_DEFAULT);
        if (userFile) fclose(userFile);
        if (tempFile) fclose(tempFile);
        return;
    }

    while (fread(&tempUser, sizeof(struct User), 1, userFile)) {
        if (strcmp(tempUser.ID, userID) == 0) {
            printf("\n");
            printCentered("Updating Annual Leave Bonus:", CYAN, BG_DEFAULT);
            printf("------------------------------------------------------------\n");
            printf(" %-20s: %s\n", "User ID", tempUser.ID);
            printf(" %-20s: %s %s\n", "Employee Name", tempUser.first_name, tempUser.last_name);
            printf(" %-20s: %d days\n", "Current Bonus", tempUser.Annual_leave_bonus);

            tempUser.Annual_leave_bonus -= days;

            if (tempUser.Annual_leave_bonus < 0) {
                printCentered("Warning: Requested leave days exceed available bonus. Setting bonus to 0.", YELLOW, BG_DEFAULT);
                tempUser.Annual_leave_bonus = 0;
            }

            printf(" %-20s: %d days\n", "Updated Bonus", tempUser.Annual_leave_bonus);
            printf("------------------------------------------------------------\n");
            found = 1;
        }

        // Write updated or unchanged user data to the temp file
        fwrite(&tempUser, sizeof(struct User), 1, tempFile);
    }

    fclose(userFile);
    fclose(tempFile);

    // Replace the original file with the updated file
    if (found) {
        remove(USER_FILE);
        rename("temp_users.dat", USER_FILE);
        char successMsg[100];
        snprintf(successMsg, sizeof(successMsg), "Annual leave bonus updated successfully for User ID: %s", userID);
        printCentered(successMsg, GREEN, BG_DEFAULT);
    } else {
        remove("temp_users.dat");
        char errorMsg[100];
        snprintf(errorMsg, sizeof(errorMsg), "Error: User ID %s not found.", userID);
        printCentered(errorMsg, RED, BG_DEFAULT);
    }
}

void reviewLeaveRequests() {
    FILE *leaveFile = fopen(LEAVE_FILE, "r+b");
    FILE *tempFile = fopen("temp_leave_requests.dat", "wb");
    struct LeaveRequest tempRequest;
    int choice;
    int hasPendingRequests = 0;

    if (leaveFile == NULL || tempFile == NULL) {
        printCentered("Error opening leave requests file.", RED, BG_DEFAULT);
        if (leaveFile) fclose(leaveFile);
        if (tempFile) fclose(tempFile);
        return;
    }

    clearScreen();
    printCentered("============================================================", WHITE, BG_GRAY);
    printCentered("                   REVIEW LEAVE REQUESTS                     ", WHITE, BG_GRAY);
    printCentered("============================================================", WHITE, BG_GRAY);

    while (fread(&tempRequest, sizeof(struct LeaveRequest), 1, leaveFile) == 1) {
        if (strcmp(tempRequest.status, "under process") == 0) {
            hasPendingRequests = 1;

            printf("\n");
            printCentered("Leave Request Details:", CYAN, BG_DEFAULT);
            printf("------------------------------------------------------------\n");
            printf(" %-20s: %s\n", "First Name", tempRequest.first_name);
            printf(" %-20s: %s\n", "Last Name", tempRequest.last_name);
            printf(" %-20s: %d\n", "Leave ID", tempRequest.ID_leave);
            printf(" %-20s: %s\n", "Type", tempRequest.type);
            printf(" %-20s: %s\n", "Title", tempRequest.title);
            printf(" %-20s: %s\n", "Date", tempRequest.date);
            printf(" %-20s: %s\n", "Reason", tempRequest.content);
            printf(" %-20s: %s\n", "Status", tempRequest.status);
            printf(" %-20s: %d\n", "Leave Days", tempRequest.days);
            printf("------------------------------------------------------------\n");

            printCentered("1. Approve", GREEN, BG_DEFAULT);
            printCentered("2. Reject", RED, BG_DEFAULT);
            printCentered("3. Skip", YELLOW, BG_DEFAULT);
            printCentered("Enter your choice: ", CYAN, BG_DEFAULT);
            scanf("%d", &choice);

            if (choice == 1) {
                strcpy(tempRequest.status, "Approved");
                approveLeave(tempRequest.ID_leave);
                printCentered("Leave request approved.", GREEN, BG_DEFAULT);
            } else if (choice == 2) {
                strcpy(tempRequest.status, "Rejected");
                printCentered("Leave request rejected.", RED, BG_DEFAULT);
            } else if (choice == 3) {
                printCentered("Skipped leave request.", YELLOW, BG_DEFAULT);
            } else {
                printCentered("Invalid choice. Skipping request.", RED, BG_DEFAULT);
            }
        }

        fwrite(&tempRequest, sizeof(struct LeaveRequest), 1, tempFile);
    }

    fclose(leaveFile);
    fclose(tempFile);

    remove(LEAVE_FILE);
    rename("temp_leave_requests.dat", LEAVE_FILE);

    if (!hasPendingRequests) {
        printCentered("No pending leave requests to review.", YELLOW, BG_DEFAULT);
    } else {
        printCentered("Leave requests have been reviewed and updated.", GREEN, BG_DEFAULT);
    }

    pauseForEnter();
}



// Function to pause and ask for Enter
void pauseForEnter() {
    printf("\nPress Enter to continue...");
    getchar();  // Wait for Enter
}


void recoverPassword() {
    FILE *userFile = fopen(USER_FILE, "rb+");
    struct User tempUser;
    char userID[50];
    char answer1[100], answer2[100];
    int found = 0;

    if (userFile == NULL) {
        printf("Error opening user file.\n");
        return;
    }

    printf("Enter your User ID: ");
    scanf("%49s", userID);
    getchar();

    // Search for the user in the file
    while (fread(&tempUser, sizeof(struct User), 1, userFile)) {
        if (strcmp(tempUser.ID, userID) == 0) {
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("User ID not found.\n");
        fclose(userFile);
        return;
    }

    // Ask security questions
    printf("\nAnswer the following security questions:\n");
    printf("%s: ", tempUser.question1);
    fgets(answer1, sizeof(answer1), stdin);
    answer1[strcspn(answer1, "\n")] = '\0';

    printf("%s: ", tempUser.question2);
    fgets(answer2, sizeof(answer2), stdin);
    answer2[strcspn(answer2, "\n")] = '\0';

    // Validate answers
    if (strcmp(answer1, tempUser.answer1) == 0 && strcmp(answer2, tempUser.answer2) == 0) {
        printf("Security questions verified!\n");
        printf("Enter your new password: ");
        scanf("%19s", tempUser.password);
        getchar();

        // Update the user's password in the file
        fseek(userFile, -sizeof(struct User), SEEK_CUR);
        fwrite(&tempUser, sizeof(struct User), 1, userFile);
        printf("Password updated successfully!\n");
    } else {
        printf("Incorrect answers to security questions.\n");
        printf("1. Retry\n2. Return to Main Menu\n");
        int choice;
        scanf("%d", &choice);
        getchar();
        if (choice == 1) {
            recoverPassword();
        }
    }

    fclose(userFile);
}


// Function to display the main menu
void displayMenu() {
    clearScreen();
    int choice;

    // Header with animated decorative box
    while(1) {
        clearScreen();
        // First frame
        printCentered(".-------------------------------------------------.", BLUE, BG_CYAN);
        printCentered("|            LEAVE REQUEST APPLICATION            |", WHITE, BG_CYAN);
        printCentered("'-------------------------------------------------'", BLUE, BG_CYAN);
        Sleep(100);

        clearScreen();
        // Second frame
        printCentered("/-------------------------------------------------\\", CYAN, BG_DEFAULT);
        printCentered("|            LEAVE REQUEST APPLICATION            |", WHITE, BG_BLUE);
        printCentered("\\-------------------------------------------------/", CYAN, BG_DEFAULT);
        Sleep(100);

        clearScreen();
        // Third frame
        printCentered("+-------------------------------------------------+", WHITE, BG_CYAN);
        printCentered("|            LEAVE REQUEST APPLICATION            |", BLUE, BG_CYAN);
        printCentered("+-------------------------------------------------+", WHITE, BG_CYAN);
        Sleep(100);

        // Break after 3 animation cycles
        static int count = 0;
        if(++count >= 3) {
            break;
        }
    }
    printf("\n");

    // Menu options in a decorative box
    printCentered("*================================================*", WHITE, BG_DEFAULT);
    printCentered("|                   MAIN MENU                    |", BLUE, BG_DEFAULT);
    printCentered("*================================================*", WHITE, BG_DEFAULT);
    printCentered("*------------------------------------------------*", WHITE, BG_DEFAULT);
    printCentered("|                                                |", CYAN, BG_DEFAULT);
    printCentered("|       [1]  Sign Up (First Time User)           |", CYAN, BG_DEFAULT);
    printCentered("|       [2]  Log In                              |", CYAN, BG_DEFAULT);
    printCentered("|       [3]  Q&A (Learn About Us)                |", CYAN, BG_DEFAULT);
    printCentered("|       [4]  Exit                                |", CYAN, BG_DEFAULT);
    printCentered("|                                                |", CYAN, BG_DEFAULT);
    printCentered("+------------------------------------------------+", WHITE, BG_DEFAULT);

    // Required headers for terminal size detection
    #ifdef _WIN32
    #include <windows.h>
    #else
    #include <sys/ioctl.h>
    #include <unistd.h>
    #endif

    // Footer with input box
    printf("\n");
    printCentered("+========================================+", WHITE, BG_DEFAULT);
    printCentered("|             Enter Choice:              |", BLUE, BG_DEFAULT);
    printCentered("|                                        |", BLUE, BG_DEFAULT);
    printCentered("+========================================+", WHITE, BG_DEFAULT);

    // Get terminal width
    int termWidth;
    #ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    termWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    #else
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    termWidth = w.ws_col;
    #endif

    int boxWidth = 40;  // Width of input box
    int leftPadding = (termWidth - boxWidth) / 2;

    // Move cursor to middle of box
    printf("\033[2A");  // Move up 2 lines
    printf("\033[%dC", leftPadding + (boxWidth/2) - 1); // Center cursor

    // Get input with validation
    char inputBuffer[10];
    int validInput = 0;

    // Clear input buffer
    fflush(stdin);

    // Get input and validate
    if (fgets(inputBuffer, sizeof(inputBuffer), stdin) != NULL) {
        if (sscanf(inputBuffer, "%d", &choice) == 1 && choice >= 1 && choice <= 4) {
            validInput = 1;
        }
    }

    if (!validInput) {
        printf("\033[2B"); // Move cursor down
        printCentered("Invalid input. Please enter a number between 1 and 4.", RED, BG_DEFAULT);
        pauseForEnter();
        displayMenu();
        return;
    }

    // Move cursor back down
    printf("\033[2B");

    // Handle the choice
    handleChoice(choice);
}

// Function to handle the user's choice (called from displayMenu)
void handleChoice(int choice) {
    switch (choice) {
        case 1:  // Sign Up
            clearScreen();
            printCentered("============================================================", WHITE, BG_GRAY);
            printCentered("                       Sign Up Page                         ", WHITE, BG_GRAY);
            printCentered("============================================================", WHITE, BG_GRAY);
            showTermsPromptAndProceed();
            pauseForEnter();  // Pause after completing sign-up
            break;

        case 2:  // Log In
            clearScreen();
            printCentered("============================================================", WHITE, BG_GRAY);
            printCentered("                       Log In Page                          ", WHITE, BG_GRAY);
            printCentered("============================================================", WHITE, BG_GRAY);
            if (logIn() == 1) {  // Successful login
                pauseForEnter();  // Allow the user to see the success message
            }
            break;

        case 3:  // About Us
            clearScreen();
            printCentered("============================================================", WHITE, BG_GRAY);
            printCentered("                         About Us                           ", WHITE, BG_GRAY);
            printCentered("============================================================", WHITE, BG_GRAY);
            createAndUseFAQ();  // Create, display, and delete the FAQ file
            pauseForEnter();  // Pause after showing content
            break;

        case 4:  // Exit
            clearScreen();
            printCentered("============================================================", WHITE, BG_GRAY);
            printCentered("                     Exiting the Application                ", WHITE, BG_GRAY);
            printCentered("============================================================", WHITE, BG_GRAY);
            printCentered("Thank you for using the Leave Request System! Goodbye!", GREEN, BG_DEFAULT);
            exit(0);  // Exit the program

        default:  // Invalid choice
            printCentered("Invalid choice. Please select a number between 1 and 4.", RED, BG_DEFAULT);
            pauseForEnter();  // Pause and wait for user input
    }
}


// Function to create and populate FAQ.dat
void createAndUseFAQ() {
    FILE *faqFile = fopen("FAQ.dat", "w");  // Open the file for writing
    if (faqFile == NULL) {
        printf("Error creating FAQ file.\n");
        return;
    }

    // Write some FAQ content into the file
    // FAQ 1 - Login
    fprintf(faqFile, "1. How do I log in?\n");
    fprintf(faqFile, "   Answer: Enter your ID and password on the Log In page.\n");
    fprintf(faqFile, "   Note: If you don't have an account, you can sign up first.\n");

    // FAQ 2 - Password Recovery
    fprintf(faqFile, "2. How do I recover my password?\n");
    fprintf(faqFile, "   Answer: Click on the 'Forgot Password' link to recover your password.\n");
    fprintf(faqFile, "   Note: You will be asked to answer security questions or receive a reset link via email.\n");

    // FAQ 3 - Contacting Support
    fprintf(faqFile, "3. How can I contact support?\n");
    fprintf(faqFile, "   Answer: Reach out to support@example.com for assistance.\n");
    fprintf(faqFile, "   Alternatively, you can call our customer service at +1-800-123-4567.\n");

    // FAQ 4 - About Leave Request System
    fprintf(faqFile, "4. Where can I find more information about the Leave Request system?\n");
    fprintf(faqFile, "   Answer: Visit the About Us page for more information.\n");
    fprintf(faqFile, "   Additional resources are available on the system's user guide.\n");

    // FAQ 5 - How to Submit a Leave Request
    fprintf(faqFile, "5. How do I submit a leave request?\n");
    fprintf(faqFile, "   Answer: Log in to your account and navigate to the 'Leave Request' page.\n");
    fprintf(faqFile, "   Choose the dates you wish to take leave and select the reason.\n");
    fprintf(faqFile, "   Submit your request for approval by your manager.\n");

    // FAQ 6 - Leave Request Approval Process
    fprintf(faqFile, "6. How does the leave request approval process work?\n");
    fprintf(faqFile, "   Answer: Once you submit your leave request, it will be sent to your manager for approval.\n");
    fprintf(faqFile, "   Your manager will either approve or deny the request, and you will be notified.\n");

    // FAQ 7 - Employee Leave Balance
    fprintf(faqFile, "7. How can I check my leave balance?\n");
    fprintf(faqFile, "   Answer: Log in and go to your profile page. You will see your current leave balance.\n");
    fprintf(faqFile, "   Note: The leave balance will be updated after each request is approved.\n");

    // FAQ 8 - Changing Your Personal Information
    fprintf(faqFile, "8. How can I change my personal information?\n");
    fprintf(faqFile, "   Answer: Go to the 'Profile' section and click on 'Edit Profile' to update your details.\n");
    fprintf(faqFile, "   You can change your name, address, phone number, and email.\n");

    // FAQ 9 - Account Deactivation
    fprintf(faqFile, "9. How do I deactivate my account?\n");
    fprintf(faqFile, "   Answer: If you no longer wish to use the system, you can request account deactivation.\n");
    fprintf(faqFile, "   Contact support@example.com for assistance with deactivating your account.\n");

    // FAQ 10 - Leave Entitlement
    fprintf(faqFile, "10. How is my leave entitlement calculated?\n");
    fprintf(faqFile, "    Answer: Leave entitlement is calculated based on your hire date and the company's policies.\n");
    fprintf(faqFile, "    Employees are entitled to a certain number of days off each year.\n");

    // FAQ 11 - System Requirements
    fprintf(faqFile, "11. What are the system requirements to use the Leave Request system?\n");
    fprintf(faqFile, "    Answer: You can access the Leave Request system via a web browser on a computer or mobile device.\n");
    fprintf(faqFile, "    Supported browsers include Google Chrome, Firefox, and Safari.\n");

    // FAQ 12 - Security Measures
    fprintf(faqFile, "12. What security measures are in place to protect my data?\n");
    fprintf(faqFile, "    Answer: All data is stored securely using encryption. Your password is hashed for protection.\n");
    fprintf(faqFile, "    Regular security audits ensure the system is protected from unauthorized access.\n");

    // FAQ 13 - Multi-factor Authentication
    fprintf(faqFile, "13. Does the system support multi-factor authentication?\n");
    fprintf(faqFile, "    Answer: Yes, multi-factor authentication (MFA) is enabled for additional security.\n");
    fprintf(faqFile, "    You will be prompted to enter a code sent to your email or phone number when logging in.\n");

    // FAQ 14 - Browser Issues
    fprintf(faqFile, "14. I am having issues with the system in my browser. What should I do?\n");
    fprintf(faqFile, "    Answer: Try clearing your browser's cache and cookies. Make sure your browser is up to date.\n");
    fprintf(faqFile, "    If the problem persists, contact support for further assistance.\n");

    // FAQ 15 - System Downtime
    fprintf(faqFile, "15. What should I do if the system is down or unavailable?\n");
    fprintf(faqFile, "    Answer: Check our website or social media for updates on system maintenance or outages.\n");
    fprintf(faqFile, "    If you are experiencing issues, contact support for help.\n");

    fprintf(faqFile, "                                                                           Programmer:Zaynab Ait Addi\n");
    // Close the FAQ file after writing
    fclose(faqFile);

    // Display FAQ content
    faqFile = fopen("FAQ.dat", "r");  // Open the file for reading
    if (faqFile == NULL) {
        printf("Error opening FAQ file for reading.\n");
        return;
    }

    char ch;
    while ((ch = fgetc(faqFile)) != EOF) {
        putchar(ch);  // Display the content of the FAQ file to the user
    }

    fclose(faqFile);  // Close the FAQ file

    // Delete the FAQ file after usage
    if (remove("FAQ.dat") == 0) {
    } else {
        printf("\nError deleting FAQ file.\n");
    }
}

void showTermsOfService() {
    clearScreen();
    printCentered("============================================================", WHITE, BG_GRAY);
    printCentered("                Terms of Service                            ", WHITE, BG_GRAY);
    printCentered("============================================================", WHITE, BG_GRAY);

    // Print the Terms of Service content (long form)
    printf("\nTerms of Service\n");
    printf("=================\n");
    printf("By signing up for the Leave Request system, you agree to the following terms:\n");
    printf("1. You must provide accurate and complete information during sign-up.\n");
    printf("2. You are responsible for keeping your account details secure.\n");
    printf("3. Unauthorized access to the system is prohibited.\n");
    printf("4. All leave requests must comply with the company's policies.\n");
    printf("5. We reserve the right to suspend or terminate accounts that violate terms.\n");
    printf("6. Your data will be stored securely, but you are responsible for your privacy settings.\n");
    printf("7. The system may be updated or changed without prior notice.\n");
    printf("8. We are not liable for any loss of data or service disruption.\n");

    printf("\nPress any key to go back to the previous menu.");
    getchar();  // Wait for the user to press any key to go back
}

int acceptTermsOfService() {
    clearScreen();
    printCentered("============================================================", WHITE, BG_GRAY);
    printCentered("                Terms of Service                            ", WHITE, BG_GRAY);
    printCentered("============================================================", WHITE, BG_GRAY);

    // Prompt user for acceptance
    printCentered("Do you accept the Terms of Service? (y/n): ", CYAN, BG_DEFAULT);
    char choice;
    scanf(" %c", &choice);
    getchar();  // To consume the trailing newline

    if (choice == 'y' || choice == 'Y') {
        return 1;  // User accepted the terms
    } else if (choice == 'n' || choice == 'N') {
        printCentered("You must accept the Terms of Service to proceed with sign-up.", RED, BG_DEFAULT);
        pauseForEnter();  // Wait for user to acknowledge and return to the menu
        return 0;  // User did not accept the terms
    } else {
        printCentered("Invalid choice. Please enter 'y' to accept or 'n' to refuse.", RED, BG_DEFAULT);
        pauseForEnter();
        return acceptTermsOfService();  // Retry the prompt
    }
}

void showTermsPromptAndProceed() {
    // Prompt the user if they want to view the full Terms of Service before accepting

    printCentered("Do you want to read the full Terms of Service? (y/n): ", CYAN, BG_DEFAULT);
    char choice;
    scanf(" %c", &choice);
    getchar();  // To consume the trailing newline

    if (choice == 'y' || choice == 'Y') {
        showTermsOfService();  // Show the full Terms of Service
    }

    // Proceed to ask for acceptance
    if (acceptTermsOfService()) {
        signUp();  // Proceed with the sign-up process if accepted
    } else {
        return;  // User refused the terms, return to the menu
    }
}

#include <windows.h>  // For Sleep and console manipulation functions

// Function to set cursor position
void setCursorPosition(int x, int y) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {x, y};
    SetConsoleCursorPosition(hConsole, pos);
}

// Big ASCII art for the title with colors
void displayBigLogo(int x, int y) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY); // Bright Blue
    setCursorPosition(x, y);
    printf(" _     _____   ___     _____ \n");
    setCursorPosition(x, y + 1);
    printf("| |   | ____|  / \\  \\ / / __|\n");

    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY); // Cyan
    setCursorPosition(x, y + 2);
    printf("| |   |  _|   / _ \\  V /|  _|\n");
    setCursorPosition(x, y + 3);
    printf("| |___| |___ / ___ \\| | | |__\n");
    setCursorPosition(x, y + 4);
    printf("|_____|_____/_/   \\_\\_| |____|\n");

    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY); // Bright Blue
    setCursorPosition(x, y + 6);
    printf(" ____  _____ ___  _   _ _____ ____ _____ \n");
    setCursorPosition(x, y + 7);
    printf("| __ \\| ____/ _ \\| | | | ____/ ___|_   _|\n");

    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // White
    setCursorPosition(x, y + 8);
    printf("| |_) |  _|| | | | | | |  _| \\___ \\ | |  \n");
    setCursorPosition(x, y + 9);
    printf("|  _ <| |__| |_| | |_| | |___ ___) || |  \n");
    setCursorPosition(x, y + 10);
    printf("|_| \\_\\_____\\__\\_\\\\___/|_____|____/ |_|  \n");

    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY); // Bright Green
    setCursorPosition(x, y + 12);
    printf(" ____  ____   ___   ____ ____      _    __  __ __  __ _____ \n");
    setCursorPosition(x, y + 13);
    printf("|  _ \\|  _ \\ / _ \\ / ___|  _ \\    / \\  |  \\/  |  \\/  | ____|\n");
    setCursorPosition(x, y + 14);
    printf("| |_) | |_) | | | | |  _| |_) |  / _ \\ | |\\/| | |\\/| |  _|  \n");
    setCursorPosition(x, y + 15);
    printf("|  __/|  _ <| |_| | |_| |  _ <  / ___ \\| |  | | |  | | |___ \n");
    setCursorPosition(x, y + 16);
    printf("|_|   |_| \\_\\\\___/ \\____|_| \\_\\/_/   \\_\\_|  |_|_|  |_|_____|\n");

    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // Reset to white
}

// Function to display the logo with animated surroundings
void displayLogoWithAnimation() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    int width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    int centerX = width / 2;
    int centerY = height / 2;

    // Start with a smaller box that will grow to fill the screen
    int boxWidth = width / 4;  // Start at 1/4 of screen width
    int boxHeight = height / 4; // Start at 1/4 of screen height

    // Animation frames using only ~/|_=-+()
    char* borders[] = {
        "|-=-|+~+|",  // Frame 1
        "|-_-|~+~|",  // Frame 2
        "|_-_|+~+|",  // Frame 3
        "|-_-|~+~|"   // Frame 4
    };
    int frameCount = 4;

    // Calculate growth per frame to reach full screen
    float widthGrowth = (float)(width - boxWidth) / 16;
    float heightGrowth = (float)(height - boxHeight) / 16;

    for (int frame = 0; frame < 16; frame++) {
        clearScreen();
        int currentFrame = frame % frameCount;

        // Increase box size with each frame
        boxWidth += (int)widthGrowth;
        boxHeight += (int)heightGrowth;

        // Ensure box stays centered
        int startX = centerX - (boxWidth / 2);
        int startY = centerY - (boxHeight / 2);

        SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);

        // Draw animated box borders
        for (int x = 0; x < boxWidth; x++) {
            setCursorPosition(startX + x, startY);
            printf("%c", x % 2 == 0 ? '-' : '=');
            setCursorPosition(startX + x, startY + boxHeight - 1);
            printf("%c", x % 2 == 0 ? '=' : '-');
        }

        for (int y = 0; y < boxHeight; y++) {
            setCursorPosition(startX, startY + y);
            printf("%c", y % 2 == 0 ? '|' : '|');
            setCursorPosition(startX + boxWidth - 1, startY + y);
            printf("%c", y % 2 == 0 ? '|' : '|');
        }

        // Draw corners with rotating patterns
        char corners[] = "()";
        setCursorPosition(startX, startY);
        printf("%c", corners[frame % 2]);
        setCursorPosition(startX + boxWidth - 1, startY);
        printf("%c", corners[(frame + 1) % 2]);
        setCursorPosition(startX, startY + boxHeight - 1);
        printf("%c", corners[(frame + 1) % 2]);
        setCursorPosition(startX + boxWidth - 1, startY + boxHeight - 1);
        printf("%c", corners[frame % 2]);

        // Add decorative patterns
        SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        for (int i = 1; i < boxWidth - 1; i++) {
            if (i % 4 == frame % 4) {
                setCursorPosition(startX + i, startY + 1);
                printf("~");
                setCursorPosition(startX + i, startY + boxHeight - 2);
                printf("~");
            }
        }

        // Calculate logo position to keep it centered in the growing box
        int logoX = startX + (boxWidth - 50) / 2;  // 50 is approximate logo width
        int logoY = startY + (boxHeight - 17) / 2; // 17 is approximate logo height
        displayBigLogo(logoX, logoY);

        Sleep(150);
    }

    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // Reset color
}



// Function to print the logo
void drawPattern() {
    const char *pattern[] = {
        "                                                      !P!.                                            ",
        "                                                  ^5&@@@&G^                                           ",
        "                                               ^5&@@@@@@@@@&G~                                        ",
        "                           .^^^^^^^^^^^.      :@@@@@@@@@@@@@@@~                                       ",
        "                          ^@@@@@@@@@@@@@.      J@@@@@@@@@@@@@P      ^GGGGGGGP                         ",
        "                         .&@@@@@@@@@@@@@B       B@@@@@@@@@@@&      .&@@@@@@@@P                        ",
        "                         P@@@@@@@@@@@@@@@7      .@@@@@@@@@@@^      G@@@@@@@@@@!                       ",
        "                        ~@@@@@@@@@@@@@@@@@.      ^JJJJJJJJJ~      ^@@@@@@@@@@@&                       ",
        "                       .@@@@@@@@@@@@@@@@@@#                        ^5&@@@@@@#J.                       ",
        "                       .?#@@@@@@@@@@@@@@@B!                           :P&#J:                          ",
        "               ^:         :Y&@@@@@@@@@#7.                                      .!GJ.                  ",
        "            ^P&@@&J:         :Y#@@@#7.                                       ^G@@@@@#?                ",
        "         ^P&@@@@@@@@&J:         :!.                                          !@@@@@@@#                ",
        "      ^5&@@@@@@@@@@@@@@#?.                                                    5@@@@@&.                ",
        "   ^Y&@@@@@@@@@@@@@@@@@@@@#?.                                                  :::::.                 ",
        ".5&@@@@@@@@@@@@@@@@@@@@@@@@@@#7                                                                       ",
        ".&@@@@@@@@@@@@@@@@@@@@@@@@@@@@5                                                                       ",
        " ^@@@@@@@@@@@@@@@@@@@@@@@@@@@#                                                                        ",
        "  J@@@@@@@@@@@@@@@@@@@@@@@@@@:   5&#####? :##:   J&! :5BGB&G.    .B&5     B#.   P&:                   ",
        "   B@@@@@@@@@@@@@@@@@@@@@@@@7   ^@&^:::.  B@@@^ .@&  &@G..^~    7&G@@.   J@G   ~@B                    ",
        "   .@@@@@@@@@@@@@@@@@@@@@@@G    B@&B##Y  !@G:&@^P@!  ^P&&B?   .#@J P@7  :@@&###@@:                    ",
        "    J@@@@@@@@@@@@@@@@@@@@@&.   ?@B:::^.  &@. .&@@# .J!. !@@! 7@&GPPB@B  G@?   Y@P                 :^  ",
        "     B@@@@@@@@@@@@@@@@@@@@~    G&#####! !&J   .##: .5##GBP! J#7    .#B .#G    G#.                 5G  ",
        "     .&@@@@@@@@@@@@@@@@@@P                                                                       :@5  ",
        "      ...................                                                                        #@:  ",
        "                                                                                                B@B   ",
        "         Y##############B7.............................................................^YG###BY!^5:   ",
        "         .@@@@@@@@@@@@@@@@5^:::::::::::::::::::::::::::::::::::::::::::::::::::::::  7&@@@@@@@@@&!    ",
        "          :&@@@@@@@@@@@@@@@@#!..................................................... 5@@@@@@@@@@@@@Y   ",
        "            J@@@@@@@@@@@@@@@@@##5?????????????????????????????????????????????????..@@@@@@@@@@@@@@@:  ",
        "             .P@@@@@@@@@@@@@@@@@&P~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~: &@@@@@@@@@@@@@&.  ",
        "               :B@@@@@@@@@@@@@@@@@@@@&GY???????????????????????????????????????J5B@^:&@@@@@@@@@@@&:   ",
        "                 .?&@@@@@@@@@@@@@@@@@@@&#BBBBBBBGGGGGGGGGGGGGGGGGGGGGGGGBBBBBBB#&@@@Y.!B&@@@@@@#7     ",
        "                    :Y#@@@@@@@@@@@@@@@@@@@@@@@@@&#GGJJJ7~~~~~~~~?JJYGG&&@@@@@@@@@@@@@Y   .^~~:        ",
        "                       .~P&@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@&BJ^                  ",
        "                           .^JB&@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#G7:                      ",
        "                                .^!YG#&@@@@@@@@@@@@@@@@@@@@@@@@@@@@@&&BPJ~:                           ",
        "                                       .:^7JPGGGGGG#&&&&&#GGGGY?7^^.                                  "
    };

    // Clear screen before printing the pattern
    clearScreen();

    // Print each line of the pattern centered
    for (int i = 0; i < sizeof(pattern) / sizeof(pattern[0]); i++) {
        printCentered(pattern[i], GREEN,BG_DEFAULT);
}
}

int main() {
    sleep(3);
    drawPattern();
    sleep(5); // Display the pattern for 5 seconds
    clearScreen();
    // Set up Unicode support
    #ifdef _WIN32
        // For Windows: Set console to UTF-8
        SetConsoleOutputCP(65001);
        setlocale(LC_ALL, "en_US.UTF-8");
    #else
        // For Unix-like systems
        setlocale(LC_ALL, "en_US.UTF-8");
    #endif

    // Clear screen for better display
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif

    // Preload setup
    createFilesIfNotExist();  // Ensure required files are created
    ensureManagerExists();    // Ensure a Manager exists
    addUsersAndLeaves();
    clearScreen();
    displayLogoWithAnimation();

    while (1) {  // Main menu loop
        displayMenu();  // Display the menu and handle user choice
        pauseForEnter();
    }
    return 0;  // Return success (not reached in normal operation)
}
