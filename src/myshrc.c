#include "myshrc.h"

void mk_hop(){
    if (mkdir(list[1], 0755) == -1) {
        perror("mkdir failed");
        return;
    }
    chdir(list[1]);
}

void openMySH(){
    mk_hop_flag=0;
    hop_seek_flag=0;
    snprintf(shrcPath, sizeof(shrcPath), "%s/.myshrc",homeDir);
    FILE* myshfile=fopen(shrcPath, "r");
    if(myshfile == NULL){
        perror("Failed to open .myshrc");
        return;
    }

    char myshline[MAX];
    // for(int i=0;i<size;i++){
    //     printf("%s\n", list[i]);
    // }
    // printf("done............\n");
    while(fgets(myshline, sizeof(myshline),myshfile)!=NULL){
        if(strncmp((myshline), "alias",5)==0){
            //found alias
            char* shtoken = strtok(myshline + 6, " = ");
            char *alias_command = strtok(NULL, "\n");
            alias_command=alias_command+2; //read from after "= "
            // printf("\nshtoken: %s\n", shtoken);
            // printf("alias_command: %s\n", alias_command);
            //alias home = cd .. . .. ~
            if(strcmp(shtoken, list[0])==0){
                char *spacenew = strtok(alias_command, " ");
                for(int i=1; i<size; i++){
                    strcpy(list[i-1],list[i]);
                }
                // printf("size: %d\n", size);
                size--;
                for(int i=0;i<size;i++){
                    strcpy(list[i+1], list[i]);
                }
                int alias_command_size=0;
                while(spacenew != NULL){
                    // printf("%s\n", spacenew);
                    size++;
                    strcpy(list[alias_command_size++],spacenew);
                    for(int i=alias_command_size;i<size;i++){
                        strcpy(list[i+1], list[i]);
                    }
                    spacenew = strtok(NULL, " ");
                }
            }
        }
        if(strncmp((myshline), "mk_hop",6)==0){
            mk_hop_flag=1;
        }
        // printf("%s", myshline);
        
    }
    // printf("again..........\n");
    // for(int i=0; i<size; i++){
    //     printf("%s ", list[i]);
    // }
    // printf("\ndone...........\n");
    fclose(myshfile);
}