#include "menu.h"


// read all images(*,jpg or *.bmp) in dir and print it as numbered list. 
void print_dir(void){
    DIR *d;
    struct dirent *dir;
    d = opendir(APP_DIR);
    if (d) {
        int i = 1;
        while ((dir = readdir(d)) != NULL) {
            if (strstr(dir->d_name, ".bmp") || strstr(dir->d_name, ".jpg")) {
                printf("%d. %s\n", i, dir->d_name);
                i++;
            }
        }
        closedir(d);
    }
}

void get_file_name(char *file_name, int file_number){
    DIR *d;
    struct dirent *dir;
    d = opendir(APP_DIR);
    if (d) {
        int i = 1;
        while ((dir = readdir(d)) != NULL) {
            if (strstr(dir->d_name, ".bmp") || strstr(dir->d_name, ".jpg")) {
                if (i == file_number) {
                    strncpy(file_name, dir->d_name, 31);
                    file_name[31] = '\0'; // Ensure null-termination
                    break;
                }
                i++;
            }
        }
        closedir(d);
    } else {
        fprintf(stderr, "Error: could not open directory %s\n", APP_DIR);
        exit(1);
    }
}

