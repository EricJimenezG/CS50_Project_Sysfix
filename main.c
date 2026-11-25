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
void execute_quick_Netscan(void);
int run_piped_command(const char *command);
void execute_deep_repair(void);

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
                execute_quick_Netscan();
                break;
            case 2:
                execute_deep_repair();
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
void execute_quick_Netscan(void) {
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

// Helper function to execute a system command via process pipe and stream output
// Note: Byte-level filtering logic developed with assistance from Gemini AI
int run_piped_command(const char *command)
{
    FILE *pipe = _popen(command, "r");
    if (pipe == NULL)
    {
        printf(COLOR_RED "[ERROR] Failed to open process pipe for: %s\n" COLOR_RESET, command);
        return -1;
    }

    char raw_buffer[512];
    size_t bytes_read;

    // Read raw bytes so UTF-16LE output (such as from sfc.exe) is not truncated at '\0'
    while ((bytes_read = fread(raw_buffer, 1, sizeof(raw_buffer), pipe)) > 0)
    {
        for (size_t i = 0; i < bytes_read; i++)
        {
            unsigned char ch = (unsigned char)raw_buffer[i];
            if (ch != '\0' && ch != 0xFF && ch != 0xFE)
            {
                putchar(ch);
            }
        }
        fflush(stdout);
    }

    return _pclose(pipe);
}

// Option 2: Execute Deep System Repair (DISM + SFC)
// Note: Module structure and demo simulation mode developed with assistance from Gemini AI
void execute_deep_repair(void)
{
    int mode = 0;
    printf(COLOR_CYAN "\n--- DEEP SYSTEM REPAIR (DISM & SFC) ---\n" COLOR_RESET);
    printf("This module repairs the Windows component image and scans protected system files.\n");
    printf(" 1. Run Live System Repair (DISM + SFC - takes 5 to 15 minutes)\n");
    printf(" 2. Run Fast Demo Simulation (for quick testing / CS50 video demo)\n");
    printf(" 3. Cancel and return to Main Menu\n");
    printf("Select repair mode (1-3): ");

    if (scanf("%d", &mode) != 1)
    {
        while (getchar() != '\n');
        mode = 0;
    }

    if (mode == 3)
    {
        printf(COLOR_YELLOW "\n[INFO] Deep System Repair cancelled.\n\n" COLOR_RESET);
        return;
    }
    else if (mode == 2)
    {
        // Fast simulated run for testing and video demonstration
        printf(COLOR_YELLOW "\n[DEMO MODE] Simulating Deep System Repair sequence...\n" COLOR_RESET);
        printf("------------------------------------------------------------\n");

        printf(COLOR_CYAN "[Stage 1/2] Running DISM Image RestoreHealth...\n" COLOR_RESET);
        for (int p = 20; p <= 100; p += 20)
        {
            printf("  DISM Progress: %d%%\n", p);
            fflush(stdout);
            Sleep(350);
        }
        printf(COLOR_GREEN "  [OK] Component store corruption check completed.\n\n" COLOR_RESET);

        printf(COLOR_CYAN "[Stage 2/2] Running System File Checker (sfc /scannow)...\n" COLOR_RESET);
        for (int p = 25; p <= 100; p += 25)
        {
            printf("  Verification %d%% complete.\n", p);
            fflush(stdout);
            Sleep(350);
        }
        printf(COLOR_GREEN "  [OK] Windows Resource Protection completed verification.\n" COLOR_RESET);
        printf("------------------------------------------------------------\n");
        printf(COLOR_GREEN "[SUCCESS] Simulated Deep System Repair finished.\n\n" COLOR_RESET);
        return;
    }
    else if (mode != 1)
    {
        printf(COLOR_RED "\n[ERROR] Invalid selection. Returning to Main Menu.\n\n" COLOR_RESET);
        return;
    }

    // Live execution using full System32 paths
    printf(COLOR_YELLOW "\n[STAGE 1/2] Running DISM Image Repair (Please do not close this window)...\n" COLOR_RESET);
    printf("------------------------------------------------------------\n");
    int dism_status = run_piped_command("C:\\Windows\\System32\\Dism.exe /Online /Cleanup-Image /RestoreHealth");
    printf("------------------------------------------------------------\n");

    if (dism_status == 0)
    {
        printf(COLOR_GREEN "[OK] DISM completed with exit code 0.\n\n" COLOR_RESET);
    }
    else
    {
        printf(COLOR_YELLOW "[WARNING] DISM exited with code %d. Proceeding to SFC scan...\n\n" COLOR_RESET, dism_status);
    }

    printf(COLOR_YELLOW "[STAGE 2/2] Running System File Checker (sfc /scannow)...\n" COLOR_RESET);
    printf("------------------------------------------------------------\n");
    int sfc_status = run_piped_command("C:\\Windows\\System32\\sfc.exe /scannow");
    printf("------------------------------------------------------------\n");

    if (dism_status == 0 && sfc_status == 0)
    {
        printf(COLOR_GREEN "[SUCCESS] Deep System Repair completed cleanly.\n\n" COLOR_RESET);
    }
    else
    {
        printf(COLOR_YELLOW "[INFO] Repair finished (DISM code: %d, SFC code: %d). Check Option 4 logs for details.\n\n" COLOR_RESET, dism_status, sfc_status);
    }
}