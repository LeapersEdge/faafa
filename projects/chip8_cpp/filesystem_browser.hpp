#pragma once

#include <filesystem>
#include <functional>
#include <string>
#include <vector>

struct Filesystem_Browser
{
    Filesystem_Browser(std::string identifier_name);

    void Reset_States();

    void Dispatch_Dir_File(const std::filesystem::path& start_browser_path, std::function<void(std::filesystem::path result_path)> callback);
    void Dispatch_Dir(const std::filesystem::path& start_browser_path, std::function<void(std::filesystem::path result_path)> callback);

    void Set_File_Whitelist(const std::vector<std::string>& whitelist);
    
    void Imgui_Uncond_Render_Dir_File_Browser();
    void Imgui_Uncond_Render_Dir_Browser();

    bool Is_Dir_File_Dispatched();
    bool Is_Dir_Dispatched();
private:
    std::string identifier_name;

    void Refresh_Browse_Path_Children();
    void Refresh_Browse_Path_Children_Dir();
    
    std::filesystem::path browse_path;
    std::function<void(std::filesystem::path result_path)> end_operation_callback;

    bool dispatched_dir = false;
    bool dispatched_dir_file = false;

    std::vector<std::filesystem::path> browse_path_children;
    std::vector<std::string> file_extension_whitelist;
};

