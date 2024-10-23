// StudentTaskData.h
#ifndef STUDENTTASKDATA_H
#define STUDENTTASKDATA_H

#include <string>
#include <vector>

struct Task {
    int id;
    std::wstring name;
    std::wstring audioFilePath;
    std::wstring time;
};

struct Student {
    int id;
    std::wstring firstName;
    std::wstring lastName;
    std::wstring grade;
    std::wstring rpi;         // New field for Raspberry Pi name
    std::wstring ipAddress;   // New field for IP address
    std::wstring password;    // New field for password
    std::vector<Task> tasks;  // Assuming Task is a structure containing task details
};

// Declare the global students vector
extern std::vector<Student> students;

#endif // STUDENTTASKDATA_H
