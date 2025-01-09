import json
import os
import re
from git import Repo, GitCommandError
import coloredlogs, logging
logger = logging.getLogger(__name__)
coloredlogs.install(level='INFO', logger=logger)
# Define paths to required files
vcpkg_json_path = os.path.join(os.path.dirname(__file__), "..", "..", "vcpkg.json")
cmakelists_path = os.path.join(os.path.dirname(__file__), "..", "..", "CMakeLists.txt")

def load_json(file_path):
    """Load a JSON file and return its content as a dictionary."""
    try:
        with open(file_path, 'r') as file:
            return json.load(file)
    except FileNotFoundError:
        logger.error(f"Error: {file_path} not found.")
        return None
    except json.JSONDecodeError:
        logger.error(f"Error: {file_path} is not a valid JSON file.")
        return None

def update_json(file_path, data):
    """Update a JSON file with new data."""
    try:
        with open(file_path, 'r+') as file:
            json_data = json.load(file)
            json_data.update(data)
            file.seek(0)
            json.dump(json_data, file, indent=4)
            file.truncate()
        logger.info(f"Updated {file_path} with new data.")
    except Exception as e:
        logger.error(f"Failed to update {file_path}: {e}")

def create_semver(vcpkg_data):
    """Create a SemVer string from available version fields in vcpkg data."""
    version = vcpkg_data.get("version")
    if version and re.match(r'^\d+\.\d+\.\d+$', version):
        return version
    semver = vcpkg_data.get("version-semver", "1.0.0").split(".")
    return '.'.join((semver + ["0", "0", "0"])[:3])

def get_git_tag(repo):
    """Return the latest tag associated with the current Git branch."""
    try:
        return repo.git.describe("--tags", "--abbrev=0")
    except GitCommandError:
        logger.warning("Warning: No tags found in repository.")
        return None

def collect_vcpkg_data(vcpkg_data_input, tag_version=True):
    """Collect environment data to write to config.cmake."""
    vcpkg_data = {}
    needed_keys = ["name", "version", "maintainers", "description", "documentation",
                   "homepage", "license", "version-semver", "version-string", "version-date"]

    for key in needed_keys:
        if key in vcpkg_data_input:
            vcpkg_data[f"MY_CMAKE_PROJECT_{key.upper()}"] = str(vcpkg_data_input[key])

    vcpkg_data["MY_CMAKE_PROJECT_SEMVER"] = create_semver(vcpkg_data_input)

    local_repo_path = os.path.join(os.path.dirname(__file__), "..", "..")
    try:
        local_repo = Repo(local_repo_path)
        if tag_version:
            tag_name = get_git_tag(local_repo)
            if tag_name:
                vcpkg_data["MY_CMAKE_PROJECT_VERSION"] = tag_name
                vcpkg_data["MY_CMAKE_PROJECT_SEMVER"] = tag_name
                update_json(vcpkg_json_path, {"version": tag_name})
            else:
                vcpkg_data["MY_CMAKE_PROJECT_VERSION"] = vcpkg_data["MY_CMAKE_PROJECT_SEMVER"]
        else:
            vcpkg_data["MY_CMAKE_PROJECT_VERSION"] = vcpkg_data["MY_CMAKE_PROJECT_SEMVER"]
    except Exception as e:
        logger.error("Error retrieving version:", e)
        vcpkg_data["MY_CMAKE_PROJECT_VERSION"] = vcpkg_data.get("MY_CMAKE_PROJECT_SEMVER", "1.0.0")

    return vcpkg_data

def write_config_cmake(vcpkg_data):
    """Write the collected environment data to config.cmake."""
    cmake_dir = os.path.join(os.path.dirname(__file__), "..", "..", "cmake")
    os.makedirs(cmake_dir, exist_ok=True)
    config_file_path = os.path.join(cmake_dir, "config.cmake")

    with open(config_file_path, 'w') as f:
        f.write("# config.cmake\n\n")
        for key, value in vcpkg_data.items():
            f.write(f"set({key} \"{value}\")\n")
            f.write(f"message(STATUS \"{key} set to: ${{{key}}}\")\n\n")
    logger.info(f"Configuration written to {config_file_path}")

def extract_dependencies(cmakelists_path_):
    """Extract dependency names from find_package calls in CMakeLists.txt."""
    dependencies = []
    find_package_pattern = re.compile(r'find_package\((\w+)')
    try:
        with open(cmakelists_path_, 'r') as file:
            dependencies = [match.group(1) for line in file if (match := find_package_pattern.search(line))]
    except FileNotFoundError:
        logger.error(f"Error: {cmakelists_path_} not found.")
    
    logger.info(f"Dependencies extracted: {dependencies}")
    return dependencies

def write_config_cmake_in(vcpkg_data, dependencies):
    """Create the Config.cmake.in file with template values."""
    target_name = vcpkg_data.get("MY_CMAKE_PROJECT_NAME", "UnknownProject")
    cmake_dir = os.path.join(os.path.dirname(__file__), "..", "..", "cmake")
    config_in_file_path = os.path.join(cmake_dir, "Config.cmake.in")

    os.makedirs(cmake_dir, exist_ok=True)
    with open(config_in_file_path, 'w') as f:
        f.write("# Config.cmake.in\n\n")
        f.write("@PACKAGE_INIT@\n\n")
        f.write(f"include(\"${{CMAKE_CURRENT_LIST_DIR}}/{target_name}Targets.cmake\")\n\n")
        f.write(f"check_required_components({target_name})\n\n")
        f.write("include(CMakeFindDependencyMacro)\n\n")
        for dep in dependencies:
            f.write(f"find_dependency({dep})\n")
    logger.info(f"Config.cmake.in written to {config_in_file_path}")

def main(tag_version=True, cmake_path=cmakelists_path):
    try:
        vcpkg_data = load_json(vcpkg_json_path)
        if not vcpkg_data:
            raise ValueError("vcpkg.json could not be loaded.")
        
        vcpkg_data = collect_vcpkg_data(vcpkg_data, tag_version)
        write_config_cmake(vcpkg_data)
        
        dependencies = extract_dependencies(cmake_path)
        write_config_cmake_in(vcpkg_data, dependencies)
        
        logger.info("Project configured successfully.")
    except Exception as e:
        logger.error(f"Error in configuration: {e}")

if __name__ == "__main__":
    main(True)
