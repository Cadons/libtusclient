import os
import shutil
import json
import logging
import platform
import subprocess
from pathlib import Path
import vcpkg_manager as vcpkg
import project_utilities as utils

# Logging configuration
logger = logging.getLogger("CMakeInitializer")

class CMakePresetManager:
    def __init__(self, repo_path,vcpkg_manager:vcpkg.VcpkgManager):
        self.repo_path = repo_path
        self.vcpkg_manager=vcpkg_manager

    @staticmethod
    def get_script_dir():
        """Returns the directory where the current script is located."""
        return Path(__file__).resolve().parent

    def configure_cmake_presets(self):
        """Sets up CMakePresets.json."""
        logger.info("Configuring CMakePresets...")
        preset_path = Path(self.repo_path) / "CMakePresets.json"
        if not preset_path.exists():
            template_path = utils.ProjectUtilities.get_template_path()/ "CMakePresets_template.json"
            if template_path.exists():
                shutil.copy(template_path, preset_path)
                logger.info("CMakePresets.json file created.")
                return True
            else:
                logger.error(f"Template file '{template_path}' not found.")
                return False
        logger.info("CMakePresets.json already exists.")
        return True
    @staticmethod
    def detect_generators():
        """Detects available generators based on platform and installed tools."""
        system_platform = platform.system()
        generators = {}

        if system_platform == "Windows":
            generators["Ninja"] = shutil.which("ninja") is not None
            vswhere_path = utils.ProjectUtilities.get_resources_path()  / "vswhere.exe"
            if vswhere_path.exists():
                try:
                        # Run vswhere to find the installed Visual Studio versions
                    result = subprocess.run([vswhere_path, "-latest", "-products", "*", "-requires", "Microsoft.Component.MSBuild", "-property", "displayName"],
                                            stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
                    if result.returncode == 0:
                            vs_version = result.stdout.strip().split(" ")[-1]
                        
                            # Extract Visual Studio version from the output
                            if vs_version == "2022":
                                vs_version = "17 " + vs_version
                            elif vs_version == "2019":
                                vs_version = "16 " + vs_version
                            elif vs_version == "2017":
                                vs_version = "15 " + vs_version
                            elif vs_version == "2015":
                                vs_version = "14 " + vs_version
                            else:
                                logger.warning(f"Unknown Visual Studio version: {vs_version}")
                                vs_version = None
                            
                            if vs_version:
                                generators[f"Visual Studio {vs_version}"] = True  # Add the detected version
                            else:
                                logger.error("No Visual Studio installation found.")
                            
                    else:
                        raise Exception(f"Error detecting Visual Studio version: {result.stderr}")
                except Exception as e:
                    logger.error(f"Error detecting Visual Studio version: {e}")
            else:
                logger.error("vswhere tool not found. Make sure Visual Studio is installed.")
        else:
            generators["Ninja"] = shutil.which("ninja") is not None
            generators["Unix Makefiles"] = shutil.which("make") is not None
            if system_platform == "Darwin":
                generators["Xcode"] = shutil.which("xcodebuild") is not None

        return {gen: available for gen, available in generators.items() if available}

    def configure_cmake_user_presets(self, generator_to_use=""):
        """Configures CMakeUserPresets.json based on available generators and vcpkg."""
        logger.info("Configuring CMakeUserPresets.json...")
        output_path = Path(self.repo_path) / "CMakeUserPresets.json"
        vcpkg_path = vcpkg.VcpkgManager.get_vcpkg_path()
        vcpkg_directory = os.path.dirname(vcpkg_path).replace("\\", "/")
        generators = self.detect_generators()

        selected_generator = generator_to_use or next(iter(generators), "")
        if selected_generator not in generators:
            logger.error("Invalid generator selected.")
            return False

        presets = {
            "version": 3,
            "configurePresets": [],
            "buildPresets": []
        }
        
        for generator in generators:
            is_selected = generator == selected_generator
            configure_preset = {
                "name": generator,
                "hidden": not is_selected,
                "generator": generator,
                "binaryDir": "${sourceDir}/build/",
                "inherits": "vcpkg",
                "cacheVariables": {
                    "CMAKE_TOOLCHAIN_FILE": f"{vcpkg_directory}/scripts/buildsystems/vcpkg.cmake",
                }
            }
            presets["configurePresets"].append(configure_preset)
            
            if is_selected:
                presets["buildPresets"].extend([
                    {
                        "name": "Debug",
                        "configurePreset": generator,
                        "configuration": "Debug"
                    },
                    {
                        "name": "Release",
                        "configurePreset": generator,
                        "configuration": "Release"
                    }
                ])

        with open(output_path, "w") as outfile:
            json.dump(presets, outfile, indent=4)
        self.vcpkg_manager.setup_vcpkg_environment()
        logger.info("CMakeUserPresets.json configured successfully.")
        return True

