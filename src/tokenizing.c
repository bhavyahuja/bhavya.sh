#include "header.h"
#include "isValid.h"
#include "piping.h"
#include "redirection.h"

void tokenise_space(char *str){
    char copy[MAX];
    strcpy(copy, str);
    char *ptr;
    char *token = strtok_r(copy, " ", &ptr);
    int i = 0;
    while (token != NULL){
        strcpy(andList[i], token);
        i++;
        token = strtok_r(NULL, " ", &ptr);
    }
    andList[i][0] = '\0';
    int j = 0;
    real_size = 0;
    while(j<i){
        while(strcmp(andList[j], "&") != 0 && j<i){
            strcpy(list[real_size++], andList[j]);
            j++;
        }
        if(strcmp(andList[j], "&") == 0){
            real_size++;
            // printf("size: %d\n", size);
            strcpy(list[real_size-1], "&");
        }
        // for(int k=0;k<size;k++){
        //     printf("%s ",list[k]);
        // }
        printf("\n");
        list[j+1][0]='\0';
        handle_piping();
        real_size = 0;
        j++;
    }
}

void semicolonTokenise(){
    char command_copy[MAX];
    strcpy(command_copy, command);
    char *ptr;
    char *token = strtok_r(command_copy, ";", &ptr);
    while (token != NULL){
        while (*token == ' '){
            token++;
        }
        // printf("%s\n",token);
        tokenise_space(token);
        token = strtok_r(NULL, ";", &ptr);
    }
}
