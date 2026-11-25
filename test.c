#include <stdio.h>
#include <stdlib.h>
#include <windows.h> 

int main(void) {
    char buffer[256];
    
    printf("Starting system capture test...\n");
    printf("-----------------------------------------\n");

    // Use the absolute path to ensure the system finds the executable
    FILE *pipe = _popen("C:\\Windows\\System32\\ipconfig.exe", "r");
    
    if (pipe == NULL) {
        printf("Error: Failed to open the process pipe.\n");
        return 1;
    }

    // Small delay to ensure the pipe stream is established
    Sleep(500);

    // Read the standard output of the command line by line using fgets
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        printf("%s", buffer);
    }

    // Close the pipe
    int returnCode = _pclose(pipe);
    
    printf("-----------------------------------------\n");
    printf("Test completed with return code: %d\n", returnCode);
    
    return 0;
}