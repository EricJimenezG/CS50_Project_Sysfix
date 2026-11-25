#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

// ANSI Escape Codes for Terminal Colors
#define COLOR_RESET  "\033[0m"
#define COLOR_CYAN   "\033[1;36m"
#define COLOR_GREEN  "\033[1;32m"
#define COLOR_RED    "\033[1;31m"
#define COLOR_YELLOW "\033[1;33m"

// Function Prototypes
int check_admin_privileges(void);
void display_menu(void);
void execute_quick_scan(void);

// Main Application Loop
int main(void) {
    // Enable ANSI escape sequences in the Windows console
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    printf(COLOR_CYAN "Initializing Automated Windows Diagnostic Tool...\n" COLOR_RESET);
    printf("--------------------------------------------------\n");

    // Verify permissions
    if (!check_admin_privileges()) {
        printf(COLOR_RED "[WARNING] This tool requires Administrator privileges.\n" COLOR_RESET);
        printf("Please close this window, right-click the executable, and select 'Run as administrator'.\n");
        system("pause");
        return 1;
    }
    
    printf(COLOR_GREEN "[SUCCESS] Administrator privileges detected.\n\n" COLOR_RESET);

    // Main Menu Loop
    int choice = 0;
    while (choice != 5) {
        display_menu();
        printf("Enter your choice (1-5): ");
        
        // Basic input handling
        if (scanf("%d", &choice) != 1) {
            // Clear the input buffer if the user enters a letter instead of a number
            while (getchar() != '\n'); 
            choice = 0;
        }

        switch (choice) {
            case 1:
                execute_quick_scan();
                break;
            case 2:
                printf(COLOR_YELLOW "\n[INFO] Deep Repair System (SFC/DISM) - Coming Soon...\n\n" COLOR_RESET);
                break;
            case 3:
                printf(COLOR_YELLOW "\n[INFO] Cache & Temp Clearing - Coming Soon...\n\n" COLOR_RESET);
                break;
            case 4:
                printf(COLOR_YELLOW "\n[INFO] Generate Diagnostic Log - Coming Soon...\n\n" COLOR_RESET);
                break;
            case 5:
                printf(COLOR_CYAN "\nExiting Diagnostic Tool. Goodbye!\n" COLOR_RESET);
                break;
            default:
                printf(COLOR_RED "\n[ERROR] Invalid choice. Please enter a number between 1 and 5.\n\n" COLOR_RESET);
        }
    }

    return 0;
}

// Function to check for Administrator privileges
int check_admin_privileges(void) {
    int is_elevated = 0;
    HANDLE hToken = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION elevation;
        DWORD size;
        if (GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &size)) {
            is_elevated = elevation.TokenIsElevated;
        }
    }
    if (hToken) CloseHandle(hToken);
    return is_elevated;
}

// Function to display the TUI Menu
void display_menu(void) {
    printf(COLOR_CYAN "==================================================\n");
    printf("   SYSFIX - WINDOWS DIAGNOSTIC & REPAIR MENU\n");
    printf("==================================================\n" COLOR_RESET);
    printf(" 1. Run Quick Network Scan (ipconfig)\n");
    printf(" 2. Run Deep System Repair (SFC/DISM)\n");
    printf(" 3. Clear Cache & Temporary Files\n");
    printf(" 4. Generate Diagnostic Log File\n");
    printf(" 5. Exit Program\n");
    printf(COLOR_CYAN "==================================================\n" COLOR_RESET);
}

// Function to execute the Quick Scan (from our earlier test)
void execute_quick_scan(void) {
    char buffer[256];
    printf(COLOR_YELLOW "\nRunning Quick Network Scan...\n" COLOR_RESET);
    printf("-----------------------------------------\n");

    FILE *pipe = _popen("C:\\Windows\\System32\\ipconfig.exe", "r");
    if (pipe == NULL) {
        printf(COLOR_RED "[ERROR] Failed to open the process pipe.\n" COLOR_RESET);
        return;
    }
    
    Sleep(500);

    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        printf("%s", buffer);
    }
    
    _pclose(pipe);
    printf("-----------------------------------------\n");
    printf(COLOR_GREEN "Scan Completed Successfully.\n\n" COLOR_RESET);
}