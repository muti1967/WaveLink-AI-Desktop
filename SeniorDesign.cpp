#include "framework.h"
#include "SeniorDesign.h"
#include <vector>
#include <string>
#include <algorithm>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#include "saves.h" // Include your new saves header
#include "StudentTaskData.h"  // Include the student/task data header
#include <map>

#include <fstream>  // For file streams
#include <sstream>  // For string streams
std::vector<Student> students;  // Define the global students vector

std::map<int, bool> taskPlayingState;  //declared  once


/////////////////////////////
/*TO DO LIST ok
*  - Make the button task for all also include time
*  - add a edit button/remove for tasks
*  - add a view files button/import export saves
*  - add another folder in saves for the data.txt and one for the audio
*  - add a play button to play the audio/tasks back in the app
*  - make it so it make sures the correct format is being entered
*
* ---- PRIORITY ---
*   - does not save new audio or tasks sometimes
*   - removes wronf student sometimes
* -ADD DELETE TASK BUTTON
* -FIX TIME NOT BEING ADDED FORALLSTUDENT TASKS
*
* Later tasks:
* - Figure out how to make the raspberrypi recieve this info, unpack the data file and use the packets sent.
*
* Wants:
*
*
*
* in progress:
*
*
*
* completed tasks :
* - i want to be able to generate a data file that contains a ip address for the raspberry pi, the name of the raspberry pi, and all the information
*       from the student, their full name, but mainly the tasks assigned. any unecessary information should not be sent.
*/

/////////////////////////////


#define MAX_LOADSTRING 100
#define IDC_LISTBOX 103
#define IDC_ADDSTUDENT_BUTTON 104  // New ID for the Add Student button
#define IDC_DROPDOWN1 201
#define IDC_DROPDOWN2 202
#define IDC_DROPDOWN3 203
#define IDC_DROPDOWN4 204
#define IDC_DROPDOWN5 205
#define IDD_TASK_DIALOG 101
#define IDD_ADDSTUDENT_DIALOG 104  // New dialog ID for adding students
#define IDC_TASK_EDIT 1001
#define IDC_STUDENT_FIRSTNAME_EDIT 1002  // Control ID for First Name Edit
#define IDC_STUDENT_LASTNAME_EDIT 1003   // Control ID for Last Name Edit
#define IDC_STUDENT_GRADE_EDIT 1004      // Control ID for Grade Edit
#define IDC_REMOVESTUDENT_BUTTON 206  // New ID for the Remove Student button

// Function declarations
void SaveDataToFile();
void LoadDataFromFile();

// Global Variables
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
int taskCount = 1; // Global task count for tasks assigned to all students
int functioncalls_stopall = 0;


HWND hwndRemoveStudentButton;  // Handle for the Remove Student button
HWND hwndListBox;                               // Handle for the list box
HWND hwndDropButton1, hwndDropButton2, hwndDropButton3, hwndDropButton4, hwndDropButton5;  // Handles for dropdown buttons
HWND hwndAddStudentButton;                      // Handle for the Add Student button
HBRUSH hbrBackground;                           // Handle to the background brush

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    TaskDlgProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    AddStudentDlgProc(HWND, UINT, WPARAM, LPARAM);  // New dialog procedure for adding students
void AddTaskForStudent(HWND hWnd, int selectedIndex);
void AddTaskForAllStudents(HWND hWnd);
void StartRecording(HWND hWnd);
void StopRecording(HWND hWnd, std::wstring& filePath, int selectedIndex, const std::wstring& taskName);
void ViewTasksForStudent(HWND hWnd, int selectedIndex);
void AddStudentToList(HWND hWnd, const Student& newStudent);  // New function to add a student to the list
void PlayPauseAudio(const std::wstring& audioFilePath, int taskIndex);

// The rest of your code...



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Load student and task data from file
    LoadDataFromFile();

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SENIORDESIGN, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SENIORDESIGN));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // Save student and task data to file before exiting
    SaveDataToFile();

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SENIORDESIGN));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(RGB(50, 50, 50)); // Dark grey background
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SENIORDESIGN);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable
    LoadDataFromFile(); // Load data on startup
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}
void ResizeControls(HWND hWnd)
{
    RECT rect;
    GetClientRect(hWnd, &rect);

    int windowWidth = rect.right - rect.left;
    int windowHeight = rect.bottom - rect.top;

    int listBoxWidth = 210;
    int listBoxHeight = 100;

    // Calculate centered positions
    int listBoxX = (windowWidth / 2) - (listBoxWidth / 2);
    int listBoxY = (windowHeight / 2) - (listBoxHeight / 2);

    // Set the positions
    SetWindowPos(hwndListBox, nullptr, listBoxX, listBoxY, listBoxWidth, listBoxHeight, SWP_NOZORDER);

    // Position the Add Student button above the listbox
    int addStudentButtonWidth = 150;
    int addStudentButtonHeight = 30;
    int addStudentButtonX = listBoxX;
    int addStudentButtonY = listBoxY - addStudentButtonHeight - 10;
    SetWindowPos(hwndAddStudentButton, nullptr, addStudentButtonX, addStudentButtonY, addStudentButtonWidth, addStudentButtonHeight, SWP_NOZORDER);

    // Position the Remove Student button below the listbox
    int removeStudentButtonWidth = 150;
    int removeStudentButtonHeight = 30;
    int removeStudentButtonX = listBoxX;
    int removeStudentButtonY = listBoxY + listBoxHeight + 10;
    SetWindowPos(hwndRemoveStudentButton, nullptr, removeStudentButtonX, removeStudentButtonY, removeStudentButtonWidth, removeStudentButtonHeight, SWP_NOZORDER);

    // Position the dropdown buttons (initially hidden)
    int dropdownX = listBoxX + listBoxWidth + 10;
    SetWindowPos(hwndDropButton1, nullptr, dropdownX, listBoxY, 150, 30, SWP_NOZORDER | SWP_HIDEWINDOW);
    SetWindowPos(hwndDropButton2, nullptr, dropdownX, listBoxY + 40, 150, 30, SWP_NOZORDER | SWP_HIDEWINDOW);
    SetWindowPos(hwndDropButton3, nullptr, dropdownX, listBoxY + 80, 150, 30, SWP_NOZORDER | SWP_HIDEWINDOW);
    SetWindowPos(hwndDropButton4, nullptr, dropdownX, listBoxY + 120, 150, 30, SWP_NOZORDER | SWP_HIDEWINDOW);
    SetWindowPos(hwndDropButton5, nullptr, dropdownX, listBoxY + 160, 150, 30, SWP_NOZORDER | SWP_HIDEWINDOW);
    SetWindowPos(hwndRemoveStudentButton, nullptr, dropdownX, listBoxY + 200, 150, 30, SWP_NOZORDER | SWP_HIDEWINDOW); // Position Remove Student button
}


void ShowDropdownButtons(HWND hWnd, int selectedIndex)
{
    // Update the global `selectedIndex` variable when dropdown is shown
    SetWindowLongPtr(hWnd, GWLP_USERDATA, selectedIndex);

    // Show dropdown buttons
    ShowWindow(hwndDropButton1, SW_SHOW);
    ShowWindow(hwndDropButton2, SW_SHOW);
    ShowWindow(hwndDropButton3, SW_SHOW);
    ShowWindow(hwndDropButton4, SW_SHOW);
    ShowWindow(hwndDropButton5, SW_SHOW);
    ShowWindow(hwndRemoveStudentButton, SW_SHOW); // Show the Remove Student button
}


void RemoveStudent(HWND hWnd, int selectedIndex) {
    // Explicitly fetch the selected index from the listbox right before removal
    selectedIndex = static_cast<int>(SendMessage(hwndListBox, LB_GETCURSEL, 0, 0));

    if (selectedIndex < 0 || selectedIndex >= students.size()) {
        MessageBox(hWnd, L"No student selected or invalid index.", L"Error", MB_OK);
        return;
    }

    // Confirm deletion
    int response = MessageBox(hWnd, L"Are you sure you want to remove this student?", L"Remove Student", MB_YESNO);
    if (response == IDYES) {
        students.erase(students.begin() + selectedIndex);

        // Update the ListBox
        SendMessage(hwndListBox, LB_RESETCONTENT, 0, 0);  // Clear the ListBox
        for (const auto& student : students) {
            SendMessage(hwndListBox, LB_ADDSTRING, 0, (LPARAM)(student.firstName + L" " + student.lastName).c_str());
        }

        // Optionally, reset the dropdown buttons and selection
        SetWindowLongPtr(hWnd, GWLP_USERDATA, -1);  // Reset selected index
        ShowWindow(hwndDropButton1, SW_HIDE);
        ShowWindow(hwndDropButton2, SW_HIDE);
        ShowWindow(hwndDropButton3, SW_HIDE);
        ShowWindow(hwndDropButton4, SW_HIDE);
        ShowWindow(hwndDropButton5, SW_HIDE);
        ShowWindow(hwndRemoveStudentButton, SW_HIDE);  // Hide Remove Student button
    }
}

void StartRecording(HWND hWnd)
{
    // Starting recording
    mciSendString(L"open new type waveaudio alias recsound", NULL, 0, NULL);
    mciSendString(L"record recsound", NULL, 0, NULL);

    MessageBox(hWnd, L"Recording started...", L"Audio Recording", MB_OK);
}
void StopRecording(HWND hWnd, std::wstring& filePath, int selectedIndex, const std::wstring& taskName)
{
    int studentID = students[selectedIndex].id;
    std::wstring lastName = students[selectedIndex].lastName;

    std::wstring sanitizedTaskName = taskName;
    std::replace(sanitizedTaskName.begin(), sanitizedTaskName.end(), L' ', L'_');

    std::wstring folderPath = GetAppDataSavesPath();
    if (folderPath.empty()) {
        MessageBox(hWnd, L"Failed to create or access the saves folder.", L"Error", MB_OK);
        return;
    }

    std::wstring fileName = std::to_wstring(studentID) + L"_" + lastName + L"_" + sanitizedTaskName + L".wav";
    filePath = folderPath + L"\\" + fileName;

    mciSendString(L"stop recsound", NULL, 0, NULL);
    mciSendString((L"save recsound " + filePath).c_str(), NULL, 0, NULL);
    mciSendString(L"close recsound", NULL, 0, NULL);

    MessageBox(hWnd, (L"Recording saved as " + filePath).c_str(), L"Audio Recording", MB_OK);
}

void StopRecordingForAll(HWND hWnd, std::wstring& filePath, int taskCount, const std::wstring& taskName)
{
    std::wstring sanitizedTaskName = taskName;
    std::replace(sanitizedTaskName.begin(), sanitizedTaskName.end(), L' ', L'_');

    std::wstring folderPath = GetAppDataSavesPath();
    if (folderPath.empty()) {
        MessageBox(hWnd, L"Failed to create or access the saves folder.", L"Error", MB_OK);
        return;
    }

    std::wstring fileName = std::to_wstring(taskCount) + L"_taskforall_" + sanitizedTaskName + L".wav";
    filePath = folderPath + L"\\" + fileName;

    mciSendString(L"stop recsound", NULL, 0, NULL);
    mciSendString((L"save recsound " + filePath).c_str(), NULL, 0, NULL);
    mciSendString(L"close recsound", NULL, 0, NULL);

    MessageBox(hWnd, (L"Recording saved as " + filePath).c_str(), L"Audio Recording", MB_OK);
}

void AddTaskForStudent(HWND hWnd, int selectedIndex)
{
    Task newTask;
    newTask.id = static_cast<int>(students[selectedIndex].tasks.size()) + 1;

    // Buffers for task details
    wchar_t taskName[256] = L"";
    std::wstring taskAudioFile;

    // Task Name
    if (DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_TASK_DIALOG), hWnd, TaskDlgProc, (LPARAM)taskName) == IDOK)
    {
        newTask.name = taskName;

        // Recording Audio (instead of description)
        int response = MessageBox(hWnd, L"Do you want to record audio?", L"Record Audio", MB_YESNO);
        if (response == IDYES)
        {
            StartRecording(hWnd);
            MessageBox(hWnd, L"Press OK to stop recording", L"Stop Recording", MB_OK);
            StopRecording(hWnd, taskAudioFile, selectedIndex, taskName);
            newTask.audioFilePath = taskAudioFile; // Save the file path as the audio file path

            // Task Time
            wchar_t taskTime[64] = L"";
            if (DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_TIME_DIALOG), hWnd, TaskDlgProc, (LPARAM)taskTime) == IDOK)
            {
                newTask.time = taskTime;

                // Add the new task to the student's task list
                students[selectedIndex].tasks.push_back(newTask);
                MessageBox(hWnd, L"Task added successfully!", L"Task Added", MB_OK);
            }
        }
    }
}

void AddTaskForAllStudents(HWND hWnd)
{
    Task newTask;

    // Buffers for task details
    wchar_t taskName[256] = L"";
    std::wstring taskAudioFile;

    // Task Name
    if (DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_TASK_DIALOG), hWnd, TaskDlgProc, (LPARAM)taskName) == IDOK)
    {
        newTask.name = taskName;

        // Recording Audio (instead of description)
        int response = MessageBox(hWnd, L"Do you want to record audio?", L"Record Audio", MB_YESNO);
        if (response == IDYES)
        {
            StartRecording(hWnd);
            MessageBox(hWnd, L"Press OK to stop recording", L"Stop Recording", MB_OK);

            // Create a filename for this task before the loop
            StopRecordingForAll(hWnd, taskAudioFile, taskCount, taskName);

            // Add the new task to each student's task list
            for (Student& student : students)
            {
                newTask.audioFilePath = taskAudioFile; // Save the file path as the audio file path
                newTask.id = static_cast<int>(student.tasks.size()) + 1; // Unique ID for each student
                student.tasks.push_back(newTask);
            }

            taskCount++; // Increment the global taskCount only once after all students have been processed
            MessageBox(hWnd, L"Task added successfully to all students!", L"Task Added", MB_OK);
        }
    }
}

void AddStudentToList(HWND hWnd, const Student& newStudent)
{
    students.push_back(newStudent);

    // Add the new student to the list box
    SendMessage(hwndListBox, LB_ADDSTRING, 0, (LPARAM)(newStudent.firstName + L" " + newStudent.lastName).c_str());
}

void ViewTasksForStudent(HWND hWnd, int selectedIndex) {
    if (students[selectedIndex].tasks.empty()) {
        MessageBox(hWnd, L"No tasks assigned.", L"Tasks", MB_OK);
        return;
    }

    int taskButtonID = 5000; // Starting ID for task buttons
    int yPosition = 50; // Initial Y position for the first task

    for (const auto& task : students[selectedIndex].tasks) {
        std::wstring taskInfo = L"Task #" + std::to_wstring(task.id) + L"\n" +
            L"Name: " + task.name + L"\n" +
            L"Time: " + task.time + L"\n";

        // Create a static text control for the task info
        CreateWindowW(L"STATIC", taskInfo.c_str(), WS_VISIBLE | WS_CHILD,
            10, yPosition, 300, 60, hWnd, nullptr, hInst, NULL);

        if (!task.audioFilePath.empty()) {
            // Create the Play/Pause button
            CreateWindowW(L"BUTTON", L"Play/Pause", WS_VISIBLE | WS_CHILD,
                320, yPosition, 100, 30, hWnd, (HMENU)(taskButtonID++), hInst, NULL);

            // Create the Redo Audio button
            CreateWindowW(L"BUTTON", L"Redo Audio", WS_VISIBLE | WS_CHILD,
                430, yPosition, 100, 30, hWnd, (HMENU)(taskButtonID++), hInst, NULL);
        }

        yPosition += 80; // Increment Y position for the next task
    }
}



#include <map>  // Ensure this include is at the top


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int selectedIndex = -1; // To track the selected student index

    switch (message)
    {
    case WM_CREATE:
    {
        hwndListBox = CreateWindowW(L"LISTBOX", NULL, WS_CHILD | WS_VISIBLE | LBS_NOTIFY | LBS_STANDARD,
            0, 0, 210, 100, hWnd, (HMENU)IDC_LISTBOX, hInst, NULL);

        hwndAddStudentButton = CreateWindowW(L"BUTTON", L"Add Student", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            0, 0, 150, 30, hWnd, (HMENU)IDC_ADDSTUDENT_BUTTON, hInst, NULL);

        hwndRemoveStudentButton = CreateWindowW(L"BUTTON", L"Remove Student", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            0, 0, 150, 30, hWnd, reinterpret_cast<HMENU>(IDC_REMOVESTUDENT_BUTTON), hInst, NULL);

        for (const auto& student : students) {
            SendMessage(hwndListBox, LB_ADDSTRING, 0, (LPARAM)(student.firstName + L" " + student.lastName).c_str());
        }

        hwndDropButton1 = CreateWindowW(L"BUTTON", L"Show Name and Grade", WS_CHILD | BS_PUSHBUTTON,
            0, 0, 150, 30, hWnd, (HMENU)IDC_DROPDOWN1, hInst, NULL);

        hwndDropButton2 = CreateWindowW(L"BUTTON", L"Show ID Number", WS_CHILD | BS_PUSHBUTTON,
            0, 0, 150, 30, hWnd, (HMENU)IDC_DROPDOWN2, hInst, NULL);

        hwndDropButton3 = CreateWindowW(L"BUTTON", L"Add Task", WS_CHILD | BS_PUSHBUTTON,
            0, 0, 150, 30, hWnd, (HMENU)IDC_DROPDOWN3, hInst, NULL);

        hwndDropButton4 = CreateWindowW(L"BUTTON", L"View Tasks", WS_CHILD | BS_PUSHBUTTON,
            0, 0, 150, 30, hWnd, (HMENU)IDC_DROPDOWN4, hInst, NULL);

        hwndDropButton5 = CreateWindowW(L"BUTTON", L"Add Task to All", WS_CHILD | BS_PUSHBUTTON,
            0, 0, 150, 30, hWnd, (HMENU)IDC_DROPDOWN5, hInst, NULL);

        ResizeControls(hWnd);
    }
    break;

    case WM_SIZE:
        ResizeControls(hWnd);
        break;

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        int wmEvent = HIWORD(wParam);

        switch (wmId)
        {
        case IDC_LISTBOX:
            if (wmEvent == LBN_SELCHANGE) {
                selectedIndex = static_cast<int>(SendMessage(hwndListBox, LB_GETCURSEL, 0, 0));
                if (selectedIndex != LB_ERR) {
                    ShowDropdownButtons(hWnd, selectedIndex);
                }
            }
            break;

        case IDC_ADDSTUDENT_BUTTON:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ADDSTUDENT_DIALOG), hWnd, AddStudentDlgProc);
            break;

        case IDC_REMOVESTUDENT_BUTTON:
            RemoveStudent(hWnd, selectedIndex);
            break;

        case IDC_DROPDOWN1:
            if (selectedIndex != -1) {
                wchar_t message[100];
                swprintf_s(message, 100, L"Student: %s %s\nGrade: %s",
                    students[selectedIndex].firstName.c_str(),
                    students[selectedIndex].lastName.c_str(),
                    students[selectedIndex].grade.c_str());
                MessageBox(hWnd, message, L"Student Info", MB_OK);
            }
            break;

        case IDC_DROPDOWN2:
            if (selectedIndex != -1) {
                wchar_t message[50];
                swprintf_s(message, 50, L"Student ID: %d", students[selectedIndex].id);
                MessageBox(hWnd, message, L"Student ID", MB_OK);
            }
            break;

        case IDC_DROPDOWN3:
            if (selectedIndex != -1) {
                AddTaskForStudent(hWnd, selectedIndex);
            }
            break;

        case IDC_DROPDOWN4:
            if (selectedIndex != -1) {
                ViewTasksForStudent(hWnd, selectedIndex);
            }
            break;

        case IDC_DROPDOWN5:
            AddTaskForAllStudents(hWnd);  // Add a task for all students
            break;

        default:
            if (wmId >= 5000 && wmId < 5100) {
                int buttonIndex = wmId - 5000;
                int taskIndex = buttonIndex / 2;  // Assuming Play/Pause and Redo buttons are adjacent
                int buttonType = buttonIndex % 2;  // 0 for Play/Pause, 1 for Redo Audio

                if (selectedIndex != -1 && taskIndex < students[selectedIndex].tasks.size()) {
                    if (buttonType == 0) {
                        // Play/Pause button
                        bool& isPlaying = taskPlayingState[taskIndex];
                        PlayPauseAudio(students[selectedIndex].tasks[taskIndex].audioFilePath, isPlaying);
                    }
                    else if (buttonType == 1) {
                        // Redo Audio button
                        std::wstring taskAudioFile;
                        StartRecording(hWnd);
                        MessageBox(hWnd, L"Press OK to stop recording", L"Stop Recording", MB_OK);
                        StopRecording(hWnd, taskAudioFile, selectedIndex, students[selectedIndex].tasks[taskIndex].name);

                        // Update the task's audio file path
                        students[selectedIndex].tasks[taskIndex].audioFilePath = taskAudioFile;

                        MessageBox(hWnd, L"Audio re-recorded successfully!", L"Redo Audio", MB_OK);
                    }
                }
            }
            break;
        }

    }
    break;

    case WM_CTLCOLORBTN:
    {
        HDC hdc = (HDC)wParam;
        SetTextColor(hdc, RGB(255, 255, 255));   // White text
        SetBkColor(hdc, RGB(50, 50, 50));        // Dark grey background
        return (INT_PTR)CreateSolidBrush(RGB(50, 50, 50));
    }
    break;

    case WM_CTLCOLORLISTBOX:
    {
        HDC hdc = (HDC)wParam;
        SetTextColor(hdc, RGB(255, 255, 255));   // White text
        SetBkColor(hdc, RGB(50, 50, 50));        // Dark grey background
        return (INT_PTR)CreateSolidBrush(RGB(50, 50, 50));
    }
    break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        SetTextColor(hdc, RGB(255, 255, 255));
        SetBkColor(hdc, RGB(50, 50, 50));

        RECT rect;
        GetClientRect(hWnd, &rect);
        DrawText(hdc, L"Task Assignment Center!", -1, &rect, DT_CENTER | DT_TOP);

        EndPaint(hWnd, &ps);
    }
    break;

    case WM_DESTROY:
        SaveDataToFile();  // Save student and task data to file before exiting
        ExportDataToFile();

        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);

    }
    return 0;
}


INT_PTR CALLBACK TaskDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            wchar_t* task = (wchar_t*)GetWindowLongPtr(hDlg, GWLP_USERDATA);
            GetDlgItemText(hDlg, IDC_TASK_EDIT, task, 256);
            EndDialog(hDlg, IDOK);
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, IDCANCEL);
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK AddStudentDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            Student newStudent;

            // Get first name
            wchar_t firstName[256];
            GetDlgItemText(hDlg, IDC_STUDENT_FIRSTNAME_EDIT, firstName, 256);
            newStudent.firstName = firstName;

            // Get last name
            wchar_t lastName[256];
            GetDlgItemText(hDlg, IDC_STUDENT_LASTNAME_EDIT, lastName, 256);
            newStudent.lastName = lastName;

            // Get grade
            wchar_t grade[256];
            GetDlgItemText(hDlg, IDC_STUDENT_GRADE_EDIT, grade, 256);
            newStudent.grade = grade;

            // Get Raspberry Pi name
            wchar_t rpi[256];
            GetDlgItemText(hDlg, IDC_STUDENT_RPI_EDIT, rpi, 256);
            newStudent.rpi = rpi;

            // Get IP address
            wchar_t ipAddress[256];
            GetDlgItemText(hDlg, IDC_STUDENT_IPADDRESS_EDIT, ipAddress, 256);
            newStudent.ipAddress = ipAddress;

            // Get password
            wchar_t password[256];
            GetDlgItemText(hDlg, IDC_STUDENT_PASSWORD_EDIT, password, 256);
            newStudent.password = password;

            // Assign an ID
            newStudent.id = static_cast<int>(students.size()) + 1;

            // Add the student to the list
            students.push_back(newStudent);

            // Update the ListBox
            SendMessage(hwndListBox, LB_ADDSTRING, 0, (LPARAM)(newStudent.firstName + L" " + newStudent.lastName).c_str());

            EndDialog(hDlg, IDOK);
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, IDCANCEL);
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void PlayPauseAudio(const std::wstring& audioFilePath, int taskIndex) {
    std::wstring alias = L"myAudio" + std::to_wstring(taskIndex);  // Create a unique alias for each task

    if (taskPlayingState[taskIndex]) {  // Use taskIndex as the key
        mciSendString((L"pause " + alias).c_str(), NULL, 0, NULL);
    }
    else {
        std::wstring openCommand = L"open \"" + audioFilePath + L"\" alias " + alias;
        mciSendString(openCommand.c_str(), NULL, 0, NULL);
        std::wstring playCommand = L"play " + alias;
        mciSendString(playCommand.c_str(), NULL, 0, NULL);
    }

    taskPlayingState[taskIndex] = !taskPlayingState[taskIndex];  // Toggle the playing state
}
