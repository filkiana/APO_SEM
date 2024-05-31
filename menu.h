/**
 * @file menu.h
 * @brief Header file for menu functions.
 */

#ifndef MENU_H
#define MENU_H

#include "lcd.h"
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#define APP_DIR "/tmp/filkiana/"

/**
 * @brief Display the menu.
 */
void show_menu(void);

/**
 * @brief Print all images in the directory.
 */
void print_dir(void);

/**
 * @brief Get the name of a file by its number in the list.
 *
 * @param file_name Buffer to store the file name.
 * @param file_number The number of the file in the list.
 */
void get_file_name(char *file_name, int file_number);

#endif // MENU_H
