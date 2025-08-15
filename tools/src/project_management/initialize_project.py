
import os
import click
from rich import print
from rich.prompt import Confirm
from jinja2 import Environment, FileSystemLoader

from tools.src.module_creation.module_creator import ModuleCreator
from tools.src.package_managers_initializers.package_manager import PackageManager
from project_configuration import ProjectConfig

PROJECT_FILE_NAME = "project.json"

def get_package_manager_initializer(package_manager: str, **kwargs):
    if package_manager == "vcpkg":
        from tools.src.package_managers_initializers.vcpkg_initializer import VCPKGInitializer
        return VCPKGInitializer(**kwargs)
    else:
        raise NotImplementedError(f"[red]Package manager '{package_manager}' is not implemented yet.[/red]")

def generate_cmakelists(cmake_template_path, output_path, project_config: ProjectConfig):
    if not os.path.exists(cmake_template_path):
        print(f"[red]CMake template not found at {cmake_template_path}. Please check the template path.[/red]")
        raise click.Abort()

    env = Environment(loader=FileSystemLoader(os.path.dirname(cmake_template_path)))
    template = env.get_template(os.path.basename(cmake_template_path))

    cmake_content = template.render(
        project_name=project_config.name,
        project_version=project_config.version,
        project_description="A monorepo project",
        org_name=project_config.organization,
        org_url="https://example.com",
        cpp_standard="23",
        prefer_ninja=True,
        subdirs=["app", "lib"]
    )

    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    with open(output_path, 'w') as cmake_file:
        cmake_file.write(cmake_content)

    print(f"[bold green]CMakeLists.txt created at[/bold green] {output_path}")

@click.group()
def cli():
    """Project initialization commands."""
    pass

@cli.command()
@click.option('--name', prompt='Project name', help='Name of the project')
@click.option('--org', prompt='Organization name', default='default_org', help='Organization name')
@click.option('--package-manager', type=click.Choice(['vcpkg', 'custom', 'conan']),
              default='vcpkg', prompt='Package manager to use', help='Package manager for dependencies')
@click.option('--force', is_flag=True, help='Force re-initialization even if project.json exists')
def init(name: str, org: str, package_manager: str, force: bool):
    """Initialize a new project."""
    try:
        root_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
        os.chdir(root_dir)
        project_json_path = os.path.join(os.getcwd(), PROJECT_FILE_NAME)

        if os.path.exists(project_json_path) and not force:
            print("[yellow]Project already initialized. Use --force to reinitialize.[/yellow]")
            return

        if force and os.path.exists(PROJECT_FILE_NAME):
            if not Confirm.ask("Existing project.json found. Do you want to overwrite it?"):
                print("[red]Operation cancelled by user.[/red]")
                return
            os.remove(PROJECT_FILE_NAME)
            print("[green]Existing project.json removed.[/green]")

        project_config = ProjectConfig(
            name=name,
            organization=org,
            package_manager=package_manager,
            version="0.1.0",
            subtrees=[]
        )

        print("[cyan]Initializing project...[/cyan]")
        initializer = get_package_manager_initializer(package_manager, project_name=name)
        initializer.initialize()

        # Save project configuration
        project_config.save(project_json_path)
        print(f"[bold green]Project configuration saved to[/bold green] {project_json_path}")

        # Generate CMakeLists.txt
        cmake_template_path = os.path.join(os.path.dirname(__file__), "..","..","..", ".resources", "templates", "cmake", "CMakeLists.txt.jinja")
        cmake_output_path = os.path.join(os.getcwd(), "CMakeLists.txt")
        generate_cmakelists(cmake_template_path, cmake_output_path, project_config)

        initializer.configure_cmake_presets()
        print("[bold green]Project initialized successfully.[/bold green]")

        #create app folder structure
        app_dir = os.path.join(os.getcwd(), "app")
        module_creator = ModuleCreator("app", "app", True)
        module_creator.create_module()
        #create lib folder structure
        lib_dir = os.path.join(os.getcwd(), "lib")
        #create .gitkeep in lib
        os.makedirs(lib_dir, exist_ok=True)
        #create CMakeLists.txt
        cmake_lib_path = os.path.join(lib_dir, "CMakeLists.txt")
        if not os.path.exists(cmake_lib_path):
            with open(cmake_lib_path, 'w') as f:
                f.write("# CMakeLists.txt for lib\n")

    except Exception as e:
        print(f"[bold red]Error initializing project: {str(e)}[/bold red]")
        raise click.Abort()

@cli.command()
def configure():
    """Configure CMake presets for the project."""
    try:
        project_json_path = os.path.join(os.getcwd(), PROJECT_FILE_NAME)
        project_config = ProjectConfig.load(project_json_path)
        initializer = get_package_manager_initializer(project_config.package_manager)
        initializer.configure_cmake_presets()
        print("[bold green]CMake presets configured successfully.[/bold green]")
    except FileNotFoundError:
        print("[red]Project not initialized. Please run 'init' command first.[/red]")
        raise click.Abort()

@cli.command()
def list_generators():
    """List available CMake generators."""
    generator_list = PackageManager.list_generators()
    print("[bold cyan]Available CMake generators for this OS:[/bold cyan]")
    for gen in generator_list:
        print(f"  - {gen}")

if __name__ == "__main__":
    cli()