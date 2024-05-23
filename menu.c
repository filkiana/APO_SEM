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

