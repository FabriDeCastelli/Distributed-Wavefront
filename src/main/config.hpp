//
// Created by Fabrizio De Castelli on 22/08/24.
//

#ifndef CONFIG_H
#define CONFIG_H

#include <filesystem>

namespace fs = std::filesystem;

namespace Config {

    inline fs::path current_path = fs::current_path().root_path();
    inline fs::path dir("/home/f.decastelli/Distributed-Wavefront/outputs/");
    const fs::path OUTPUTS_DIRECTORY = current_path / dir;


}

#endif //CONFIG_H