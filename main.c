#include <stdio.h>
#include <stdlib.h>

int main(void) {
    char buffer[256];
    
    printf("Starting system capture test...\n");
    printf("-----------------------------------------\n");

    // Open a process pipe to execute 'ipconfig' in read ("r") mode
    FILE *pipe = _popen("ipconfig", "r");
    
    if (pipe == NULL) {
        printf("Error: Failed to open the process pipe.\n");
        return 1;
    }

    // Read the standard output of the command line by line using fgets
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        printf("%s", buffer);
    }

    // Close the pipe with _pclose
    _pclose(pipe);
    
    printf("-----------------------------------------\n");
    printf("Test completed successfully.\n");
    
    return 0;
}