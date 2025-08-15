import os
import re
import subprocess
from abc import ABC, abstractmethod

from rich.console import Console
from rich.prompt import IntPrompt

console = Console()

class PackageManager(ABC):
    """
    Base class for package managers.
    """

    def __init__(self, name: str):
        self.name = name


    @abstractmethod
    def initialize(self):
        pass

    @abstractmethod
    def configure_cmake_presets(self):
        pass
    @abstractmethod
    def get_toolchain_file(self):
        """
        Returns the path to the toolchain file for this package manager.
        """
        pass
    @abstractmethod
    def get_parent_preset(self):
        """
        Returns the name of the parent preset for this package manager.
        """
        pass

    @staticmethod
    def list_generators():
        """
        Get list of available CMake generators, excluding specific IDE generators.

        Returns:
            list: List of available CMake generator names
        """
        output = PackageManager._get_cmake_help_output()
        if not output:
            return []

        return PackageManager._parse_generators(output)

    @staticmethod
    def _get_cmake_help_output():
        """
        Execute cmake --help command and return its output.

        Returns:
            str: Command output or empty string on error
        """
        try:
            return subprocess.check_output(
                ['cmake', '--help'],
                universal_newlines=True
            )
        except subprocess.CalledProcessError:
            return ""

    @staticmethod
    def _parse_generators(cmake_output):
        """
        Parse cmake help output to extract generator names.

        Args:
            cmake_output (str): Output from cmake --help command

        Returns:
            list: List of generator names
        """
        generators = []

        # Find the generators section
        lines = cmake_output.splitlines()
        try:
            generators_start = next(
                i for i, line in enumerate(lines)
                if "Generators" in line
            )
            # Skip the header line
            generators_start += 2
        except StopIteration:
            return []
        #exclude IDE generators
        # Parse generator entries
        for line in lines[generators_start:]:
            if not line.strip():
                break

            match = re.match(r'^\s*(\S.+?)\s+=', line)
            if not match:
                continue
            generator = match.group(1).replace("*", "").strip()
            generators.append(generator)

        return generators
    @staticmethod
    def generate_cmake_user_presets(inherit_from="",toolchain_file=None):
        generators = PackageManager.list_generators()
        if not generators:
            console.print("[bold red]No CMake generators found.[/bold red]")
            return

        cmake_presets_json = {
            "version": 3,
            "configurePresets": [],
            "buildPresets": [],
        }
        cpus = os.cpu_count() or 4

        console.print("[bold green]Available CMake Generators:[/bold green]")
        for idx, generator in enumerate(generators, 1):
            console.print(f"[cyan]{idx}[/cyan]. {generator}")

        console.print("[yellow]Warning: Be sure that the selected generator is available on your machine[/yellow]")

        selected_preset = None
        while selected_preset is None:
            try:
                selected_idx = IntPrompt.ask("Select generator number", default=1, show_default=False)
                selected_preset = generators[selected_idx - 1]
            except (ValueError, IndexError):
                console.print("[red]Invalid selection. Please choose a valid number.[/red]")

        generator = selected_preset
        generator_name = generator.replace(" ", "_").replace("-", "_")
        cmake_presets_json["configurePresets"].append({
            "name": generator_name,
            "hidden": selected_preset != generator,
            "generator": generator,
            "binaryDir": "${sourceDir}/build/${presetName}",
            "inherits": inherit_from
        })
        if toolchain_file:
            cmake_presets_json["configurePresets"][-1]["cacheVariables"]={
                "CMAKE_TOOLCHAIN_FILE": toolchain_file
            }
        cmake_presets_json["buildPresets"].append({
            "name": f"{generator_name} Debug",
            "hidden": selected_preset != generator,
            "configurePreset": generator_name,
            "configuration": "Debug",
            "jobs": cpus,
        })
        cmake_presets_json["buildPresets"].append({
            "name": f"{generator_name} Release",
            "hidden": selected_preset != generator,
            "configurePreset": generator_name,
            "configuration": "Release",
            "jobs": cpus,
        })

        cmake_user_presets_path = os.path.join(__file__,"..","..","..","..", "CMakeUserPresets.json")
        import json
        with open(cmake_user_presets_path, 'w') as f:
            json.dump(cmake_presets_json, f, indent=2)

        console.print(f"[bold green]CMake user presets generated at[/bold green] {cmake_user_presets_path}")
