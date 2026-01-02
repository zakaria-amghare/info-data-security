#ifndef ATTACKER_H
#define ATTACKER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>
#include "dataStructure.h"
#include "Function.h"

// Character sets
#define CHARSET_LOWERCASE "abcdefghijklmnopqrstuvwxyz"
#define CHARSET_UPPERCASE "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define CHARSET_DIGITS "0123456789"
#define CHARSET_SPECIAL "!@#$%^&*()-_=+[]{}|\\;:'\",.<>?/`~"

// Statistics structure
typedef struct {
    unsigned long long total_attempts;
    unsigned long long valid_passwords_tested;
    time_t start_time;
    time_t end_time;
    char found_password[128];
    int found;
} crack_stats;

// Check if password meets all requirements
bool meets_requirements(const char *password) {
    int length = strlen(password);
    if (length < 8) return false;
    
    bool has_upper = false, has_lower = false, has_digit = false, has_special = false;
    
    for (int i = 0; i < length; i++) {
        if (isupper(password[i])) has_upper = true;
        else if (islower(password[i])) has_lower = true;
        else if (isdigit(password[i])) has_digit = true;
        else if (strchr(CHARSET_SPECIAL, password[i])) has_special = true;
    }
    
    return has_upper && has_lower && has_digit && has_special;
}

// Generate next valid password using smart incrementing
bool generate_next_valid_password(char *password, int length) {
    if (length < 8) return false;
    
    static bool first_call = true;
    static int indices[128];
    
    if (first_call) {
        password[0] = 'A';
        password[1] = 'a';
        password[2] = '0';
        password[3] = '!';
        
        for (int i = 4; i < length; i++) {
            password[i] = 'a';
        }
        password[length] = '\0';
        
        for (int i = 0; i < length; i++) {
            indices[i] = 0;
        }
        
        first_call = false;
        return true;
    }
    
    int pos = length - 1;
    
    while (pos >= 0) {
        if (pos == 0) {
            if (password[pos] < 'Z') {
                password[pos]++;
                return true;
            } else {
                password[pos] = 'A';
                pos--;
            }
        } else if (pos == 1) {
            if (password[pos] < 'z') {
                password[pos]++;
                return true;
            } else {
                password[pos] = 'a';
                pos--;
            }
        } else if (pos == 2) {
            if (password[pos] < '9') {
                password[pos]++;
                return true;
            } else {
                password[pos] = '0';
                pos--;
            }
        } else if (pos == 3) {
            const char *specials = CHARSET_SPECIAL;
            int special_len = strlen(specials);
            
            char *current_pos = strchr(specials, password[pos]);
            if (current_pos && (current_pos - specials) < special_len - 1) {
                password[pos] = specials[(current_pos - specials) + 1];
                return true;
            } else {
                password[pos] = specials[0];
                pos--;
            }
        } else {
            const char *all_chars = CHARSET_LOWERCASE CHARSET_UPPERCASE CHARSET_DIGITS CHARSET_SPECIAL;
            int all_len = strlen(all_chars);
            
            char *current_pos = strchr(all_chars, password[pos]);
            if (current_pos && (current_pos - all_chars) < all_len - 1) {
                password[pos] = all_chars[(current_pos - all_chars) + 1];
                return true;
            } else {
                password[pos] = all_chars[0];
                pos--;
            }
        }
    }
    
    return false;
}

// Recursive generation with guaranteed valid passwords
void generate_smart_passwords(user_data *user, int length, crack_stats *stats) {
    char password[128];
    
    password[0] = 'A';
    password[1] = 'a';
    password[2] = '0';
    password[3] = '!';
    
    for (int i = 4; i < length; i++) {
        password[i] = 'a';
    }
    password[length] = '\0';
    
    printf("\n>> Starting length %d with pattern: [Upper][lower][digit][special][%d more]\n", 
           length, length - 4);
    printf("   First password: %s\n", password);
    
    static bool reset_generator = true;
    char temp[128];
    strcpy(temp, password);
    generate_next_valid_password(temp, length);
    
    do {
        stats->valid_passwords_tested++;
        
        char full_input[256];
        char computed_hash[65];
        snprintf(full_input, sizeof(full_input), "%s%s", password, user->salte);
        sha256(full_input, computed_hash);
        
        if (stats->valid_passwords_tested % 10000 == 0) {
            printf(".");
            fflush(stdout);
        }
        
        if (stats->valid_passwords_tested % 500000 == 0) {
            time_t current = time(NULL);
            double elapsed = difftime(current, stats->start_time);
            double rate = stats->valid_passwords_tested / (elapsed > 0 ? elapsed : 1);
            printf("\n    [%llu tested | %.0f/sec | Current: %s]", 
                   stats->valid_passwords_tested, rate, password);
            fflush(stdout);
        }
        
        if (strcmp(computed_hash, user->hash) == 0) {
            stats->found = 1;
            strcpy(stats->found_password, password);
            return;
        }
        
    } while (generate_next_valid_password(password, length));
}

// Print statistics
void print_crack_stats(crack_stats *stats, int length_tested) {
    printf("\n\n");
    printf("+========================================================+\n");
    printf("|          SMART BRUTE FORCE STATISTICS                 |\n");
    printf("+========================================================+\n");
    
    if (stats->found) {
        printf("| [+] Status: PASSWORD FOUND!                            |\n");
        printf("| [*] Password: %-41s|\n", stats->found_password);
    } else {
        printf("| [-] Status: Password not found at length %d            |\n", length_tested);
    }
    
    printf("| [*] Valid Passwords Tested: %-27llu|\n", stats->valid_passwords_tested);
    
    double elapsed = difftime(stats->end_time, stats->start_time);
    printf("| [*] Time Elapsed: %.2f seconds                         |\n", elapsed);
    
    if (elapsed > 0) {
        double rate = stats->valid_passwords_tested / elapsed;
        printf("| [*] Speed: %.0f valid passwords/sec                    |\n", rate);
    }
    
    printf("+========================================================+\n");
}

// Main cracking function
void crack_user_password_smart(user_data *user, int min_length, int max_length) {
    printf("\n+========================================================+\n");
    printf("|         SMART BRUTE FORCE ATTACK                       |\n");
    printf("+========================================================+\n");
    printf("| [*] User: %-45s|\n", user->nom);
    printf("| [*] Salt: %-45s|\n", user->salte);
    printf("| [*] Hash: %-45s|\n", user->hash);
    printf("+--------------------------------------------------------+\n");
    printf("| [+] Only testing VALID passwords                       |\n");
    printf("| [*] Requirements: Upper + Lower + Digit + Special      |\n");
    printf("| [*] Length range: %d to %d characters                   |\n", min_length, max_length);
    printf("+========================================================+\n");
    
    crack_stats stats = {0};
    stats.start_time = time(NULL);
    
    for (int len = min_length; len <= max_length; len++) {
        printf("\n========================================================\n");
        printf(">> Testing passwords of length %d\n", len);
        printf("========================================================\n");
        
        generate_smart_passwords(user, len, &stats);
        
        if (stats.found) {
            stats.end_time = time(NULL);
            printf("\n\n*** PASSWORD CRACKED! ***\n");
            print_crack_stats(&stats, len);
            return;
        }
        
        printf("\n[-] Not found at length %d\n", len);
    }
    
    stats.end_time = time(NULL);
    printf("\n\n[-] Password not found in range %d-%d characters\n", min_length, max_length);
    print_crack_stats(&stats, max_length);
}

// Crack all passwords in file
void crack_all_passwords(const char *filename, int min_length, int max_length) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("[-] Error opening file: %s\n", filename);
        return;
    }
    
    file_header header;
    fread(&header, sizeof(file_header), 1, file);
    
    printf("\n+========================================================+\n");
    printf("|     SMART BRUTE FORCE ATTACK INITIATED                 |\n");
    printf("+========================================================+\n");
    printf("| [*] File: %-45s|\n", filename);
    printf("| [*] Total Users: %-38d|\n", header.number_of_users);
    printf("| [*] Mode: Smart (Valid passwords only)                 |\n");
    printf("+========================================================+\n");
    
    for (int i = 0; i < header.number_of_users; i++) {
        user_data user;
        fread(&user, sizeof(user_data), 1, file);
        
        printf("\n\n");
        printf("========================================================\n");
        printf("  USER %d of %d\n", i + 1, header.number_of_users);
        printf("========================================================\n");
        
        crack_user_password_smart(&user, min_length, max_length);
    }
    
    fclose(file);
    printf("\n\n[+] Cracking session completed!\n");
}

#endif // ATTACKER_H