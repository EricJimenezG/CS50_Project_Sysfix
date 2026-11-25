/*  CS50 Final Project: Windows Diagnostic & Repair Tool (Sysfix)
    Created by: Eric Jimenez Garcia
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>

// ANSI Escape Codes for Terminal Colors
#define COLOR_RESET  "\033[0m"
#define COLOR_CYAN   "\033[1;36m"
#define COLOR_GREEN  "\033[1;32m"
#define COLOR_RED    "\033[1;31m"
#define COLOR_YELLOW "\033[1;33m"

// Defyning maximum log lines and line length for log generation
#define MAX_RECENT_LOGS 15
#define LOG_LINE_LEN 512

// Data Struct to track cache and temporary file cleanup metrics
typedef struct
{
    unsigned long files_deleted;
    unsigned long files_skipped;
    unsigned long long bytes_freed;
} CleanupStats;

// Data Struct to hold parsed CBS.log diagnostic metrics and system info
typedef struct
{
    char timestamp[64];
    char computer_name[MAX_COMPUTERNAME_LENGTH + 1];
    unsigned long long uptime_minutes;
    unsigned long total_sr_entries;
    unsigned long repaired_count;
    unsigned long unrepaired_count;
    unsigned long scans_completed;
} DiagnosticReport;

// Function Prototypes
int check_admin_privileges(void);
void display_menu(void);
void execute_quick_Netscan(void);
int run_piped_command(const char *command);
void execute_deep_repair(void);
void clean_directory_contents(const char *dir_path, int remove_self, CleanupStats *stats);
void execute_cache_cleanup(void);
void execute_log_generator(void);

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
                execute_cache_cleanup();
                break;
            case 4:
                execute_log_generator();
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

// Helper function to recursively delete temporary files while safely skipping locked files
// Note: Win32 directory traversal logic developed with assistance from Gemini AI
void clean_directory_contents(const char *dir_path, int remove_self, CleanupStats *stats)
{
    char base_path[MAX_PATH];
    strncpy(base_path, dir_path, MAX_PATH - 1);
    base_path[MAX_PATH - 1] = '\0';

    // Strip trailing backslash if present
    size_t len = strlen(base_path);
    while (len > 0 && (base_path[len - 1] == '\\' || base_path[len - 1] == '/'))
    {
        base_path[len - 1] = '\0';
        len--;
    }

    char search_path[MAX_PATH];
    if (snprintf(search_path, sizeof(search_path), "%s\\*", base_path) >= MAX_PATH)
    {
        return;
    }

    WIN32_FIND_DATAA find_data;
    HANDLE hFind = FindFirstFileA(search_path, &find_data);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        return;
    }

    do
    {
        // Skip current and parent directory entries
        if (strcmp(find_data.cFileName, ".") == 0 || strcmp(find_data.cFileName, "..") == 0)
        {
            continue;
        }

        char full_path[MAX_PATH];
        if (snprintf(full_path, sizeof(full_path), "%s\\%s", base_path, find_data.cFileName) >= MAX_PATH)
        {
            continue;
        }

        if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            // Recursively clean subdirectories and remove them if emptied
            clean_directory_contents(full_path, 1, stats);
        }
        else
        {
            unsigned long long file_size =
                ((unsigned long long)find_data.nFileSizeHigh << 32) | find_data.nFileSizeLow;

            // Remove read-only flag if set so temporary files can be deleted
            if (find_data.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
            {
                SetFileAttributesA(full_path, find_data.dwFileAttributes & ~FILE_ATTRIBUTE_READONLY);
            }

            if (DeleteFileA(full_path))
            {
                stats->files_deleted++;
                stats->bytes_freed += file_size;
            }
            else
            {
                // File is currently locked/in use by Windows or a running program
                stats->files_skipped++;
            }
        }
    } while (FindNextFileA(hFind, &find_data) != 0);

    FindClose(hFind);

    // Attempt to remove empty subdirectories (silently ignored if locked files remain inside)
    if (remove_self)
    {
        RemoveDirectoryA(base_path);
    }
}

// Option 3: Flush DNS Cache and Clear Windows Temporary Files
// Note: Module workflow developed with assistance from Gemini AI
void execute_cache_cleanup(void)
{
    char confirm = 'n';
    printf(COLOR_CYAN "\n--- CACHE & TEMPORARY FILES CLEANUP ---\n" COLOR_RESET);
    printf("This module will perform the following maintenance tasks:\n");
    printf("  1. Flush the Windows DNS Resolver Cache (ipconfig /flushdns)\n");
    printf("  2. Remove stale files in User Temp (%%TEMP%%)\n");
    printf("  3. Remove stale files in System Temp (C:\\Windows\\Temp)\n");
    printf("\nProceed with cleanup? (y/n): ");

    if (scanf(" %c", &confirm) != 1)
    {
        while (getchar() != '\n');
        confirm = 'n';
    }
    else
    {
        while (getchar() != '\n');
    }

    if (confirm != 'y' && confirm != 'Y')
    {
        printf(COLOR_YELLOW "\n[INFO] Cleanup cancelled. Returning to Main Menu.\n\n" COLOR_RESET);
        return;
    }

    // Step 1: Flush DNS Resolver Cache
    printf(COLOR_YELLOW "\n[1/3] Flushing DNS Resolver Cache...\n" COLOR_RESET);
    printf("------------------------------------------------------------\n");
    int dns_status = run_piped_command("C:\\Windows\\System32\\ipconfig.exe /flushdns");
    printf("------------------------------------------------------------\n");
    if (dns_status == 0)
    {
        printf(COLOR_GREEN "[OK] DNS cache flushed successfully.\n\n" COLOR_RESET);
    }
    else
    {
        printf(COLOR_RED "[WARNING] DNS flush returned exit code %d.\n\n" COLOR_RESET, dns_status);
    }

    CleanupStats stats = {0, 0, 0};

    // Step 2: Clean Current User %TEMP% Directory
    char user_temp[MAX_PATH];
    DWORD path_len = GetTempPathA(MAX_PATH, user_temp);
    if (path_len > 0 && path_len < MAX_PATH)
    {
        printf(COLOR_YELLOW "[2/3] Cleaning User Temp folder (%s)...\n" COLOR_RESET, user_temp);
        clean_directory_contents(user_temp, 0, &stats);
        printf(COLOR_GREEN "[OK] User Temp scan complete.\n\n" COLOR_RESET);
    }
    else
    {
        printf(COLOR_RED "[WARNING] Could not resolve User Temp directory path.\n\n" COLOR_RESET);
    }

    // Step 3: Clean System-Wide C:\Windows\Temp Directory
    printf(COLOR_YELLOW "[3/3] Cleaning System Temp folder (C:\\Windows\\Temp)...\n" COLOR_RESET);
    clean_directory_contents("C:\\Windows\\Temp", 0, &stats);
    printf(COLOR_GREEN "[OK] System Temp scan complete.\n\n" COLOR_RESET);

    // Summary Report
    double mb_freed = (double)stats.bytes_freed / (1024.0 * 1024.0);
    printf(COLOR_CYAN "============================================================\n");
    printf("                 CLEANUP SUMMARY REPORT\n");
    printf("============================================================\n" COLOR_RESET);
    printf("  Temporary files removed : %lu\n", stats.files_deleted);
    printf("  In-use files skipped    : %lu\n", stats.files_skipped);
    printf("  Total disk space freed  : %.2f MB\n", mb_freed);
    printf(COLOR_CYAN "============================================================\n\n" COLOR_RESET);
}

// Option 4: Parse C:\Windows\Logs\CBS\CBS.log for [SR] entries and generate a report file
// Note: Log parsing, circular buffer, and report formatting developed with assistance from Gemini AI
void execute_log_generator(void)
{
    const char *cbs_path = "C:\\Windows\\Logs\\CBS\\CBS.log";
    const char *report_filename = "sysfix_diagnostic_report.txt";

    printf(COLOR_CYAN "\n--- GENERATE DIAGNOSTIC LOG REPORT ---\n" COLOR_RESET);
    printf("Scanning %s for System Repair ([SR]) events...\n", cbs_path);

    DiagnosticReport report = {0};

    // 1. Capture current local timestamp
    time_t now = time(NULL);
    struct tm *local_time = localtime(&now);
    if (local_time != NULL)
    {
        strftime(report.timestamp, sizeof(report.timestamp), "%Y-%m-%d %H:%M:%S", local_time);
    }
    else
    {
        strncpy(report.timestamp, "Unknown Timestamp", sizeof(report.timestamp) - 1);
    }

    // 2. Capture Windows machine name and uptime
    DWORD name_size = sizeof(report.computer_name);
    if (!GetComputerNameA(report.computer_name, &name_size))
    {
        strncpy(report.computer_name, "UNKNOWN-PC", sizeof(report.computer_name) - 1);
    }
    report.uptime_minutes = GetTickCount64() / (1000ULL * 60ULL);

    // 3. Allocate a circular buffer on the heap for the most recent [SR] log entries
    char **recent_logs = malloc(MAX_RECENT_LOGS * sizeof(char *));
    if (recent_logs == NULL)
    {
        printf(COLOR_RED "[ERROR] Memory allocation failed for log buffer.\n\n" COLOR_RESET);
        return;
    }
    for (int i = 0; i < MAX_RECENT_LOGS; i++)
    {
        recent_logs[i] = malloc(LOG_LINE_LEN * sizeof(char));
        if (recent_logs[i] == NULL)
        {
            for (int j = 0; j < i; j++)
            {
                free(recent_logs[j]);
            }
            free(recent_logs);
            printf(COLOR_RED "[ERROR] Memory allocation failed for log entry.\n\n" COLOR_RESET);
            return;
        }
    }

    int write_idx = 0;
    int total_stored = 0;

    // 4. Open and parse CBS.log line by line
    FILE *cbs_file = fopen(cbs_path, "r");
    if (cbs_file == NULL)
    {
        printf(COLOR_YELLOW "[WARNING] Could not open %s (file may be missing or locked).\n" COLOR_RESET, cbs_path);
        printf("Generating system status report without CBS history...\n");
    }
    else
    {
        char line[1024];
        while (fgets(line, sizeof(line), cbs_file) != NULL)
        {
            // Filter specifically for System File Checker [SR] tags
            if (strstr(line, "[SR]") != NULL)
            {
                report.total_sr_entries++;

                if (strstr(line, "Repairing") != NULL || strstr(line, "Repaired") != NULL)
                {
                    report.repaired_count++;
                }
                if (strstr(line, "Cannot repair") != NULL)
                {
                    report.unrepaired_count++;
                }
                if (strstr(line, "Verify complete") != NULL)
                {
                    report.scans_completed++;
                }

                // Store entry in circular buffer
                strncpy(recent_logs[write_idx], line, LOG_LINE_LEN - 1);
                recent_logs[write_idx][LOG_LINE_LEN - 1] = '\0';
                write_idx = (write_idx + 1) % MAX_RECENT_LOGS;
                if (total_stored < MAX_RECENT_LOGS)
                {
                    total_stored++;
                }
            }
        }
        fclose(cbs_file);
    }

    // 5. Write formatted diagnostic report to disk
    FILE *out_file = fopen(report_filename, "w");
    if (out_file == NULL)
    {
        printf(COLOR_RED "[ERROR] Failed to create output report file: %s\n\n" COLOR_RESET, report_filename);
        for (int i = 0; i < MAX_RECENT_LOGS; i++)
        {
            free(recent_logs[i]);
        }
        free(recent_logs);
        return;
    }

    fprintf(out_file, "====================================================================\n");
    fprintf(out_file, "               SYSFIX - WINDOWS DIAGNOSTIC REPORT\n");
    fprintf(out_file, "====================================================================\n");
    fprintf(out_file, "Generated On       : %s\n", report.timestamp);
    fprintf(out_file, "Computer Name      : %s\n", report.computer_name);
    fprintf(out_file, "Administrator Mode : %s\n", check_admin_privileges() ? "Enabled (Elevated)" : "Disabled");
    fprintf(out_file, "System Uptime      : %llu hours, %llu minutes\n",
            report.uptime_minutes / 60ULL, report.uptime_minutes % 60ULL);
    fprintf(out_file, "--------------------------------------------------------------------\n");
    fprintf(out_file, "CBS / SYSTEM FILE CHECKER ([SR]) SUMMARY\n");
    fprintf(out_file, "--------------------------------------------------------------------\n");
    fprintf(out_file, "Total [SR] Log Entries   : %lu\n", report.total_sr_entries);
    fprintf(out_file, "Verification Scans Found : %lu\n", report.scans_completed);
    fprintf(out_file, "Corrupt Files Repaired   : %lu\n", report.repaired_count);
    fprintf(out_file, "Unrepaired Corruptions   : %lu\n", report.unrepaired_count);
    fprintf(out_file, "--------------------------------------------------------------------\n");
    fprintf(out_file, "MOST RECENT [SR] LOG ENTRIES (UP TO %d)\n", MAX_RECENT_LOGS);
    fprintf(out_file, "--------------------------------------------------------------------\n");

    if (total_stored == 0)
    {
        fprintf(out_file, "No [SR] entries found in CBS.log.\n");
    }
    else
    {
        int start_idx = (total_stored < MAX_RECENT_LOGS) ? 0 : write_idx;
        for (int i = 0; i < total_stored; i++)
        {
            int idx = (start_idx + i) % MAX_RECENT_LOGS;
            size_t entry_len = strlen(recent_logs[idx]);
            fprintf(out_file, "%s", recent_logs[idx]);
            if (entry_len == 0 || recent_logs[idx][entry_len - 1] != '\n')
            {
                fprintf(out_file, "\n");
            }
        }
    }

    fprintf(out_file, "====================================================================\n");
    fclose(out_file);

    // 6. Free dynamically allocated circular buffer
    for (int i = 0; i < MAX_RECENT_LOGS; i++)
    {
        free(recent_logs[i]);
    }
    free(recent_logs);

    // Display summary in console
    printf(COLOR_GREEN "\n[SUCCESS] Diagnostic report saved to: %s\n" COLOR_RESET, report_filename);
    printf("------------------------------------------------------------\n");
    printf("  Computer Name          : %s\n", report.computer_name);
    printf("  System Uptime          : %lluh %llum\n", report.uptime_minutes / 60ULL, report.uptime_minutes % 60ULL);
    printf("  Total [SR] Entries     : %lu\n", report.total_sr_entries);
    printf("  Files Repaired         : %lu\n", report.repaired_count);
    printf("  Unrepaired Corruptions : %lu\n", report.unrepaired_count);
    printf("------------------------------------------------------------\n\n");
}