import os
from typing import Optional
import click
from rich.console import Console
from tools.src.source_management.file_synchronizer import FileSynchronizer

console = Console()


def detect_module_structure(module_path: str) -> Optional[str]:
    """Detect the module name from the given path."""
    if not os.path.exists(module_path):
        return None
    return os.path.basename(os.path.normpath(module_path))


def synchronize_module(module_path: str) -> bool:
    """Synchronize the specified module's files."""
    if not os.path.exists(module_path):
        console.print(f"[bold red]Module path {module_path} does not exist[/bold red]")
        return False

    try:
        synchronizer = FileSynchronizer(module_path)
        synchronizer.synchronize()
        console.print(f"[bold green]Successfully synchronized module at {module_path}[/bold green]")
        return True
    except Exception as e:
        console.print(f"[bold red]Failed to synchronize module: {str(e)}[/bold red]")
        return False


@click.group()
def cli():
    """Update and synchronize module files."""
    pass


@cli.command()
@click.argument('path', type=click.Path(exists=True), required=False)
def sync(path: Optional[str]):
    """Synchronize a module's files.

    If PATH is not provided, will use current directory.
    """
    try:
        # Use provided path or current directory
        module_path = os.path.abspath(path if path else os.getcwd())

        # Detect and validate module
        module_name = detect_module_structure(module_path)
        if not module_name:
            raise click.ClickException(f"Invalid module path: {module_path}")

        console.print(f"[bold cyan]Processing module:[/bold cyan] {module_name}")

        # Synchronize the module
        if not synchronize_module(module_path):
            raise click.ClickException("Synchronization failed")

    except click.ClickException as e:
        console.print(f"[bold red]Error:[/bold red] {str(e)}")
        raise
    except Exception as e:
        console.print(f"[bold red]Unexpected error:[/bold red] {str(e)}")
        raise click.ClickException(str(e))


@cli.command()
def list_modules():
    """List all available modules in the project."""
    try:
        # Get project root (2 levels up from this file)
        project_root = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

        # Check common module directories
        module_dirs = ['app', 'lib']
        found_modules = []

        for dir_name in module_dirs:
            dir_path = os.path.join(project_root, dir_name)
            if os.path.exists(dir_path):
                modules = [d for d in os.listdir(dir_path)
                           if os.path.isdir(os.path.join(dir_path, d))]
                for module in modules:
                    found_modules.append(f"{dir_name}/{module}")

        if found_modules:
            console.print("[bold cyan]Available modules:[/bold cyan]")
            for module in sorted(found_modules):
                console.print(f"  - {module}")
        else:
            console.print("[yellow]No modules found in the project[/yellow]")

    except Exception as e:
        console.print(f"[bold red]Error listing modules:[/bold red] {str(e)}")
        raise click.ClickException(str(e))


if __name__ == '__main__':
    cli()