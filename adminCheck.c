#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

// Function to check for Administrator privileges
int check_admin_privileges(void) {
    int is_elevated = 0;
    HANDLE hToken = NULL;

    // Open the access token associated with the current process
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION elevation;
        DWORD size;

        // Retrieve the token information to check the elevation flag
        if (GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &size)) {
            is_elevated = elevation.TokenIsElevated;
        }
    }
    
    // Clean up the handle to prevent memory leaks
    if (hToken) {
        CloseHandle(hToken);
    }

    return is_elevated;
}

// Main function to initialize the diagnostic tool for admin privileges
int main(void) {
    printf("Initializing Automated Windows Diagnostic Tool...\n");
    printf("--------------------------------------------------\n");

    // Verify permissions
    if (check_admin_privileges()) {
        printf("[SUCCESS] Administrator privileges detected.\n");
        // Future code: Launch the main CLI menu here
    } else {
        printf("[WARNING] This tool requires Administrator privileges.\n");
        printf("Please close this window, right-click the executable, and select 'Run as administrator'.\n");
        
        // Pause so the user can read the error before the terminal closes
        system("pause");
        return 1;
    }

    return 0;
}