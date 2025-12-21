#include <string.h>
#include <stdio.h>
#include "dataStructure.h"
#include <openssl/sha.h>

void create_file_if_not_exists(const char *filename) {
    FILE *file = fopen(filename, "r");

    if (file != NULL) {
        // File exists
        printf("File already exists: %s\n", filename);
        fclose(file);
    } else {
        // File does not exist → create it
        file = fopen(filename, "w");
        if (file != NULL) {
            printf("File created: %s\n", filename);
            file_header header = {0, sizeof(file_header)};
            fwrite(&header, sizeof(file_header), 1, file);
            fclose(file);
        } else {
            printf("Error creating file: %s\n", filename);
        }
    }
}

void add_user(const char *filename, user_data *user) {
    FILE *file = fopen(filename, "r+b");
    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        return;
    }

    file_header header;
    fread(&header, sizeof(file_header), 1, file);

    for (int i = 0; i < header.number_of_users; i++)
    {
        user_data temp;
        fread(&temp, sizeof(user_data), 1, file);
        printf("Comparing %s with %s\n", temp.nom, user->nom);
        if (strcmp(temp.nom, user->nom) == 0)
        {
            printf("User with name %s already exists.\n", user->nom);
            fclose(file);
            return;
        }
        
    }
    
    fwrite(user, sizeof(user_data), 1, file);

    header.number_of_users += 1;
    header.size_of_file += sizeof(user_data);

    fseek(file, 0, SEEK_SET);
    fwrite(&header, sizeof(file_header), 1, file);

    fclose(file);
}

void read_file(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        return;
    }

    file_header header;
    fread(&header, sizeof(file_header), 1, file);
    printf("Number of users: %d\n", header.number_of_users);
    printf("Size of file: %d bytes\n", header.size_of_file);
    for (int i = 0; i < 50; i++)
    {
        printf("-");
    }
    printf("\n");

    user_data user;
    for (int i = 0; i < header.number_of_users; i++) {
        fread(&user, sizeof(user_data), 1, file);
        printf("User %d:\n", i + 1);
        printf("  Name: %s\n", user.nom);
        printf("  Salt: %s\n", user.salte);
        printf("  Hash: %s\n", user.hash);
    }

    fclose(file);
}

#include <openssl/evp.h> // Change the header to include EVP

void sha256(const char *string, char outputBuffer[65])
{
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len;
    
    // Create and initialize a message digest context
    EVP_MD_CTX *context = EVP_MD_CTX_new();
    
    if (context != NULL) {
        // Initialize the context with SHA256 algorithm
        if (EVP_DigestInit_ex(context, EVP_sha256(), NULL)) {
            // Provide the data to hash
            EVP_DigestUpdate(context, string, strlen(string));
            // Finalize the hash
            EVP_DigestFinal_ex(context, hash, &hash_len);
        }
        // Clean up the context
        EVP_MD_CTX_free(context);
    }
    
    // Convert bytes to hex string
    for(unsigned int i = 0; i < hash_len; i++)
    {
        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
    }
    outputBuffer[64] = '\0';
}