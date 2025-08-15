import os
import click
from rich.console import Console
from tools.src.module_creation import module_creator

console = Console()
MODULE_TYPES = ["app", "lib"]


@click.group()
def cli():
    """Module creation commands."""
    pass


@cli.command()
@click.argument('name', type=str)
@click.option('--type', '-t', 'module_type',
              type=click.Choice(MODULE_TYPES),
              default='lib',
              help='Type of module to create (app or lib)')
@click.option('--gtest/--no-gtest',
              default=True,
              help='Include Google Test support')
def create(name: str, module_type: str, gtest: bool):
    """
    Create a new module with the specified name and type.

    NAME is the name of the module to create.
    """
    try:
        if not name:
            console.print("[bold red]Module name is required.[/bold red]")
            return

        module_creator_instance = module_creator.ModuleCreator(name, module_type, gtest)

        if not module_creator_instance.module_path:
            console.print("[bold red]Module path is not set. Cannot create module.[/bold red]")
            return

        if not module_creator_instance.create_module():
            return

        # Update CMakeLists.txt parent
        cmake_file_path = os.path.join(module_creator_instance.module_path, "..", "CMakeLists.txt")
        if os.path.exists(cmake_file_path):
            with open(cmake_file_path, 'r') as cmake_file:
                cmake_content = cmake_file.read()
            if f"add_subdirectory({name})" not in cmake_content:
                with open(cmake_file_path, 'a') as cmake_file:
                    cmake_file.write(f"\nadd_subdirectory({name})\n")
        else:
            with open(cmake_file_path, 'w') as cmake_file:
                cmake_file.write(f"add_subdirectory({name})\n")

        console.print(
            f"[green]Module '{name}' of type '{module_type}' created successfully at {module_creator_instance.module_path}[/green]")

    except Exception as e:
        console.print(f"[bold red]Error creating module: {str(e)}[/bold red]")
        raise click.Abort()


@cli.command()
def list_types():
    """List available module types."""
    console.print("\n[bold blue]Available module types:[/bold blue]")
    for type_name in MODULE_TYPES:
        console.print(f"  - {type_name}")


if __name__ == "__main__":
    cli()