#include<stdio.h>
#include<stdlib.h>
#include"Function.h"
#include"dataStructure.h"
#include<string.h>
#include<time.h>


int main()
{
    user_data u;
    strcpy(u.nom, "rabah");
    time_t now = time(NULL);
    sprintf(u.salte, "%ld", now % 1000000000000000);
    char plain_text[128];
    snprintf(plain_text, sizeof(plain_text), "password%s", u.salte);

    // Pass the safe buffer to the hashing function
    sha256(plain_text, u.hash);

    create_file_if_not_exists("file");
    add_user("file", &u);
    read_file("file");
    
    return 0;
   
}