import os
import sys
import click
import importlib.util
from typing import Dict, Any, Optional, Tuple
from rich.console import Console
from rich.panel import Panel
from rich import box

console = Console()

# --- Tool configuration ---
TOOLS: Dict[str, Tuple[str, str]] = {
    'subtree': ('subtree_manager', 'cli'),
    'module': ('create_module', 'cli'),
    'source': ('create_source_file', 'cli'),
    'init': ('initialize_project', 'cli'),
    'update': ('update_files', 'cli'),
    'configure': ('configure_project', 'cli'),
}

# --- Path setup ---
CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.dirname(CURRENT_DIR)

# Centralized search paths
MODULE_SEARCH_PATHS = [
    CURRENT_DIR,
    PROJECT_ROOT,
    os.path.join(CURRENT_DIR, "src", "source_management"),
    os.path.join(CURRENT_DIR, "src", "module_creation"),
    os.path.join(CURRENT_DIR, "src", "project_management"),
    os.path.join(CURRENT_DIR, "src", "subtree")
]


def setup_python_path():
    """Ensure all necessary paths are on sys.path."""
    for path in MODULE_SEARCH_PATHS:
        if os.path.exists(path) and path not in sys.path:
            sys.path.insert(0, path)


# --- Module loading utilities ---

def import_module_from_file(module_name: str, file_path: str) -> Optional[Any]:
    try:
        spec = importlib.util.spec_from_file_location(module_name, file_path)
        if not spec or not spec.loader:
            raise ImportError(f"Could not create spec for {file_path}")

        module = importlib.util.module_from_spec(spec)
        sys.modules[module_name] = module
        spec.loader.exec_module(module)
        return module
    except Exception as e:
        console.print(f"[bold red]Import Error:[/bold red] {e}")
        return None


def find_tool_path(module_name: str) -> str:
    """Search for the file path of a tool module."""
    for directory in MODULE_SEARCH_PATHS:
        path = os.path.join(directory, f"{module_name}.py")
        if os.path.exists(path):
            return path

    raise FileNotFoundError(f"Module '{module_name}' not found in known locations.")


# --- CLI commands ---

@click.group()
def cli():
    """Project management tools suite."""
    pass


@cli.command()
def info():
    """Show available tools."""
    print_welcome()


def print_welcome():
    """Display welcome message with tool list."""
    tool_list = "\n".join(
        f"- [green]{name}[/green]: {module}" for name, (module, _) in TOOLS.items()
    )
    console.print(Panel.fit(
        f"[bold blue]Project Management Tools[/bold blue]\n\n"
        f"Available tools:\n{tool_list}\n\n"
        "Use --help with any command for more information",
        title="Welcome",
        border_style="blue",
        box=box.ROUNDED
    ))


def load_commands():
    """Dynamically import and register subcommands."""
    setup_python_path()

    for command_name, (module_name, cli_func_name) in TOOLS.items():
        try:
            path = find_tool_path(module_name)
            module = import_module_from_file(module_name, path)
            if not module:
                continue

            cli_func = getattr(module, cli_func_name, None)
            if callable(cli_func):
                cli.add_command(cli_func, command_name)
            else:
                console.print(f"[yellow]Warning:[/yellow] {module_name} has no function '{cli_func_name}'")
        except Exception as e:
            console.print(f"[yellow]Warning:[/yellow] Could not load tool '{command_name}': {e}")


# --- Entry point ---

def main():
    try:
        print_welcome()
        load_commands()
        cli()
    except Exception as e:
        console.print(f"[bold red]Fatal error:[/bold red] {e}")
        sys.exit(1)


if __name__ == '__main__':
    main()