import os

import click
from rich.console import Console

from tools.src.package_managers_initializers.package_manager import PackageManager
from tools.src.package_managers_initializers.vcpkg_initializer import VCPKGInitializer
from tools.src.project_management.project_configuration import ProjectConfig


@click.group()
def cli():
    """Project configuration commands."""
    pass

@cli.command()
def configure():
    """
    Configure the project using the specified package manager.

    PACKAGE_MANAGER_NAME is the name of the package manager to use.
    """
    console = Console()
    try:
        #ask witch generator to use
        console.print("[bold cyan]Configuring project with package manager...[/bold cyan]")


        console.print("[bold cyan]Configuring CMake presets...[/bold cyan]")
        project_configuration = ProjectConfig.load(os.path.join(__file__,"..","..","..","..", "project.json"))
        package_manager=project_configuration.package_manager
        print(f"[bold cyan]Using package manager: {package_manager}[/bold cyan]")
        if not package_manager:
            console.print("[bold red]No package manager configured in project.json.[/bold red]")
            return
        toolchain_path = ""
        parent_preset=""
        if package_manager == "vcpkg":
            vcpkg_instance = VCPKGInitializer("")
            toolchain_path=vcpkg_instance.get_toolchain_file()
            parent_preset = vcpkg_instance.get_parent_preset()
        else:
            console.print(f"[bold yellow]Package manager '{package_manager}' is not supported yet.[/bold yellow]")
        console.print("[yellow]Warning: Be sure that the selected generator is available on your machine[/yellow]")

        if len(toolchain_path)>0 and len(parent_preset)>0:
            console.print(f"[bold green]Using toolchain file: {toolchain_path}[/bold green]")
            PackageManager.generate_cmake_user_presets(
                inherit_from=parent_preset,
                toolchain_file=toolchain_path
            )
        else:
            PackageManager.generate_cmake_user_presets()
        console.print("[bold green]CMake presets configured successfully.[/bold green]")
    except FileNotFoundError:
        console.print("[bold red]Project configuration file not found. Please run 'init' command first.[/bold red]")
        raise click.Abort()
@cli.command()
def list_generators():
    """List available CMake generators."""
    console = Console()
    generator_list = PackageManager.list_generators()
    console.print("[bold cyan]Available CMake generators for this OS:[/bold cyan]")
    for gen in generator_list:
        console.print(f"  - {gen}")
if __name__ == "__main__":
    cli()