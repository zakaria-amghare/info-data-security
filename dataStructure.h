#include<stdio.h>
#include<stdlib.h>
typedef struct file_header
{
int number_of_users;
int size_of_file;
}file_header;
typedef struct user_data
{
char nom[50];
char salte[17];
char hash[65];
}user_data;
