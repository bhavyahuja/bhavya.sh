#include "log.h"
#include "header.h"
#include "tokenizing.h"

int l = 0;

void addLog()
{
    FILE *logFile;
    char lines[15][MAX];
    lineCount = 0;
    char buffer[MAX];
    char temp[MAX];

    strcpy(temp, input);

    char *token = strtok(temp, " ");
    while (token != NULL)
    {
        if (strcmp(token, "log") == 0)
        {
            return;
        }
        token = strtok(NULL, " ");
    }

    // open in read mode
    logFile = fopen(logPath, "r");

    if (logFile != NULL)
    {
        // read the file line by line into memory
        while (fgets(buffer, sizeof(buffer), logFile) != NULL && lineCount < MAX)
        {
            buffer[strcspn(buffer, "\n")] = '\0'; // remove newline
            strncpy(lines[lineCount], buffer, MAX);
            lineCount++;
        }
        fclose(logFile);
    }

    // dont add the same command again
    for (int i = 0; i < lineCount; i++)
    {
        if (strcmp(lines[i], input) == 0)
        {
            return; // Do not add duplicate command
        }
    }

    // open in write mode
    if (lineCount < 15)
    {
        strncpy(lines[lineCount], input, MAX);
        lineCount++;
    }
    else
    {
        for (int i = 1; i < 15; i++)
        {
            strncpy(lines[i - 1], lines[i], MAX);
        }
        strncpy(lines[14], input, MAX);
    }
    // for(int i=0;i<lineCount;i++){
    //     printf("%s\n", lines[i]);
    // }

    logFile = fopen(logPath, "w");
    if (logFile == NULL)
    {
        perror("fopen");
        return;
    }
    for (int i = 0; i < lineCount; i++)
    {
        fputs(lines[i], logFile);
        fputs("\n", logFile);
    }
    fclose(logFile);

    l = lineCount;
}

void displayLog()
{
    if (l > 0)
    {
        FILE *logFile;
        char buffer[MAX];
        logFile = fopen(logPath, "r");
        if (logFile == NULL)
        {
            perror("fopen");
            return;
        }
        while (fgets(buffer, sizeof(buffer), logFile) != NULL)
        {
            printf("%s", buffer);
        }
        fclose(logFile);
    }
}

void purgeLog()
{
    FILE *logFile;
    logFile = fopen(logPath, "w");
    if (logFile == NULL)
    {
        perror("fopen");
        return;
    }
    fclose(logFile);
    printf("Log file purged successfully\n");
}

void executeLog(int n)
{
    // printf("invalid line %d\n", l);
    int executing = l - n + 1;
    int c = 0;
    FILE *logFile;
    char buffer[MAX];
    logFile = fopen(logPath, "r");
    if (logFile == NULL)
    {
        perror("fopen");
        return;
    }
    while (fgets(buffer, sizeof(buffer), logFile) != NULL && c < executing)
    {
        c++;
        buffer[strcspn(buffer, "\n")] = 0; // remove newline
        if (c == executing)
        {
            // printf("Executing: %s\n", buffer);
            strcpy(command, buffer);
            // printf("input: %s\n", buffer);
            semicolonTokenise();
            break;
        }
    }
    fclose(logFile);
}
