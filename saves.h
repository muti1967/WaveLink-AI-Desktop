// saves.h
#ifndef SAVES_H
#define SAVES_H

#include <string>

void SaveDataToFile();
void LoadDataFromFile();
void ExportDataToFile();
// Declare the functions
bool CreateDirectoryRecursively(const std::wstring& path);

std::wstring GetAppDataPath();
std::wstring CreateSavesFolder();
std::wstring GetAppDataSavesPath();


#endif // SAVES_H
