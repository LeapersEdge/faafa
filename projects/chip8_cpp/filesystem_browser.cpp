#include "filesystem_browser.hpp"

#include "imgui.h"
#include "raylib.h"
#include <algorithm>
#include <cassert>
#include <filesystem>

namespace fs = std::filesystem;

Filesystem_Browser::Filesystem_Browser(std::string identifier_name)
    :
    identifier_name(identifier_name)
{
    browse_path = fs::current_path();
}

void Filesystem_Browser::Reset_States()
{
    dispatched_dir = false;
    dispatched_dir_file = false;
    file_extension_whitelist.clear();
    browse_path_children.clear();
}

void Filesystem_Browser::Imgui_Uncond_Render_Dir_File_Browser()
{
    if (dispatched_dir_file)
    {

        ImGui::SetNextWindowSize(ImVec2(0,0));
        ImGui::Begin(("Directory & File Browser##" + identifier_name).c_str());

        ImGui::Text(browse_path.string().c_str());
        
        if (ImGui::Button("Up##fs browser dir file"))
        {
            if (browse_path.has_parent_path())
            {
                browse_path = browse_path.parent_path();
                Refresh_Browse_Path_Children();
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Refresh##fs browser dir file"))
            Refresh_Browse_Path_Children();

        if (ImGui::BeginListBox("##dir file browser", ImVec2(-FLT_MIN, ImGui::GetTextLineHeightWithSpacing() * 8)))
        {
            for (auto& child_path : browse_path_children)
            {
                if (ImGui::Selectable(child_path.filename().string().c_str()))
                {
                    if (fs::is_directory(child_path))
                    {
                        browse_path = child_path;
                        Refresh_Browse_Path_Children(); 

                        ImGui::EndListBox();
                        ImGui::End();
                        return;
                    }
                    else if (child_path.has_extension())
                    {
                        dispatched_dir_file = false;
                        TraceLog(LOG_INFO, "FS Browser: File selected, calling callback with result path: %s", child_path.c_str()); 

                        ImGui::EndListBox();
                        ImGui::End();
                        end_operation_callback(child_path); // calling callback after imgui finished window just in case
                        return;
                    }
                }
            }

            ImGui::EndListBox();
        }

        ImGui::End();
    }
}

void Filesystem_Browser::Imgui_Uncond_Render_Dir_Browser()
{
    if (dispatched_dir)
    {
        ImGui::SetNextWindowSize(ImVec2(0,0));
        ImGui::Begin(("Directory Browser##" + identifier_name).c_str());

        ImGui::Text(browse_path.string().c_str());

        if (ImGui::Button("Up##fs browser dir"))
        {
            if (browse_path.has_parent_path())
            {
                browse_path = browse_path.parent_path();
                Refresh_Browse_Path_Children_Dir();
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Refresh##fs browser dir"))
            Refresh_Browse_Path_Children_Dir();

        if (ImGui::BeginListBox("##dir file browser", ImVec2(-FLT_MIN, ImGui::GetTextLineHeightWithSpacing() * 8)))
        {
            for (auto& child_path : browse_path_children)
            {
                if (ImGui::Selectable(child_path.filename().string().c_str()))
                {
                    browse_path = child_path;
                    Refresh_Browse_Path_Children_Dir(); 

                    ImGui::EndListBox();
                    ImGui::End();
                    return;
                }
            }

            ImGui::EndListBox();
        }

        if (ImGui::Button("Select current directory"))
        {
            dispatched_dir = false;
            TraceLog(LOG_INFO, "FS Browser: Dir selected, calling callback with result path: %s", browse_path.c_str()); 
            end_operation_callback(browse_path);
        }

        ImGui::End();    
    }
}

void Filesystem_Browser::Dispatch_Dir_File(const fs::path& path, std::function<void(std::filesystem::path result_path)> callback)
{
    browse_path = path;
    Refresh_Browse_Path_Children();
    end_operation_callback = callback;
    
    dispatched_dir_file = true;
    dispatched_dir = false;
}

void Filesystem_Browser::Dispatch_Dir(const fs::path& path, std::function<void(std::filesystem::path result_path)> callback)
{
    browse_path = path;
    Refresh_Browse_Path_Children_Dir();
    end_operation_callback = callback;

    dispatched_dir = true;
    dispatched_dir_file = false;
}

void Filesystem_Browser::Refresh_Browse_Path_Children()
{
    browse_path_children.clear();
  
    for (const auto & entry : fs::directory_iterator(browse_path))
    {
        if (fs::is_directory(entry) || (
                                            entry.path().has_extension() && 
                                            std::find(
                                                file_extension_whitelist.begin(), 
                                                file_extension_whitelist.end(), 
                                                entry.path().extension().string()
                                            ) != file_extension_whitelist.end()
                                        )
            )
        {
            browse_path_children.push_back(entry.path());
        }
    }

    TraceLog(LOG_INFO, "FS Browser: Refreshed available paths. Current directory path: %s", browse_path.c_str());
}

void Filesystem_Browser::Refresh_Browse_Path_Children_Dir()
{
    browse_path_children.clear();
  
    for (const auto & entry : fs::directory_iterator(browse_path))
    {
        if (fs::is_directory(entry))
            browse_path_children.push_back(entry.path());
    }

    TraceLog(LOG_INFO, "FS Browser: Refreshed available paths (only dirs). Current directory path: %s", browse_path.c_str());
}

void Filesystem_Browser::Set_File_Whitelist(const std::vector<std::string>& whitelist)
{
    file_extension_whitelist.clear();
    file_extension_whitelist = whitelist;
}

bool Filesystem_Browser::Is_Dir_File_Dispatched()
{
    return dispatched_dir_file;
}

bool Filesystem_Browser::Is_Dir_Dispatched()
{
    return dispatched_dir;
}

