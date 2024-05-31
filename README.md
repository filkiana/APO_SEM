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
1. **Clone the Repository:**
   ```bash
   git clone https://github.com/filkiana/APO_SEM
   cd APO_SEM
   git checkout dev
   ```
2. **Install LGSL and LJPEG Libraries:**
    ```bash
    chmod +x install.sh
    ./install.sh
    ```
3. **Configure Makefile board ip:**
    ```bash
    vim Makefile
    ```
    Change the IP address in the `BOARD_IP` variable to the IP address of the MicroZed board. 
4. **Build the Project:**
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


## License
MIT License

Copyright (c) 2024 Anatolii Filkin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
