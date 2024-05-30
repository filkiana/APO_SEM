# README

## Introduction

### Scanning Documents from Photographs Using C and MicroZed Camera
**Author:** Anatolii Filkin  
**Date:** 20.04.2024

**Project Description:**
This application is designed to scan documents by capturing a photograph of a textual document and processing it to produce a rectangular output image. It assumes that the input photograph contains an A4 paper. The photograph can be taken using SSH. The user selects four corner points of the document, and the application returns the scanned image. The LED diode displays the intencity of the currently selected pixel.

## Dependencies

### LGSL
- **Description:** LGSL (GNU Scientific Library) is a numerical library for C language. It provides a wide range of mathematical routines such as random number generators, special functions, and least-squares fitting.
- **License:** GNU General Public License (GPL)

### LJPEG
- **Description:** LJPEG is a JPEG image compression library that allows for reading and writing JPEG images. It is used in this project for handling image input and output.
- **License:** Independent JPEG Group's (IJG) JPEG library license


## Project Links

- **Git Project Link:**https://github.com/filkiana/APO_SEM

## Installation Manual
0. **Download and build the dependencies:**
   * https://www.gnu.org/software/gsl/
   * https://www.ijg.org/
    1. For LGSL: gsl.tar.gz
    ```bash
        tar -xvzf gsl.tar.gz
        cd gsl
        ./configure "CC=arm-linux-gnueabihf-gcc" 
        make
    ```
    2. For LJPEG: jpegsrc.v9d.tar.gz
    ```bash
        tar -xvzf jpegsrc.v9d.tar.gz
        cd jpeg-9d
        ./configure "CC=arm-linux-gnueabihf-gcc" --prefix={DIR_WHERE_YOU_HAVE_THIS_LIBRARY}
        make
    ```
1. **Clone the Repository:**
   ```bash
   git clone https://github.com/filkiana/APO_SEM
   cd APO_SEM
   ```
2. **Configure Makefile with the paths to the dependencies and board ip:**
    ```
    using your favorite text editor. 
    ```
3. **Build the Project:**
   ```bash
   make
   ``` 

## User Manual

### Running the Application
1. **Build and Run the Application on the board using make:**
   ```bash
   make run
    ```
2. **Chose file for scanning using keyboard:**
3. **Select four corner points of the document using the knobs:**
    ```
    to mark the corners of the document, use the knobs(red for y and blue for x) to move the red-cross cursor and press the green knob to mark the corner.
    1. Top left corner
    2. Top right corner
    3. Bottom right corner
    4. Bottom left corner
    ```
4. **Get the scanned image:**
    ```
    The scanned image will be saved in the same directory as application.
    ```


## Conclusion

This project demonstrates the integration of multiple hardware components (LCD display, knobs,RGB DIODA, SSH) and software libraries (LGSL, LJPEG) to create a functional document scanning application on the MicroZed platform. By leveraging user input and real-time image processing, the application provides a practical solution for capturing and processing document images.User gets commands on the display by font texts. The LED feedback system enhances user interaction, making the application intuitive and efficient. 
Whole project written in C language and have nice modular structure. The code is well-documented and follows the coding style.The project is version-controlled using Git, with meaningful commit messages and a separate development branch. 
