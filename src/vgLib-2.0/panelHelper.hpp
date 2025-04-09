// Modified from https://community.vcvrack.com/t/death-to-helper-py-long-live-svghelper/19427
// Assumes that panel control placement is on the light (default) SVG.

#pragma once

#include <functional>
#include <string>
#include <regex>
#include <rack.hpp>
#include <unordered_map>
#include <algorithm>
#include "../../../voxglitch_positions.hpp"

struct PanelHelper
{
    rack::ModuleWidget* m_moduleWidget;
    std::shared_ptr<rack::Svg> m_svg;
    std::string m_moduleName;

    struct NamedPosition {
        std::string name;
        Vec position;
    };

    PanelHelper(rack::ModuleWidget* moduleWidget)
        : m_moduleWidget(moduleWidget), m_svg(nullptr) {}

    // Loads the panel from the given filename
    // Use this if you are using only the light version of the panel
    void loadPanel(const std::string& filename)
    {
        if (!m_svg) {
            auto panel = rack::createPanel(filename);
            m_svg = panel->svg;
            m_moduleWidget->setPanel(panel);

            #ifdef METAMODULE
            std::string moduleName = filename;
            // get only the panel name from Module name: VoxGlitch/autobreak/autobreak_panel.png
            size_t slashPos = moduleName.find_last_of("/");
            if (slashPos != std::string::npos) {
                moduleName = moduleName.substr(slashPos + 1);
            }
            
            size_t pngPos = moduleName.find(".png");
            if (pngPos != std::string::npos) {
                moduleName = moduleName.substr(0, pngPos);
            }
            m_moduleName = moduleName;
            #endif
        }
    }

    // Loads the panel from the given filenames
    // Use this if you are using both the light and dark versions of the panel

    void loadPanel(const std::string& filename1, const std::string& filename2)
    {
        if (!m_svg) {
            ThemedSvgPanel *panel = rack::createPanel(filename1, filename2);
            m_svg = panel->lightSvg;
            m_moduleWidget->setPanel(panel);

            #ifdef METAMODULE
            std::string moduleName = filename1;
            // get only the panel name from Module name: VoxGlitch/autobreak/autobreak_panel.png
            size_t slashPos = moduleName.find_last_of("/");
            if (slashPos != std::string::npos) {
                moduleName = moduleName.substr(slashPos + 1);
            }
            
            size_t pngPos = moduleName.find(".png");
            if (pngPos != std::string::npos) {
                moduleName = moduleName.substr(0, pngPos);
            }
            
            m_moduleName = moduleName;
            #endif
        }
    }

    // Finds the position of a named control
    // Returns the center of the control's bounding box
    // If the control isn't found, returns position from VoxglitchPositions if available, otherwise Vec(0, 0)

    Vec findNamed(const std::string& name)
    {
        Vec result;
        bool found = false;
        forEachShape([&](NSVGshape* shape) {
            if (std::string(shape->id) == name) {
                result = getBoundsCenter(shape->bounds);
                found = true;
            }
        });
        
        // If not found in SVG, check VoxglitchPositions
        if (!found) {
            std::string panelName = m_moduleName + "_" + name;

            printf("panelName: %s\n", panelName.c_str());
            
            if (!m_moduleName.empty()) {
                // Search in the flattened map
                for (const auto& pair : VoxglitchPositions::modules) {
                    if (pair.first == panelName) {
                        printf("found: %s (%.3f, %.3f)\n", pair.first.c_str(), pair.second.x, pair.second.y);
                        return pair.second;
                    }
                }
            }
        }
        
        return result;
    }
    
    // Load module name for direct position lookups
    void loadModulePositions(const std::string& moduleName) {
        std::string lowerModuleName = moduleName;
        std::transform(lowerModuleName.begin(), lowerModuleName.end(), lowerModuleName.begin(),
                     [](unsigned char c){ return std::tolower(c); });
        m_moduleName = lowerModuleName;
    }

    std::vector<NamedPosition> findPrefixed(const std::string& prefix)
    {
        std::vector<NamedPosition> result;
        // First check SVG
        forEachShape([&](NSVGshape* shape) {
            std::string id(shape->id);
            if (id.compare(0, prefix.length(), prefix) == 0) {
                result.push_back({id, getBoundsCenter(shape->bounds)});
            }
        });
        
        // Then check VoxglitchPositions
        if (!m_moduleName.empty()) {
            for (const auto& pair : VoxglitchPositions::modules) {
                std::string id(pair.first);
                // Check if this entry belongs to the current module
                if (id.compare(0, m_moduleName.length(), m_moduleName) == 0) {
                    // Extract the part after the module name to check for prefix
                    std::string componentId = id.substr(m_moduleName.length());
                    
                    // Check if the component ID starts with the prefix
                    if (componentId.compare(0, prefix.length(), prefix) == 0) {
                        // Only add if not already in the result
                        bool found = false;
                        for (const auto& pos : result) {
                            if (pos.name == pair.first) {
                                found = true;
                                break;
                            }
                        }
                        if (!found) {
                            result.push_back({pair.first, pair.second});
                        }
                    }
                }
            }
        }
        
        return result;
    }

    std::vector<NamedPosition> findMatched(const std::string& pattern)
    {
        std::vector<NamedPosition> result;
        std::regex regex(pattern);
        
        // First check SVG
        forEachShape([&](NSVGshape* shape) {
            std::string id(shape->id);
            std::smatch match;
            if (std::regex_search(id, match, regex)) {
                result.push_back({id, getBoundsCenter(shape->bounds)});
            }
        });
        
        // Then check VoxglitchPositions
        if (!m_moduleName.empty()) {
            for (const auto& pair : VoxglitchPositions::modules) {
                std::string id(pair.first);
                // Check if this entry belongs to the current module
                if (id.compare(0, m_moduleName.length(), m_moduleName) == 0) {
                    std::smatch match;
                    if (std::regex_search(pair.first, match, regex)) {
                        // Only add if not already in the result
                        bool found = false;
                        for (const auto& pos : result) {
                            if (pos.name == pair.first) {
                                found = true;
                                break;
                            }
                        }
                        if (!found) {
                            result.push_back({pair.first, pair.second});
                        }
                    }
                }
            }
        }
        
        return result;
    }

    void forEachPrefixed(const std::string& prefix, const std::function<void(unsigned int, const Vec&)>& callback)
    {
        auto positions = findPrefixed(prefix);
        for (unsigned int i = 0; i < positions.size(); ++i) {
            callback(i, positions[i].position);
        }
    }

    void forEachMatched(const std::string& regex, const std::function<void(const std::vector<std::string>&, const Vec&)>& callback)
    {
        std::regex re(regex);
        
        // First process SVG shapes
        forEachShape([&](NSVGshape* shape) {
            std::string id(shape->id);
            std::smatch match;
            if (std::regex_search(id, match, re)) {
                std::vector<std::string> captures;
                for (size_t i = 1; i < match.size(); ++i) {
                    captures.push_back(match[i].str());
                }
                callback(captures, getBoundsCenter(shape->bounds));
            }
        });
        
        // Then process VoxglitchPositions
        if (!m_moduleName.empty()) {
            for (const auto& pair : VoxglitchPositions::modules) {
                std::string id(pair.first);
                // Check if this entry belongs to the current module
                if (id.compare(0, m_moduleName.length(), m_moduleName) == 0) {
                    std::smatch match;
                    if (std::regex_search(pair.first, match, re)) {
                        std::vector<std::string> captures;
                        for (size_t i = 1; i < match.size(); ++i) {
                            captures.push_back(match[i].str());
                        }
                        callback(captures, pair.second);
                    }
                }
            }
        }
    }

private:
    void forEachShape(const std::function<void(NSVGshape*)>& callback)
    {
        if (!m_svg || !m_svg->handle) return;

        #ifndef METAMODULE
        for (NSVGshape* shape = m_svg->handle->shapes; shape != nullptr; shape = shape->next) {
            callback(shape);
        }
        #endif
    }

    Vec getBoundsCenter(float* bounds)
    {
        return Vec((bounds[0] + bounds[2]) / 2, (bounds[1] + bounds[3]) / 2);
    }
};