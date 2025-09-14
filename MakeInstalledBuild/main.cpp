#include <Windows.h>
#include <iostream>
#include <string>

int main() {

    std::wstring valueName = L"5.5.4FFS";
    std::wstring keyPath = L"SOFTWARE\\Epic Games\\Unreal Engine\\Builds";

    std::wcout << L"/////////////////////////////////" << std::endl;
    std::wcout << L"///// Friendly Fire Studios /////" << std::endl;
    std::wcout << L"/////////////////////////////////" << std::endl;
    std::wcout << L"                                 " << std::endl;
    std::wcout << L"Creating new engine Build: " << valueName << std::endl;

	wchar_t buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    std::wstring fullPath(buffer);
    std::wstring::size_type pos = fullPath.find_last_of(L"\\");
    std::wstring valueData = fullPath.substr(0, pos);

    HKEY hKey;
    DWORD disposition;
    LONG result = RegCreateKeyEx(HKEY_CURRENT_USER, keyPath.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, &disposition);
    if (result != ERROR_SUCCESS) {
        std::cerr << "Error to create Registry key: " << result << std::endl;
        system("pause");
        return 1;
    }

    result = RegSetValueEx(hKey, valueName.c_str(), 0, REG_SZ, reinterpret_cast<const BYTE*>(valueData.c_str()), (valueData.size() + 1) * sizeof(wchar_t));
    if (result != ERROR_SUCCESS) {
        std::cerr << "Error to add registry value: " << result << std::endl;
        RegCloseKey(hKey);
        system("pause");
        return 2;
    }

    RegCloseKey(hKey);

    std::wcout << L"Installed Successfully " << std::endl;
    system("pause");
    return 0;
}