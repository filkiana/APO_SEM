#ifndef MENU_H
#define MENU_H

#include "lcd.h"
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#define APP_DIR "/tmp/filkiana/"
void show_menu(void);

void print_dir(void);

void get_file_name(char *file_name, int file_number);

#endif // MENU_H