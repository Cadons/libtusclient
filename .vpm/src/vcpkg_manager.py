import os
import subprocess
import json
import logging
import platform
import coloredlogs
import re
# Set up logger with colored output for better visibility
logger = logging.getLogger("CMakeInitializer")
class VcpkgManager:
    def __init__(self, repo_path):
        """
        Initializes the VcpkgManager instance with the provided paths.
        
        Args:
            vcpkg_path (str): Path to the VCPKG executable.
            repo_path (str): Path to the repository.
            vcpkg_root_path (str): Path to the VCPKG installation directory.
        """
        self.vcpkg_path = self.get_vcpkg_path()
        self.repo_path = repo_path
        self.vcpkg_root_path = VcpkgManager.get_vcpkg_path()

    def configure_vcpkg(self):
        """
        Configures the VCPKG project by running the appropriate VCPKG command to initialize it.
        
        Returns:
            bool: True if the project was successfully configured, False otherwise.
        """
        logger.info("Configuring VCPKG project...")

        os.chdir(self.repo_path)

        # Check if vcpkg.json already exists
        if not os.path.exists("vcpkg.json"):
            # If it doesn't exist, create a new VCPKG project
            command = [self.vcpkg_path, 'new', "--application"]
            try:
                subprocess.run(command, check=True)
                logger.info("VCPKG project configured.")
            except subprocess.CalledProcessError as e:
                logger.error(f"An error occurred while configuring VCPKG: {e}")
                return False

        return True

    def create_vcpkg_json(self, project_name):
        """
        Creates or updates the `vcpkg.json` file with the given project name and version.
        If the file exists, it updates the name and version without overriding other properties.

        Args:
            project_name (str): The name of the project.
            version (str): The version of the project (default is "1.0.0").
        """
        vcpkg_file_path = os.path.join(self.repo_path, "vcpkg.json")

        # Initialize an empty dictionary for JSON content
        vcpkg_data = {}

        # If vcpkg.json exists, load its content
        if os.path.exists(vcpkg_file_path):
            with open(vcpkg_file_path, "r") as vcpkg_file:
                try:
                    vcpkg_data = json.load(vcpkg_file)
                    logger.info("Loaded existing vcpkg.json content.")
                except json.JSONDecodeError:
                    logger.warning("Invalid JSON format in vcpkg.json. Proceeding with an empty template.")

        # Update the name and version
        vcpkg_data["name"] = project_name
        if vcpkg_data.get("version") is None:
            vcpkg_data["version"] = "1.0.0"
        if vcpkg_data.get("dependencies") is None:
            vcpkg_data["dependencies"] = []
        # Write the updated content back to vcpkg.json
        with open(vcpkg_file_path, "w") as vcpkg_file:
            json.dump(vcpkg_data, vcpkg_file, indent=4)
        
        logger.info("vcpkg.json file created or updated with project name and version.")

    def setup_vcpkg_environment(self):
        """
        Sets up environment variables for VCPKG by configuring VCPKG_ROOT and updating PATH.
        """
        try:
            # Set the VCPKG_ROOT environment variable
            os.environ["VCPKG_ROOT"] = self.vcpkg_root_path
            logger.debug(f"Set VCPKG_ROOT to: {self.vcpkg_root_path}")

            # Modify the PATH environment variable to include VCPKG_ROOT
            os.environ["PATH"] = f"{self.vcpkg_root_path};{os.environ.get('PATH', '')}"
            logger.debug(f"Updated PATH to include VCPKG_ROOT.")
        except Exception as e:
            logger.critical(f"An error occurred while setting environment variables: {e}")

    @staticmethod
    def get_vcpkg_path():
        """
        Attempts to find the VCPKG executable path based on the platform using either "where" (Windows) or "which" (Linux/Mac).
        
        Returns:
            str: Path to the vcpkg executable, or None if not found.
        """
        system_platform = platform.system()
        logger.debug(f"Detected system platform: {system_platform}")
        command = "where" if system_platform == "Windows" else "which"
        logger.debug(f"Using command: {command}")

        try:
            # Run the command to find vcpkg
            result = subprocess.run([command, "vcpkg"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
            logger.debug(f"Command output: {result.stdout.strip()}")

            if result.returncode == 0:
                vcpkg_path = result.stdout.strip().splitlines()[0]
                return vcpkg_path
            else:
                raise FileNotFoundError("vcpkg not found in the system's PATH.")
        except Exception as e:
            logger.critical(f"An error occurred while finding vcpkg: {e}")
            return None
    @staticmethod
    def is_valid_package_name(name):
        """Check if the package name is valid according to vcpkg rules."""
        # Check for lowercase alphanumeric characters and hyphens only
        return bool(re.match(r'^[a-z0-9-]+$', name))
    def add_dependency(self, dependency_name):
        """
        Adds a dependency to the vcpkg.json file.

        Args:
            dependency_name (str): The name of the dependency to add.
        """
        # Read the existing vcpkg.json file
        with open(os.path.join(self.repo_path, "vcpkg.json"), "r") as vcpkg_file:
            vcpkg_json_content = json.load(vcpkg_file)

        # Check if the dependency already exists
        if dependency_name not in vcpkg_json_content["dependencies"]:
            # Add the dependency to the "dependencies" array
            vcpkg_json_content["dependencies"].append(dependency_name)

            # Write the updated content back to the vcpkg.json file
            with open(os.path.join(self.repo_path, "vcpkg.json"), "w") as vcpkg_file:
                json.dump(vcpkg_json_content, vcpkg_file, indent=4)

            logger.info(f"Added '{dependency_name}' to vcpkg.json.")
        else:
            logger.warning(f"'{dependency_name}' already exists in vcpkg.json.")
    def add_vcpkg_library_dependencies(self):
        self.add_dependency("vcpkg-cmake")
        cmake_config = {
            "name": "vcpkg-cmake-config",
            "host": True
        }
        self.add_dependency(cmake_config)
        

