#include "saves.h"
#include <fstream>
#include <sstream>
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include "StudentTaskData.h"
#include "SeniorDesign.h"  // Ensure you're including the header where hwndListBox is declared



//saves in this format:FirstName,LastName,Grade,RpiName,IpAddress,Password,Task1FilePath,Task1Time,Task2FilePath,Task2Time,...
//see files data for default save file
extern HWND hwndListBox;  // Use the external hwndListBox declared elsewhere
extern std::vector<Student> students;  // Use the external students vector

// Function to create directory recursively
bool CreateDirectoryRecursively(const std::wstring& path) {
    size_t pos = 0;
    std::wstring currentPath;

    while ((pos = path.find_first_of(L"\\/", pos)) != std::wstring::npos) {
        currentPath = path.substr(0, pos++);
        if (!currentPath.empty() && !PathFileExists(currentPath.c_str())) {
            if (!CreateDirectory(currentPath.c_str(), NULL)) {
                DWORD error = GetLastError();
                if (error != ERROR_ALREADY_EXISTS) {
                    std::wstring errorMsg = L"Failed to create directory: " + currentPath + L" Error code: " + std::to_wstring(error);
                    MessageBox(NULL, errorMsg.c_str(), L"Directory Creation Error", MB_OK);
                    return false;
                }
            }
        }
    }

    if (!PathFileExists(path.c_str())) {
        if (!CreateDirectory(path.c_str(), NULL)) {
            DWORD error = GetLastError();
            std::wstring errorMsg = L"Failed to create directory: " + path + L" Error code: " + std::to_wstring(error);
            MessageBox(NULL, errorMsg.c_str(), L"Directory Creation Error", MB_OK);
            return false;
        }
    }

    return true;
}

// Function to get the AppData saves path
std::wstring GetAppDataSavesPath() {
    WCHAR path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, path))) {
        std::wstring appDataPath = std::wstring(path) + L"\\SeniorDesign\\saves";
        if (CreateDirectoryRecursively(appDataPath)) {
            return appDataPath;
        }
    }
    MessageBox(NULL, L"Failed to get AppData path or create saves folder.", L"Error", MB_OK);
    return L"";
}

// Save data to file

// Save data to file
void SaveDataToFile() {
    std::wstring savesPath = GetAppDataSavesPath();
    if (savesPath.empty()) return;

    std::wstring filePath = savesPath + L"\\data.txt";
    std::wofstream outFile(filePath);
    if (!outFile) {
        MessageBox(nullptr, L"Error opening file for writing.", L"Error", MB_OK);
        return;
    }

    for (const auto& student : students) {
        outFile << student.firstName << L"," << student.lastName << L"," << student.grade << L","
            << student.rpi << L"," << student.ipAddress << L"," << student.password;

        // Save each task's ID, name, filename, and time
        for (const auto& task : student.tasks) {
            outFile << L"," << task.id << L"," << task.name << L"," << task.audioFilePath << L"," << task.time;
        }

        outFile << L"\n";  // Move to the next line for the next student
    }

    outFile.close();
}


// Load data from file
// Load data from file
// Load data from file
void LoadDataFromFile() {
    std::wstring savesPath = GetAppDataSavesPath();
    if (savesPath.empty()) return;

    std::wstring filePath = savesPath + L"\\data.txt";
    std::wifstream inFile(filePath);
    if (!inFile) {
        MessageBox(nullptr, L"Error opening file for reading.", L"Error", MB_OK);
        return;
    }

    students.clear();
    std::wstring line;
    while (std::getline(inFile, line)) {
        Student newStudent;
        std::wstringstream studentStream(line);

        std::getline(studentStream, newStudent.firstName, L',');
        std::getline(studentStream, newStudent.lastName, L',');
        std::getline(studentStream, newStudent.grade, L',');
        std::getline(studentStream, newStudent.rpi, L',');
        std::getline(studentStream, newStudent.ipAddress, L',');
        std::getline(studentStream, newStudent.password, L',');


        // Load tasks if available
        while (studentStream.good()) {
            Task newTask;
            std::wstring taskIdStr;

            if (std::getline(studentStream, taskIdStr, L',') &&
                std::getline(studentStream, newTask.name, L',') &&
                std::getline(studentStream, newTask.audioFilePath, L',') &&
                std::getline(studentStream, newTask.time, L',')) {

                newTask.id = std::stoi(taskIdStr);  // Convert the task ID to an integer
                newStudent.tasks.push_back(newTask);
            }
        }

        students.push_back(newStudent);
    }

    inFile.close();

    // Update ListBox after loading data
    SendMessage(hwndListBox, LB_RESETCONTENT, 0, 0);  // Clear the ListBox first
    for (const auto& student : students) {
        SendMessage(hwndListBox, LB_ADDSTRING, 0, (LPARAM)((student.firstName + L" " + student.lastName).c_str()));
    }
}
// Export data to file
void ExportDataToFile() {
    std::wstring savesPath = GetAppDataSavesPath();
    if (savesPath.empty()) return;

    std::wstring filePath = savesPath + L"\\exports.txt";
    std::wofstream outFile(filePath);
    if (!outFile) {
        MessageBox(nullptr, L"Error opening export file for writing.", L"Error", MB_OK);
        return;
    }

    for (const auto& student : students) {
        outFile << student.rpi << L"," << student.ipAddress << L"," << student.password;

        // Save each task's name, filename, and time
        for (const auto& task : student.tasks) {
            outFile << L"," << task.name << L"," << task.audioFilePath << L"," << task.time;
        }

        outFile << L"\n";  // Move to the next line for the next student
    }

    outFile.close();
    //optional message for testingg
  //  MessageBox(nullptr, L"Data exported successfully to exports.txt.", L"Export Success", MB_OK);
}
