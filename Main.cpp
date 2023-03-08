/**
 * Class to make bad apple into ascii and print to discord. This is probably one of my worst ideas ever. For future use, please just use the OpenCV video
 * processing library. It makes this way easier lol.
 * 
 * Created by Dax McPartland, 3/7/2023
*/
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <string>
#include <fstream>
#include "Image.h"
#include <Windows.h>

std::string concatenateStrings(const std::vector<std::string>& lines) {
    std::string result;
    for (const std::string& line : lines) {
        result += "\n" + line;
    }
    return result;
}


void pasteAndEnter() {
     
    //Simulate a ` key press and release
    INPUT singleQuote[2];
    ZeroMemory(singleQuote, sizeof(singleQuote));
    singleQuote[0].type = INPUT_KEYBOARD;
    singleQuote[0].ki.wVk = VK_OEM_3;
    singleQuote[1].type = INPUT_KEYBOARD;
    singleQuote[1].ki.dwFlags = KEYEVENTF_KEYUP;
    singleQuote[1].ki.wVk = VK_OEM_3;

    // Simulate an Enter key press and release
    INPUT enter[2];
    ZeroMemory(enter, sizeof(enter));
    enter[0].type = INPUT_KEYBOARD;
    enter[0].ki.wVk = VK_RETURN;
    enter[1].type = INPUT_KEYBOARD;
    enter[1].ki.dwFlags = KEYEVENTF_KEYUP;
    enter[1].ki.wVk = VK_RETURN;
    

    // Simulate a Ctrl+V key press and release
    INPUT ctrlV[4];
    ZeroMemory(ctrlV, sizeof(ctrlV));
    ctrlV[0].type = INPUT_KEYBOARD;
    ctrlV[0].ki.wVk = VK_CONTROL;
    ctrlV[1].type = INPUT_KEYBOARD;
    ctrlV[1].ki.wVk = 'V';
    ctrlV[2].type = INPUT_KEYBOARD;
    ctrlV[2].ki.dwFlags = KEYEVENTF_KEYUP;
    ctrlV[2].ki.wVk = 'V';
    ctrlV[3].type = INPUT_KEYBOARD;
    ctrlV[3].ki.dwFlags = KEYEVENTF_KEYUP;
    ctrlV[3].ki.wVk = VK_CONTROL;

    // Simulate a ` key press and release
    SendInput(2, singleQuote, sizeof(INPUT));
    Sleep(25);
    // Simulate a CLT + V key press and release
    SendInput(4, ctrlV, sizeof(INPUT));
    Sleep(25);

    // Simulate a ` key press and release
    SendInput(2, singleQuote, sizeof(INPUT));
    Sleep(25);

    // Simulate an Enter key press and release
    SendInput(2, enter, sizeof(INPUT));
    Sleep(25);
}

void copyImageToClipboard(const std::vector<std::string>& lines) {
    // Concatenate the strings into a single string
    std::string imageData = concatenateStrings(lines);

    // Open the clipboard
    if (!OpenClipboard(NULL)) {
        return;
    }

    // Clear the clipboard
    EmptyClipboard();

    // Allocate memory for the image data
    HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE, imageData.size() + 1);
    if (hData == NULL) {
        CloseClipboard();
        return;
    }

    // Copy the image data into the allocated memory
    char* pMemory = (char*)GlobalLock(hData);
    if (pMemory == NULL) {
        GlobalFree(hData);
        CloseClipboard();
        return;
    }

    memcpy(pMemory, imageData.c_str(), imageData.size() + 1);
    GlobalUnlock(hData);

    // Set the image data to the clipboard
    SetClipboardData(CF_TEXT, hData);

    // Close the clipboard
    CloseClipboard();
}

void convertImageAndDisplay(Image img){ 
    const std::string densityMap = " .:-=+*#%@"; // Create density string for the ascii art
    std::vector<std::string> imgRows = img.convertToAscii(densityMap);
    copyImageToClipboard(imgRows); // Copy the ascii art
    Sleep(50);
    pasteAndEnter(); // Paste the ascii art
    Sleep(100);
}


int main() {
    const int frameBounds = 30; // Number of frames to capture per video segment
    int counter = 0; // Counter to keep track of video segments
    int numOfFrames = 151; // Total number of frames in each video segement (other than last one)
    
    while(frameBounds * counter < 240) { // Loop through all video segments until we reach the end of the video (240 stands for the amount of seconds)
        std::string currFolder = "badapple_"; // Name of the current video segment folder
        
        // Append the start and end frame numbers to the folder name
        currFolder.append(std::to_string(frameBounds * counter));
        counter++;
        currFolder.append("-");
        currFolder.append(std::to_string(frameBounds * counter));

        int frameCounter = 1; // Counter to keep track of frames within the current video segment
        
        // If this is the last video segment, update the number of frames to capture because last folder only has 48 frames
        if(frameBounds * counter == 240) {
            numOfFrames = 48;
        }
        
        while(frameCounter < numOfFrames) { // Loop through all frames in the current video segment
            std::string filePath = "./badappleframes/" + currFolder + "/"; // Path to the current frame image file
            std::string frame = "ezgif-frame-";
            
            // Pad the frame number with leading zeros because png's have leading 0's
            std::ostringstream oss;
            oss << std::setw(3) << std::setfill('0') << frameCounter;
            std::string result = oss.str();
            frame.append(result);
            frame.append(".png");
            filePath.append(frame);
            
            const char* imgName = const_cast<char*>(filePath.c_str()); // Convert the file path to a C-style string
            Image img(imgName); // Load the image from the file
            convertImageAndDisplay(img); // convert file to ascii and display it on the screen
            Sleep(2000); // Wait for 2 seconds before showing the next frame because discord tick speed
            frameCounter++;
        }
    }
    return 0;
}