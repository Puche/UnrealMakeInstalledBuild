#include <Windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

// Function to extract integer value from JSON line
int ExtractJsonInt(const std::string& line, const std::string& key) {
    size_t pos = line.find(key);
    if (pos == std::string::npos) return -1;
    
    // Find the colon after the key
    size_t colonPos = line.find(':', pos);
    if (colonPos == std::string::npos) return -1;
    
    // Extract the number after the colon
    std::string numStr;
    for (size_t i = colonPos + 1; i < line.length(); i++) {
        if (isdigit(line[i])) {
            numStr += line[i];
        } else if (!numStr.empty()) {
            break;
        }
    }
    
    return numStr.empty() ? -1 : std::stoi(numStr);
}

// Function to read version from Build.version file
std::wstring GetEngineVersion() {
    // Get executable directory
    wchar_t buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    std::wstring fullPath(buffer);
    std::wstring::size_type pos = fullPath.find_last_of(L"\\");
    std::wstring exeDir = fullPath.substr(0, pos);
    
    // Build path to Build.version file
    std::wstring buildVersionPath = exeDir + L"\\Engine\\Build\\Build.version";
    
    // Open file
    std::ifstream file(buildVersionPath);
    if (!file.is_open()) {
        std::wcerr << L"ERROR: Could not open Build.version file at: " << buildVersionPath << std::endl;
        std::wcerr << L"Using default version: 5.6.1FFS" << std::endl;
        return L"5.6.1FFS";
    }
    
    // Parse JSON manually (simple approach for this specific structure)
    int majorVersion = -1;
    int minorVersion = -1;
    int patchVersion = -1;
    
    std::string line;
    while (std::getline(file, line)) {
        if (majorVersion == -1) {
            int val = ExtractJsonInt(line, "MajorVersion");
            if (val != -1) majorVersion = val;
        }
        if (minorVersion == -1) {
            int val = ExtractJsonInt(line, "MinorVersion");
            if (val != -1) minorVersion = val;
        }
        if (patchVersion == -1) {
            int val = ExtractJsonInt(line, "PatchVersion");
            if (val != -1) patchVersion = val;
        }
        
        // Stop if we found all three values
        if (majorVersion != -1 && minorVersion != -1 && patchVersion != -1) {
            break;
        }
    }
    
    file.close();
    
    // Validate that we got all values
    if (majorVersion == -1 || minorVersion == -1 || patchVersion == -1) {
        std::wcerr << L"ERROR: Could not parse version numbers from Build.version" << std::endl;
        std::wcerr << L"Using default version: 5.6.1FFS" << std::endl;
        return L"5.6.1FFS";
    }
    
    // Build version string: "Major.Minor.PatchFFS"
    std::wstringstream versionStream;
    versionStream << majorVersion << L"." << minorVersion << L"." << patchVersion << L"FFS";
    
    return versionStream.str();
}

int main() {
    // Get version dynamically from Build.version
    std::wstring valueName = GetEngineVersion();
    std::wstring keyPath = L"SOFTWARE\\Epic Games\\Unreal Engine\\Builds";

    std::wcout << L"/////////////////////////////////" << std::endl;
    std::wcout << L"///// Friendly Fire Studios /////" << std::endl;
    std::wcout << L"/////////////////////////////////" << std::endl;
    std::wcout << L"                                 " << std::endl;
    std::wcout << L"Creating/Updating engine Build: " << valueName << std::endl;

    // Get executable directory
    wchar_t buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    std::wstring fullPath(buffer);
    std::wstring::size_type pos = fullPath.find_last_of(L"\\");
    std::wstring valueData = fullPath.substr(0, pos);

    std::wcout << L"Engine Path: " << valueData << std::endl;

    // Check if value already exists
    HKEY hKeyCheck;
    LONG resultCheck = RegOpenKeyEx(
        HKEY_CURRENT_USER,
        keyPath.c_str(),
        0,
        KEY_QUERY_VALUE,
        &hKeyCheck
    );

    bool valueExists = false;
    std::wstring oldPath;
    
    if (resultCheck == ERROR_SUCCESS) {
        wchar_t existingValue[MAX_PATH];
        DWORD bufferSize = sizeof(existingValue);
        DWORD valueType;
        
        LONG queryResult = RegQueryValueEx(
            hKeyCheck,
            valueName.c_str(),
            NULL,
            &valueType,
            reinterpret_cast<BYTE*>(existingValue),
            &bufferSize
        );
        
        if (queryResult == ERROR_SUCCESS) {
            valueExists = true;
            oldPath = existingValue;
            std::wcout << L"                                 " << std::endl;
            std::wcout << L"⚠ Registry value already exists!" << std::endl;
            std::wcout << L"  Old Path: " << oldPath << std::endl;
            std::wcout << L"  New Path: " << valueData << std::endl;
        }
        
        RegCloseKey(hKeyCheck);
    }

    // Create/Open registry key with write permissions
    HKEY hKey;
    DWORD disposition;
    LONG result = RegCreateKeyEx(
        HKEY_CURRENT_USER, 
        keyPath.c_str(), 
        0, 
        NULL, 
        REG_OPTION_NON_VOLATILE, 
        KEY_SET_VALUE | KEY_QUERY_VALUE, // Added KEY_QUERY_VALUE for better permissions
        NULL, 
        &hKey, 
        &disposition
    );
    
    if (result != ERROR_SUCCESS) {
        std::wcerr << L"ERROR: Failed to create/open Registry key. Error code: " << result << std::endl;
        system("pause");
        return 1;
    }

    // Set registry value (this will overwrite if it exists)
    result = RegSetValueEx(
        hKey, 
        valueName.c_str(), 
        0, 
        REG_SZ, 
        reinterpret_cast<const BYTE*>(valueData.c_str()), 
        static_cast<DWORD>((valueData.size() + 1) * sizeof(wchar_t))
    );
    
    if (result != ERROR_SUCCESS) {
        std::wcerr << L"ERROR: Failed to set registry value. Error code: " << result << std::endl;
        RegCloseKey(hKey);
        system("pause");
        return 2;
    }

    RegCloseKey(hKey);

    std::wcout << L"                                 " << std::endl;
    if (valueExists) {
        std::wcout << L"✓ Updated Successfully!" << std::endl;
    } else {
        std::wcout << L"✓ Installed Successfully!" << std::endl;
    }
    std::wcout << L"✓ Registry Key: " << keyPath << std::endl;
    std::wcout << L"✓ Version Name: " << valueName << std::endl;
    std::wcout << L"✓ Current Path: " << valueData << std::endl;
    system("pause");
    return 0;
}