import json
import os
import logging
import shutil
import coloredlogs
import re
import git_manager as gm
import project_structure_manager as ps
import vcpkg_manager as vm
import cmake_presets_manager as cpm
import project_utilities as utils
import sys
from pathlib import Path
sys.path.append(str(Path(__file__).resolve().parent.parent.parent / "cmake_tools_for_vcpkg/tools"))
from setup import main as cmake_tools # type: ignore
# Set up logger with colored output
logger = logging.getLogger("CMakeInitializer")

class CMakeProjectManager:

    def __init__(self, repo_path:Path, project_name:str, project_type:str, is_qt_project:bool):
        self.repo_path = repo_path
        self.project_name = project_name
        self.project_type = project_type
        self.is_qt_project = is_qt_project
        self.file_path=os.path.dirname(__file__)
        self.template_path = utils.ProjectUtilities.get_template_path()
        self.resources_path= utils.ProjectUtilities.get_resources_path()
        self.vcpkg_manager = vm.VcpkgManager(repo_path)
        self.git_manager = gm.GitRepositoryManager(repo_path)
        self.config_bkp={}
        self.already_created=False
        self.structure_manager= ps.ProjectStructureManager(repo_path,self.git_manager)
        self.cmake_presets_manager=cpm.CMakePresetManager(repo_path,self.vcpkg_manager)

    @staticmethod
    def is_valid_package_name(name)->bool:
        """Check if the package name is valid according to vcpkg rules."""
        # Check for lowercase alphanumeric characters and hyphens only
        return bool(re.match(r'^[a-z0-9-]+$', name))
    def is_executable(self)->bool:
        return self.project_type=="exe"
    def is_shared_library(self)->bool:
        return self.project_type=="shared"
    def is_static_library(self)->bool:
        return self.project_type=="static"
    def configure_cmake(self)->bool:
        if os.path.exists(self.repo_path/"CMakeLists.txt"):
            #check project name in the file
            with open(self.repo_path/"CMakeLists.txt", "r") as cmake_file:
                content = cmake_file.read()
                if f"project({self.project_name}" in content:
                    logger.warning("Project already configured.")
                    self.already_created=True
                    return True
                else:
                    logger.info("Project not configured. Configuring...")
                    self.already_created=False
        else:
            logger.info("CMakeLists.txt not found. Configuring...")
        template_path = os.path.join(self.template_path, "CMakeList_template.txt")
        if not os.path.exists(template_path):
            logger.error(f"Template file '{template_path}' not found.")
            return False
        with open(template_path, "r") as template_file:
            template_content = template_file.read()
            
        is_qt_project_str=""
        if self.is_qt_project:
            is_qt_project_str="true"
        else:
            is_qt_project_str="false"
        self.project_types = {
            "exe": f"create_executable(\"{self.project_name}\" {is_qt_project_str})",
            "shared": f"create_shared_library(\"{self.project_name}\" {is_qt_project_str})",
            "static": f"create_static_library(\"{self.project_name}\" {is_qt_project_str})"
        }
        
        if self.project_type not in self.project_types:
            logger.error("Invalid artifact type selected.")
            return False
        self.configure_vcpkg()
        template_content=self._configure_qt(self.is_qt_project, template_content)
        template_content = template_content.replace("<PROJECT_NAME>", self.project_name)
        template_content = template_content.replace("<ARTIFACT_TYPE>", self.project_types[self.project_type])

        if self.is_executable():
            template_content = template_content.replace("<TEST_LIB>", f"create_static_library(\"{self.project_name}_lib\" {is_qt_project_str})")
            template_content = template_content.replace("<PROJECT_LIB>",f"set(PROJECT_LIB \"{self.project_name}_lib\")")
        else:
            template_content = template_content.replace("<TEST_LIB>", "")
            template_content = template_content.replace("<PROJECT_LIB>", f"set(PROJECT_LIB \"{self.project_name}\")")
        #set LOGO
        logger.info("Adding logo to the project...")
        logo_content=self.get_logo()
        #put # at the beginning of each line
        logo_content = "# " + logo_content.replace("\n", "\n# ")
        template_content = template_content.replace("<LOGO>",logo_content)
        # Write final CMake content
        with open("CMakeLists.txt", "w") as cmake_file:
            cmake_file.write(template_content)
        logger.info("CMakeLists.txt configured.")
        return True
    
    def configure_vcpkg(self)->bool:
        if os.path.exists(self.repo_path/"vcpkg.json"):
            self.vcpkg_manager.create_vcpkg_json(self.project_name)
            logger.info("vcpkg.json file configured....")
        return True
    def write_project_configuration(self):
        #write in .vpm/.cache the file config.json with the name, project type and isqt
        project_config = {
            "name": self.project_name,
            "type": self.project_type,
            "is_qt_project": self.is_qt_project
        }
        if not os.path.exists(os.path.join(self.repo_path, ".vpm", ".cache")):
            os.makedirs(os.path.join(self.repo_path, ".vpm", ".cache"), exist_ok=True)
        with open(os.path.join(self.repo_path, ".vpm", ".cache", "config.json"), "w") as config_file:
            json.dump(project_config, config_file, indent=4)
        logger.info("Project configuration written.")
        
    def load_project_configuration(self):
        config_path = os.path.join(self.repo_path, ".vpm", ".cache", "config.json")
        self.already_created=False
        if os.path.exists(config_path):
            with open(config_path, "r") as config_file:
                config = json.load(config_file)
            self.config_bkp=config
            self.project_name = config.get("name", "")
            self.project_type = config.get("type", "")
            self.is_qt_project = config.get("is_qt_project", False)
            self.already_created=True
            logger.info("Project configuration loaded.")
    

    def configure(self)->bool:
        logger.info("Configuring CMakeLists.txt...")
        
        self.load_project_configuration()

        if self.is_project_initialized():
            logger.info("Project already initialized.")
            return True
        os.chdir(self.repo_path)
        if (not self.already_created and not self.is_project_initialized()) or not os.path.exists(self.repo_path/"CMakeLists.txt") or os.path.exists(self.repo_path/"vcpkg.json"):
            self.configure_vcpkg()
            if self.project_type !="exe":
                self.vcpkg_manager.add_vcpkg_library_dependencies()
            self.configure_cmake()
            self.write_project_configuration()
        
        if not self.is_project_initialized():
            if not self.structure_manager.configure(self.project_name,self.project_type,self.is_qt_project):
                raise Exception("Project structure configuration failed")
            
        # Run the CMakeTools setup
        logger.info("Updating project configuration....")
        cmake_tools(False, "CMakeLists.txt")
        return True
    
    def get_cmake_presets_manager(self):
        return self.cmake_presets_manager
    def configure_with_inputs(self, name, type, is_qt_project)->bool:
        self.project_name = name
        self.project_type = type
        self.is_qt_project=is_qt_project
        return self.configure()
    def _configure_qt(self, is_qt_project,template_content)->str:
        """Configure the qt project"""

        if is_qt_project and self.is_executable():
            qt_template_path = os.path.join(self.template_path, "QT_PROJECT_TEMPLATE_EXE.txt")
            if not os.path.exists(qt_template_path):
                logger.error(f"Qt template file '{qt_template_path}' not found.")
                return False
            with open(qt_template_path, "r") as qt_template_file:
                qt_template_content = qt_template_file.read()
            template_content = template_content.replace("<QT_PROJECT_SECTION>", qt_template_content)
            template_content = template_content.replace("set(PROJECT_LIBRARIES \"\")", "")
            #update vcpkg.json with the qtbase, qtdeclarative and qttranslations
            logger.info("Adding Qt libraries to the manifest...")
            self.vcpkg_manager.add_dependency("qtbase")
            self.vcpkg_manager.add_dependency("qtdeclarative")
            self.vcpkg_manager.add_dependency("qttranslations")
        elif is_qt_project and (self.is_shared_library() or self.is_static_library()):
            qt_template_path = os.path.join(self.template_path, "QT_PROJECT_TEMPLATE_LIB.txt")
            if not os.path.exists(qt_template_path):
                logger.error(f"Qt template file '{qt_template_path}' not found.")
                return False
            with open(qt_template_path, "r") as qt_template_file:
                qt_template_content = qt_template_file.read()
            template_content = template_content.replace("<QT_PROJECT_SECTION>", qt_template_content)
            template_content = template_content.replace("set(PROJECT_LIBRARIES \"\")", "")
            logger.info("Adding Qt libraries to the manifest...")
            self.vcpkg_manager.add_dependency("qtbase")
        else:
            template_content = template_content.replace("<QT_PROJECT_SECTION>", "")
        return template_content

    def is_project_initialized(self) -> bool:
        """
        Verifica se il progetto è inizializzato correttamente.
        
        Returns:
            bool: True se il progetto è inizializzato correttamente, False altrimenti.
        """
        if not self.check_project_files():
            return False
        if not self.check_cmake_file():
            return False
        if not self.check_vcpkg_json():
            return False

        if not self.check_config_json():
            return False
        

        return True
    
    def check_cmake_file(self) -> bool:
        """
        Checks if the CMakeLists.txt file exists and contains the project name.
        
        Returns:
            bool: True if the project is already configured, False otherwise.
        """
        if os.path.exists(self.repo_path / "CMakeLists.txt"):
            with open(self.repo_path / "CMakeLists.txt", "r") as cmake_file:
                content = cmake_file.read()
                if f"project({self.project_name}" in content:
                    logger.warning("Project already configured.")
                    self.already_created = True
                    return True
        else:
            return False
        return False

    def check_project_files(self) -> bool:
        """
        Checks if the files listed in the project.json file exist in the repository.
        
        Returns:
            bool: True if all files exist, False otherwise.
        """
        project_json_path = os.path.join(self.resources_path, "project.json")
        if not os.path.exists(project_json_path):
            logger.error("No project.json found in the resources.")
            return False

        with open(project_json_path, "r") as project_json_file:
            project_json_content = json.load(project_json_file)
            for file in project_json_content:
                file_path = os.path.join(self.repo_path, file.replace("/", os.sep))
                if not os.path.exists(file_path):
                    logger.debug(f"File {file} not found in the project.")
                    return False
                logger.debug(f"File {file} found in the project.")

        return True

    def check_vcpkg_json(self) -> bool:
        """
        Checks the validity of the vcpkg.json file.
        
        Returns:
            bool: True if vcpkg.json exists and is valid, False otherwise.
        """
        vcpkg_json_path = os.path.join(self.repo_path, "vcpkg.json")
        if not os.path.exists(vcpkg_json_path):
            logger.error("No vcpkg.json found in the project.")
            return False

        with open(vcpkg_json_path, "r") as vcpkg_json_file:
            vcpkg_json_content = json.load(vcpkg_json_file)
            if "name" not in vcpkg_json_content or "version" not in vcpkg_json_content:
                logger.error("vcpkg.json is not valid.")
                return False

        return True

    def check_config_json(self) -> bool:
        """
        Checks the validity of the config.json file in the cache.
        
        Returns:
            bool: True if config.json exists and is valid, False otherwise.
        """
        cache_path = utils.ProjectUtilities.get_cache_path()
        config_json_path = os.path.join(cache_path, "config.json")

        if not os.path.exists(config_json_path):
            logger.error("No config.json found in the cache.")
            return False

        with open(config_json_path, "r") as config_file:
            config = json.load(config_file)
            if not config.get("name"):
                logger.error("Project name is void or null.")
                return False
            if not config.get("type"):
                logger.error("Project type is void or null.")
                return False
            if config.get("type") not in ["exe", "shared", "static"]:
                logger.error("Project type is not valid.")
                return False

        return True

    def get_reserved_names(self)->json: 
        content=""
        with open(os.path.join(self.resources_path, "reserved_names.json"), "r") as reserved_names_file:
            content = json.load(reserved_names_file)
        return content
    def configure_automations(self,generator=""):
        if self.is_project_initialized():
            return self.cmake_presets_manager.configure_cmake_user_presets(generator)

        if self.cmake_presets_manager.configure_cmake_presets() and self.cmake_presets_manager.configure_cmake_user_presets(generator):
            if os.path.exists(os.path.join(self.repo_path, "init.sh")):
                shutil.move(os.path.join(self.repo_path,"init.sh"), os.path.join(self.repo_path,"vpm.sh"))
            if os.path.exists(os.path.join(self.repo_path, "init.bat")):
                shutil.move(os.path.join(self.repo_path, "init.bat"), os.path.join(self.repo_path, "vpm.bat"))
           

            logger.info("init scripts moved to initializers")
           
            return True
        else:
            logger.error("Error configuring CMakeUserPresets.json.")
            return False
 
    def reset(self):
        newfiles = self.git_manager.get_untracked_files()

        for file in newfiles:
            file_path = Path(self.repo_path) / file
            if file_path.is_dir():
                logger.warning(f"Removing directory: {file_path}")
                shutil.rmtree(file_path, ignore_errors=True)
            elif file_path.is_file():
                logger.warning(f"Removing file: {file_path}")
                file_path.unlink()

        cmake_user_presets = Path(self.repo_path) / "CMakeUserPresets.json"
        cmake_presets = Path(self.repo_path) / "CMakePresets.json"
        cmake_folder = Path(self.repo_path) / "cmake"
        include_folder = Path(self.repo_path) / "include"
        src_folder = Path(self.repo_path) / "src"
        test_folder = Path(self.repo_path) / "test"
        resources_folder = Path(self.repo_path) / "resources"
        ci_folder=Path(self.repo_path) / ".ci"

        #clear .cache folder
        cache_folder = Path(self.repo_path) / ".vpm" / ".cache"
        if cache_folder.exists():
            shutil.rmtree(cache_folder, ignore_errors=True)
            logger.warning(f"Removed directory: {cache_folder}")
            #restore config backup if it isn't void
            if len(self.config_bkp)>0:
                os.makedirs(os.path.join(self.repo_path, ".vpm", ".cache"), exist_ok=True)
                with open(os.path.join(self.repo_path, ".vpm", ".cache", "config.json"), "w") as config_file:
                    json.dump(self.config_bkp, config_file, indent=4)
                logger.info("Restored config cache")
        if not self.already_created:
            if cmake_user_presets.exists():
                cmake_user_presets.unlink()
                logger.warning(f"Removed file: {cmake_user_presets}")

            if cmake_presets.exists():
                cmake_presets.unlink()
                logger.warning(f"Removed file: {cmake_presets}")

            if cmake_folder.exists() and not any(cmake_folder.iterdir()):
                cmake_folder.rmdir()
                logger.warning(f"Removed empty directory: {cmake_folder}")
            if ci_folder.exists() and not any(ci_folder.iterdir()):
                ci_folder.rmdir()
                logger.warning(f"Removed empty directory: {ci_folder}")
            for folder in [include_folder, src_folder, test_folder,resources_folder]:
                if folder.exists() and folder.is_dir():
                    logger.warning(f"Removing directory and its contents: {folder}")
                    shutil.rmtree(folder, ignore_errors=True)
                

        logger.info("Project folder cleaned.")

    @staticmethod
    def get_logo():
        #read company_logo.txt in .assets folder
        logo=""
        with open(os.path.join(utils.ProjectUtilities.get_assets_path(), "company_logo.txt"), "r") as file:
            logo = file.read()
        return logo