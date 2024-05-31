/**
 * @file menu.c
 * @brief Implementation of menu functions.
 */
#include "menu.h"


/**
 * @brief Print all images in the directory.
 */ 
void print_dir(void){
    DIR *d;
    struct dirent *dir;
    d = opendir(APP_DIR);
    if (d) {
        int i = 1;
        while ((dir = readdir(d)) != NULL) {
            if (strstr(dir->d_name, ".jpeg") || strstr(dir->d_name, ".jpg")) {
                printf("%d. %s\n", i, dir->d_name);
                i++;
            }
        }
        closedir(d);
    }
}

/**
 * @brief Get the name of a file by its number in the list.
 *
 * @param file_name Buffer to store the file name.
 * @param file_number The number of the file in the list.
 */
void get_file_name(char *file_name, int file_number){
    DIR *d;
    struct dirent *dir;
    d = opendir(APP_DIR);
    if (d) {
        int i = 1;
        while ((dir = readdir(d)) != NULL) {
            if (strstr(dir->d_name, ".jpeg") || strstr(dir->d_name, ".jpg")) {
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

