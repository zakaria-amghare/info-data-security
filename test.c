#include<stdio.h>
#include<stdlib.h>
#include"dataStructure.h"
#include<string.h>

#include"attacker.h"

int main()
{
    user_data u;
    u = create_user("zakaria3","abcde");
    
    
    create_file_if_not_exists("file.data");
    add_user("file.data", &u);
    read_file("file.data");
     
    return 0;
   
}