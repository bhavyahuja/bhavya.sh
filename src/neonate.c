#include "neonate.h"

// Function to get the most recent PID
pid_t fetch_latest_pid() {
    DIR *proc_directory;
    struct dirent *directory_entry;
    pid_t latest_pid = 0;
    time_t latest_time = 0;

    proc_directory = opendir("/proc");
    if (!proc_directory) {
        perror("Unable to open /proc");
        return -1;
    }

    while ((directory_entry = readdir(proc_directory)) != NULL) {
        if (isdigit(directory_entry->d_name[0])) {
            pid_t current_pid = atoi(directory_entry->d_name);
            char stat_filepath[256];
            struct stat file_stat;

            snprintf(stat_filepath, sizeof(stat_filepath), "/proc/%d", current_pid);
            if (stat(stat_filepath, &file_stat) == 0) {
                if (file_stat.st_ctime > latest_time) {
                    latest_time = file_stat.st_ctime;
                    latest_pid = current_pid;
                }
            }
        }
    }

    closedir(proc_directory);
    return latest_pid;
}

// Function to check if a key has been pressed
int check_key_press() {
    struct termios original_settings, modified_settings;
    int character;
    int flags;

    tcgetattr(STDIN_FILENO, &original_settings);
    modified_settings = original_settings;
    modified_settings.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &modified_settings);
    flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    character = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &original_settings);
    fcntl(STDIN_FILENO, F_SETFL, flags);

    if (character != EOF) {
        ungetc(character, stdin);
        return 1;
    }

    return 0;
}

// Main function to run periodically and check for key presses
void neonate(int interval_seconds) {
    pid_t latest_pid;
    char key_input;

    while (1) {
        latest_pid = fetch_latest_pid();
        if (latest_pid != -1) {
            printf("%d\n", latest_pid);
        } else {
            printf("Failed to retrieve the latest PID\n");
        }

        // Sleep and check for key presses
        for (int i = 0; i < interval_seconds * 10; i++) {
            usleep(100000);  // Sleep for 0.1 seconds
            if (check_key_press()) {
                key_input = getchar();
                if (key_input == 'x') {
                    return;
                }
            }
        }
    }
}
