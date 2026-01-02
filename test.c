#include <stdio.h>
#include <stdlib.h>
#include "dataStructure.h"
#include "attacker.h"

int main() {
    // Create a user with a simple valid password
    user_data u = create_user("zaki the great", "Aaaaaaaa1!");
    
    create_file_if_not_exists("target.data");
    add_user("target.data", &u);
    
    printf("User created with password: Ba0!bcde\n");
    printf("Now attempting to crack...\n\n");
    
    // Try to crack it (will find it quickly since it starts with Aa0!)
    crack_all_passwords("target.data", 8, 10);
    
    return 0;
}