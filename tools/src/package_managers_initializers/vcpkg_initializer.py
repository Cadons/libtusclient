from .package_manager import PackageManager
import os
import json
from rich.console import Console
from rich.prompt import Confirm

console = Console()

class VCPKGInitializer(PackageManager):
    """
    VCPKG package manager initializer.
    """
    VCPKG_CONFIG_FILE = "vcpkg.json"
    CMAKE_PRESETS_FILE = "CMakePresets.json"

    def __init__(self, project_name: str = "my_project"):
        super().__init__("vcpkg")
        self.project_name = project_name.lower().replace(" ", "_")

        self.vcpkg_location = "${VCPKG_ROOT}"
        if os.name != 'nt':
            vcpkg_path = os.popen("which vcpkg").read().strip()
            if vcpkg_path:
                self.find_vcpkg_root(vcpkg_path)
            else:
                console.print(
                    "[bold red]VCPKG not found. Please ensure vcpkg is installed and configured correctly.[/bold red]")
                exit(1)
        else:
            vcpkg_path = os.popen("where vcpkg").read().strip()
            if vcpkg_path:
                self.find_vcpkg_root(vcpkg_path)
            else:
                console.print(
                    "[bold red]VCPKG not found. Please ensure vcpkg is installed and configured correctly.[/bold red]")
                exit(1)
        self.toolchain_file =f"{self.vcpkg_location}/scripts/buildsystems/vcpkg.cmake"
        self.parent_preset = "vcpkg-default"

    def find_vcpkg_root(self, vcpkg_path):
        if vcpkg_path:
            vcpkg_dir = os.path.dirname(os.path.abspath(vcpkg_path))
            self.vcpkg_location = vcpkg_dir.replace("\\", "/")
            return self.vcpkg_location
        return None
    def create_vcpkg_json(self):
        return os.system(f"vcpkg new --application --name {self.project_name} --version 0.1.0")

    def initialize(self):
        console.print("[bold green]Initializing VCPKG project[/bold green]")
        if os.path.exists(self.VCPKG_CONFIG_FILE):
            overwrite = Confirm.ask(f"[yellow]{self.VCPKG_CONFIG_FILE} already exists. Overwrite?[/yellow]")
            if not overwrite:
                console.print("[cyan]Skipping vcpkg.json creation.[/cyan]")
                return
        else:
            ret = self.create_vcpkg_json()
            if ret != 0:
                console.print("[bold red]Failed to create vcpkg.json via 'vcpkg new'[/bold red]")
                return

        vcpkg_json_path = os.path.abspath(self.VCPKG_CONFIG_FILE)
        if os.path.exists(vcpkg_json_path):
            with open(vcpkg_json_path, 'r') as file:
                vcpkg_data = json.load(file)

            vcpkg_data.update({
                "name": self.project_name,
                "version": "0.1.0",
                "dependencies": ["gtest"]
            })

            with open(vcpkg_json_path, 'w') as file:
                json.dump(vcpkg_data, file, indent=2)
            console.print(f"[green]{self.VCPKG_CONFIG_FILE} updated successfully[/green]")
        else:
            console.print(f"[bold red]Warning:[/bold red] {self.VCPKG_CONFIG_FILE} not found at {vcpkg_json_path}")

    def configure_cmake_presets(self):
        """
        Create generic CMake presets for vcpkg with toolchain file link.
        """
        cmake_presets = {
            "version": 3,
            "configurePresets": [
                {
                    "name": "vcpkg-default",
                    "hidden": True,
                    "binaryDir": "${sourceDir}/build/"
                }
            ]
        }

        cmake_presets_path = os.path.join(__file__,"..","..","..","..", self.CMAKE_PRESETS_FILE)
        if not os.path.exists(cmake_presets_path):
            with open(cmake_presets_path, 'w') as f:
                json.dump(cmake_presets, f, indent=2)
            console.print(f"[green]CMake presets created at {cmake_presets_path}[/green]")
        else:
            console.print(f"[cyan]CMake presets already exist at {cmake_presets_path}. Skipping creation.[/cyan]")

        self.generate_cmake_user_presets(inherit_from=self.parent_preset, toolchain_file=self.toolchain_file)
    def get_toolchain_file(self):
        """
        Returns the path to the vcpkg toolchain file.
        """
        return self.toolchain_file
    def get_parent_preset(self):
        """
        Returns the parent preset for vcpkg.
        """
        return self.parent_preset