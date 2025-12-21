#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dataStructure.h"
#include "Function.h"

// Helper to try all combinations of a specific length
int brute_force_recursive(user_data *user, char *attempt, int position, int length, int *global_counter) {
    if (position == length) {
        attempt[length] = '\0'; // Null terminate the guess
        
        char full_input[128];
        char computed_hash[65];

        // Concatenate guess + salt
        snprintf(full_input, sizeof(full_input), "%s%s", attempt, user->salte);
        sha256(full_input, computed_hash);

        // Progress Tracking
        (*global_counter)++;
        if (*global_counter % 10000 == 0) {
            printf("*");
            fflush(stdout);
        }

        // Check if match found
        if (strcmp(computed_hash, user->hash) == 0) {
            printf("\n    >> SUCCESS! Password for %s is: %s\n", user->nom, attempt);
            return 1;
        }
        return 0;
    }

    // Try 'a' through 'z' for the current character position
    for (char c = 'a'; c <= 'z'; c++) {
        attempt[position] = c;
        if (brute_force_recursive(user, attempt, position + 1, length, global_counter)) {
            return 1;
        }
    }
    return 0;
}

void crack_all_passwords(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) return;

    file_header header;
    fread(&header, sizeof(file_header), 1, file);

    for (int i = 0; i < header.number_of_users; i++) {
        user_data user;
        fread(&user, sizeof(user_data), 1, file);
        printf("\nCracking %s...", user.nom);

        int global_counter = 0;
        int found = 0;
        char attempt[10]; // Max password length to try

        // Try lengths 1 to 8 (e.g., a, ab, abc...)
        for (int len = 1; len <= 8; len++) {
            if (brute_force_recursive(&user, attempt, 0, len, &global_counter)) {
                found = 1;
                break;
            }
        }
        if (!found) printf("\n    >> Password not found.");
    }
    fclose(file);
}