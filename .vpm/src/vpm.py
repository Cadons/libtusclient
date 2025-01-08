import argparse
from pathlib import Path
import platform
import sys
import project_manager as pm
import git_manager as gm
import vcpkg_manager as vm
import cmake_presets_manager as cpm
import coloredlogs, logging
import os
import project_utilities as utils

logger = logging.getLogger("CMakeInitializer")
coloredlogs.install(level='INFO', logger=logger)


def parse_arguments():
    """Parse command line arguments."""
    parser = argparse.ArgumentParser(description="Configure and create a CMake project.")
    parser.add_argument("--name", help="The name of the project", type=str, default=None)
    parser.add_argument("--type", help="The type of the project (exe, shared, static)", type=str, default=None)
    parser.add_argument("--isQt", help="Whether the project is a Qt project (True or False)", type=bool, default=None)
    parser.add_argument("--generator", help="The generator to use for CMake", type=str, default="")
    parser.add_argument("--reset", help="Reset the folder", action="store_true")
    return parser.parse_args()


def display_environment_info(repo_path):
    """Display environment information."""
    system_platform = platform.system()
    architecture = platform.architecture()[0]
    logger.info("-----------------ENVIROMENT INFO----------------")
    logger.info(f"Detected system platform: {system_platform}")
    logger.info(f"Architecture: {architecture}")
    logger.info(f"Repo path: {repo_path}")
    logger.info(f"Vcpkg path: {vm.VcpkgManager.get_vcpkg_path()}")
    logger.info("------------------------------------------------")


def reset_cache_if_requested():
    """Reset cache if the reset argument is provided."""
    cache_path = utils.ProjectUtilities.get_cache_path()
    config_path = os.path.join(cache_path, "config.json")
    if os.path.exists(config_path):
        os.remove(config_path)
        logger.info("Cache reset successfully.")


def configure_tools_and_vcpkg(git_manager, vcpkg_manager):
    """Configure tools repository and VCPKG."""
    if not git_manager.configure_tools_repo():
        raise Exception("Error configuring the tools repository.")
    if not vcpkg_manager.configure_vcpkg():
        raise Exception("Error configuring VCPKG.")

def requested_project_data_to_user(project_manager: pm.CMakeProjectManager):
    """
    Prompts the user for the project name, type, and whether it is a Qt project.
    """
    # Prompt the user for a valid project name
    while True:
        logger.info("Enter a valid project name:")
        project_name = input().strip()

        # Check if the project name is valid and not reserved
        if project_name not in project_manager.get_reserved_names() and project_manager.is_valid_package_name(project_name):
            break
        elif project_name in project_manager.get_reserved_names():
            logger.warning(f"The name '{project_name}' is reserved. Please choose another name.")
        else:
            logger.error("Invalid name. Package names must be lowercase alphanumeric characters and hyphens.")
    
    # Mapping for artifact type selection
    artifact_type_map = {"1": "exe", "2": "shared", "3": "static"}
    
    # Loop until a valid artifact type is chosen
    while True:
        logger.info("Select artifact type:\n1) Executable\n2) Dynamic Library\n3) Static Library")
        artifact_type_selected = input().strip()

        if artifact_type_selected in artifact_type_map:
            selected_artifact_type = artifact_type_map[artifact_type_selected]
            logger.info(f"Selected artifact type: {selected_artifact_type}")
            break
        else:
            logger.error("Invalid selection. Please enter 1, 2, or 3.")

    # Loop to confirm if the project is a Qt project
    while True:
        logger.info("Is this a Qt project? (y/n): ")
        is_qt_project_input = input().strip().lower()

        if is_qt_project_input == "y":
            is_qt_project = True
            logger.info("Qt project selected.")
            break
        elif is_qt_project_input == "n":
            is_qt_project = False
            logger.info("Non-Qt project selected.")
            break
        else:
            logger.error("Invalid input. Please enter 'y' for yes or 'n' for no.")

    return {"name": project_name, "type": selected_artifact_type, "isQt": is_qt_project}
def request_generator(cmake_preset_manager: cpm.CMakePresetManager):
    """
    Prompts the user to select a CMake generator from the available options.
    """
    logger.info("Available generators:")
    generators = cmake_preset_manager.detect_generators()

    # Check if no generators are detected
    if not generators:
        raise Exception("No generators found. Please install a CMake generator.")

    # Display the list of available generators
    for idx, generator in enumerate(generators, start=1):
        logger.info(f"{idx}) {generator}")

    # Loop until a valid generator is selected
    while True:
        try:
            logger.info(f"Select a generator (1-{len(generators)}): ")
            selected_generator_index = int(input().strip()) - 1

            # Validate the selected index
            if 0 <= selected_generator_index < len(generators):
                selected_generator = list(generators.keys())[selected_generator_index]
                logger.info(f"Selected generator: {selected_generator}")
                return selected_generator
            else:
                logger.error(f"Invalid selection. Please choose a number between 1 and {len(generators)}.")
        except ValueError:
            logger.error("Invalid input. Please enter a valid number.")

def request_project_data(project_manager):
    """Request project data from the user."""
    input_data = requested_project_data_to_user(project_manager)
    return input_data["name"], input_data["type"], input_data["isQt"]


def configure_project(project_manager: pm.CMakeProjectManager, input_data=None):
    """Configure the CMake project."""
    if input_data:
        return project_manager.configure_with_inputs(*input_data)
    return project_manager.configure()


def handle_project_creation(project_manager: pm.CMakeProjectManager, is_project_ready: bool) -> bool:
    """Handle project creation based on its initialization state."""

    if is_project_ready:
        logger.info("Project already configured.")
        return True

    cmake_lists_path = "CMakeLists.txt"
    vcpkg_json_path = "vcpkg.json"
    config_json_path = os.path.join(utils.ProjectUtilities.get_cache_path(), "config.json")

    if os.path.exists(cmake_lists_path) and os.path.exists(vcpkg_json_path) and project_manager.check_config_json():
        logger.info("Checking existing project configuration.")
        return configure_project(project_manager)

    if not os.path.exists(cmake_lists_path) and os.path.exists(config_json_path)and project_manager.check_config_json():
        logger.info("Using cached project configuration.")
        return configure_project(project_manager)

    logger.info("Requesting project data from user.")
    input_data = request_project_data(project_manager)
    
    return configure_project(project_manager, input_data)


def main():
    repo_path = Path(__file__).resolve().parent.parent.parent
    logger.info(pm.CMakeProjectManager.get_logo())

    args = parse_arguments()
    
    if args.reset:
        reset_cache_if_requested()
    
    # Setup environment and managers
    display_environment_info(repo_path)
    git_manager = gm.GitRepositoryManager(repo_path)
    vcpkg_manager = vm.VcpkgManager(repo_path)
    project_manager = pm.CMakeProjectManager(repo_path, args.name, args.type, args.isQt)
    
    try:
        is_project_ready = project_manager.is_project_initialized()
        
        # Step 1: Configure tools and VCPKG
        configure_tools_and_vcpkg(git_manager, vcpkg_manager)

        # Step 2: Handle project creation
        if not handle_project_creation(project_manager, is_project_ready):
            raise Exception("Project creation failed.")

        # Step 3: Configure CMake generator
        cmake_preset_manager = project_manager.get_cmake_presets_manager()
        generator = args.generator or request_generator(cmake_preset_manager)

        # Step 4: Configure automations or update presets
        if not is_project_ready:
            if not project_manager.configure_automations(generator):
                raise Exception("Error configuring project automations.")
            logger.info("Project created successfully.")
        else:
            cmake_preset_manager.configure_cmake_user_presets(generator)
            logger.info("Project updated for local configuration.")

    except KeyboardInterrupt:
        logger.warning("Process interrupted. Cleaning up before exiting...")
        if not project_manager.is_project_initialized():
            project_manager.reset()
            logger.info("Cleanup complete.")
        sys.exit(1)

    except Exception as e:
        project_manager.reset()
        logger.error(f"Error: {str(e)}")
        sys.exit(1)


if __name__ == "__main__":
    main()
