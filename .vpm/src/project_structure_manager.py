import os
import shutil
import logging
import coloredlogs
from pathlib import Path
import project_utilities as utils
import git_manager as gm
import json
# Logging configuration
logger = logging.getLogger("CMakeInitializer")
class ProjectStructureManager:
    def __init__(self, repo_path,git_manager:gm.GitRepositoryManager):
        self.repo_path = repo_path
        self.git_manager=git_manager
        self.project_name = ""
        self.is_executable = False
        self.is_shared_lib = False
        self.is_qt_project = False
        with open(os.path.join(utils.ProjectUtilities.get_resources_path(), "platforms.json"),"r") as file:
            self.platforms = json.load(file)
        

    def configure(self,project_name, project_type,is_qt_project=False)->bool:
        """Configures the entire project structure."""
        self.is_qt_project = is_qt_project
        self.project_name=project_name
        if project_type =="exe":
            self.is_executable = True
            self.is_shared_lib = False
        elif project_type =="shared":
            self.is_executable = False
            self.is_shared_lib = True
        else:
            self.is_executable = False
            self.is_shared_lib = False
        if not self._check_existing_structure():
            self._create_directories()

        if self.is_qt_project and self.is_executable:
            self._create_qt_project_sources()
        elif self.is_executable:
            self._create_default_executable()
        elif self.is_shared_lib:
            self._create_shared_lib()
        else:
            self._create_static_lib()
        logger.info("Creating tests....")
        self._setup_test_files()
        logger.info("Setting up configuration files....")
        self._create_config_file()
        logger.info("Setting up CI/CD....")
        self._configure_ci_cd()
        self._add_license_and_contributing_files()
        self._configure_doxyfile()
        logger.info("Project structure configured successfully.")
        return True
    # Helper function to get the current script directory
    @staticmethod
    def _get_script_dir():
        """Returns the directory where the current script is located."""
        return Path(__file__).resolve().parent

    # Helper function to get a standardized project path
    def _get_project_path(self, *paths):
        """Returns a standardized path inside the project."""
        return Path(self.repo_path).joinpath(*paths)
    def _get_cmake_folder_project_path(self, *paths):
        """Returns a standardized path inside the project."""
        if not os.path.exists(Path(self.repo_path).joinpath("cmake")):
            os.makedirs(Path(self.repo_path).joinpath("cmake"))
        return Path(self.repo_path).joinpath("cmake", *paths)
    # Helper functions
    @staticmethod
    def _create_directory(path):
        """Creates a directory if it doesn't already exist."""
        os.makedirs(path, exist_ok=True)
        logger.debug(f"Directory '{path}' created.")

    @staticmethod
    def _is_empty_directory(path):
        """Checks if a directory is empty."""
        return not os.listdir(path)

    @staticmethod
    def _copy_file(src: Path, dest: Path) -> bool:
        """Copies a file if it exists."""
        src = Path(src)
        dest = Path(dest)
        try:
            if src.exists():
                shutil.copy(src, dest)
                logger.debug(f"Copied '{src}' to '{dest}'")
                return True
            else:
                logger.error(f"Template file '{src}' not found.")
                return False
        except Exception as e:
            logger.error(f"Failed to copy '{src}' to '{dest}': {e}")
            return False

    def _check_existing_structure(self):
        """Checks if the project structure already exists."""
        required_dirs = ["src", "include", "test"]
        if all(self._get_project_path(d).exists() for d in required_dirs):
            logger.warning("Project structure already exists.")
            return True
        return False

    def _create_directories(self):
        """Creates the main directories of the project."""
        for folder in ["src", "include", "test"]:
            self._create_directory(self._get_project_path(folder))
        logger.info("Project directories created.")

    def _create_qt_project_sources(self):
        """Sets up source files for a Qt project."""
        template_dir = utils.ProjectUtilities.get_template_path()
        
        if self._is_empty_directory(self._get_project_path("include")):
            self._copy_file(template_dir / "include" / "main.h.txt", 
                            self._get_project_path("include", "main.h"))
        
        if self._is_empty_directory(self._get_project_path("src")):
            self._copy_file(template_dir / "src" / "qt_main_template.cpp.txt", 
                            self._get_project_path("src", "main.cpp"))
        if not self._get_project_path("resources").exists():
            self._create_directory(self._get_project_path("resources"))
        if self._is_empty_directory(self._get_project_path("resources")):
            self._copy_file(template_dir / "resources/resources.qrc", 
                            self._get_project_path("resources/resources.qrc"))
        qml_path = self._get_project_path("resources", "qml")
        if not qml_path.exists():
            self._create_directory(qml_path)
            self._copy_file(template_dir / "resources/qml/main.qml", 
                            qml_path / "main.qml")



        logger.info("Qt project source files created.")

    def _create_default_executable(self):
        """Creates the main files for a standard C++ executable."""
        if self._is_empty_directory(self._get_project_path("src")):
            with open(self._get_project_path("src", "main.cpp"), "w") as main_file:
                main_file.write("#include <iostream>\n\nint main() {\n    std::cout << \"Hello, World!\" << std::endl;\n    return 0;\n}")
        if self._is_empty_directory(self._get_project_path("include")):
            with open(self._get_project_path("include", "main.h"), "w") as main_include:
                main_include.write("#pragma once\n")
        logger.info("Default executable main.cpp and main.h files created.")

    def _create_shared_lib(self):
        """Creates the main files for a shared library."""
        template_dir = utils.ProjectUtilities.get_template_path()
        
        if self._is_empty_directory(self._get_project_path("include")):
            self._copy_file(template_dir / "include" / "shared_template.h.txt", self._get_project_path("include", "shared_lib.h"))
        if self._is_empty_directory(self._get_project_path("src")):
            with open(self._get_project_path("src", "shared_lib.cpp"), "w") as lib_file:
                lib_file.write("#include <iostream>\n#include \"shared_lib.h\"\nvoid shared_lib_function() {\n    std::cout << \"Shared library function.\" << std::endl;\n}")
        logger.info("Shared library files created.")

    def _create_static_lib(self):
        """Creates the main files for a static library."""
        if self._is_empty_directory(self._get_project_path("include")):
            with open(self._get_project_path("include", "static_lib.h"), "w") as lib_file:
                lib_file.write("#pragma once\nvoid static_lib_function();")
        if self._is_empty_directory(self._get_project_path("src")):
            with open(self._get_project_path("src", "static_lib.cpp"), "w") as lib_file:
                lib_file.write("#include <iostream>\n#include \"static_lib.h\"\nvoid static_lib_function() {\n    std::cout << \"Static library function.\" << std::endl;\n}")
        logger.info("Static library files created.")

    def _setup_test_files(self):
        """Sets up the test files."""
        template_dir = utils.ProjectUtilities.get_template_path()
        if self._is_empty_directory(self._get_project_path("test")):

            self._copy_file(template_dir / "test" / "CMakeList_template.txt", self._get_project_path("test", "CMakeLists.txt"))

            with open(self._get_project_path("test", "test.cpp"), "w") as test_file:
                test_file.write("int main() {\n    return 0;\n}\n")
        logger.info("Test files created.")

    def _create_config_file(self):
        """Copies the configuration header file."""
        if os.path.exists(os.path.join(self.repo_path,"cmake","confi.h.in")):
            return
        template_dir = utils.ProjectUtilities.get_template_path()
        self._copy_file(template_dir / "config.h.in", self._get_cmake_folder_project_path("config.h.in"))
        logger.info("Config.h.in file created.")
    def _check_platforms(self)->bool:
        for platform in self.platforms:
            if not os.path.exists(os.path.join(self.repo_path, ".ci", f"{platform}.yml")):
                return False
    def _configure_ci_cd(self):
        """Sets up CI/CD files."""
        if os.path.exists(os.path.join(self.repo_path,".gitlab-ci.yml")) and self._check_platforms():
            return
        template_dir = utils.ProjectUtilities.get_template_path()
        ci_cd_template = ".gitlab-ci_template_library.yml" if not self.is_executable else ".gitlab-ci_template_standard.yml"
        if not self.is_executable:

            template_dir = utils.ProjectUtilities.get_template_path()
            ci_cd_template = ".gitlab-ci_template_library.yml"
            with open(template_dir / ci_cd_template, "r") as ci_cd_file:
                ci_cd_content = ci_cd_file.read()
                ci_cd_content = ci_cd_content.replace("<REPO_URL>", self.git_manager.get_repo_remote_url())
                with open(self._get_project_path(".gitlab-ci.yml"), "w") as ci_cd_file:
                    ci_cd_file.write(ci_cd_content)

        else:
            self._copy_file(template_dir / ci_cd_template, self._get_project_path(".gitlab-ci.yml"))
        
        if not os.path.exists(os.path.join(self.repo_path, ".ci")):
            os.makedirs(os.path.join(self.repo_path, ".ci"), exist_ok=True)

        template_dir = utils.ProjectUtilities.get_template_path()
        ci_template_dir = template_dir / ".ci"


        for platform in self.platforms:
            if not os.path.exists(os.path.join(self.repo_path, ".ci", f"{platform}.yml")):
                self._copy_file(ci_template_dir / f"{platform}.yml", self._get_project_path(".ci", f"{platform}.yml"))

        logger.info("CI/CD configured.")

    def _configure_doxyfile(self):
        """Configures the Doxyfile for documentation."""
        if os.path.exists(os.path.join(self.repo_path,"Doxyfile")):
            return
        template_dir = utils.ProjectUtilities.get_template_path()
        with open(template_dir / "Doxyfile_template", "r") as doxyfile:
            doxyfile_content = doxyfile.read()
            doxyfile_content = doxyfile_content.replace("<PROJECT_NAME>", self.project_name)
            with open(self._get_project_path("Doxyfile"), "w") as doxyfile:
                doxyfile.write(doxyfile_content)
                logger.info("Doxyfile configured.")


    def _add_license_and_contributing_files(self):
        """Adds LICENSE and CONTRIBUTING files."""
        template_dir = utils.ProjectUtilities.get_template_path()

        license_path = self._get_project_path("LICENSE")
        contributing_path = self._get_project_path("CONTRIBUTING.md")

        if not license_path.exists():
            if not self._copy_file(template_dir / "LICENSE", license_path):
                logger.warning("Failed to add LICENSE file.")
            else:
                logger.info("LICENSE file added.")

        if not contributing_path.exists():
            if not self._copy_file(template_dir / "CONTRIBUTING.md", contributing_path):
                logger.warning("Failed to add CONTRIBUTING.md file.")
            else:
                logger.info("CONTRIBUTING.md file added.")
        
