import os
import click
from rich.console import Console
from tools.src.module_creation.module_creator import ModuleCreator
from typing import Dict

console = Console()


def to_pascal_case(text: str) -> str:
    if not text:
        return text
    if '_' not in text and text[0].isupper():
        return text
    components = text.split('_')
    return ''.join(x.title() for x in components)


def compose_namespace_path(namespace: str, path: str = "src") -> str:
    """Convert a namespace string into a directory path."""
    if not namespace:
        return path
    return os.path.join(path, *namespace.split('::'))


TEMPLATE_TYPES = {
    "class": ("Standard C++ Class", "ClassTemplateHandler"),
    "gtest": ("Google Test", "GTestTemplateHandler"),
    "gtest_fixture": ("Google Test with Fixture", "GTestTemplateHandler"),
    "gtest_parametrized": ("Parametrized Google Test", "GTestTemplateHandler"),
    "qrc": ("Qt Resource File", "QrcTemplateHandler")
}


@click.group()
def cli():
    """Source file creation commands."""
    pass


@cli.command()
def list_templates():
    """List available template types."""
    console.print("\n[bold cyan]Available template types:[/bold cyan]")
    for key, (desc, _) in TEMPLATE_TYPES.items():
        console.print(f"  - {key}: {desc}")


@cli.command()
@click.option('--type', '-t', 'template_type',
              type=click.Choice(list(TEMPLATE_TYPES.keys())),
              required=True,
              help='Type of template to use')
@click.option('--name', '-n',
              required=True,
              help='Name for the new file/class')
@click.option('--namespace', '-ns',
              default="",
              help='Namespace (for C++ files)')
@click.option('--output-dir', '-o',
              default="src",
              help='Output directory')
def create(template_type: str, name: str, namespace: str, output_dir: str):
    """Create new source files from templates."""
    try:
        creator = ModuleCreator(".", "lib", True)

        if template_type.startswith("gtest"):
            create_test_files(template_type, name, namespace, output_dir, creator)
        elif template_type == "class":
            create_class_files(name, namespace, output_dir, creator)
        elif template_type == "qrc":
            create_qrc_file(name, output_dir, creator)

        try:
            from tools.src.source_management.update_files import synchronize_module
            console.print("\n[bold blue]Updating CMake files...[/bold blue]")
            if synchronize_module(os.path.abspath(output_dir)):
                console.print("[bold green]CMake files updated successfully![/bold green]")
            else:
                console.print("[bold yellow]Warning: Failed to update CMake files[/bold yellow]")
        except ImportError:
            console.print("[bold yellow]Warning: Could not import update_files.py[/bold yellow]")
        except Exception as e:
            console.print(f"[bold yellow]Warning: Failed to update CMake files: {str(e)}[/bold yellow]")

    except Exception as e:
        console.print(f"[bold red]Error creating files: {str(e)}[/bold red]")
        raise click.Abort()


def create_class_files(name: str, namespace: str, output_dir: str, creator: ModuleCreator):
    """Create C++ class header and source files."""
    source_output_dir = os.path.join(output_dir, "src")
    header_output_dir = os.path.join(output_dir, "include")

    if namespace:
        header_output_dir = compose_namespace_path(namespace, header_output_dir)
        source_output_dir = compose_namespace_path(namespace, source_output_dir)
        os.makedirs(header_output_dir, exist_ok=True)
        os.makedirs(source_output_dir, exist_ok=True)

    header_filename = f"{to_pascal_case(name)}.h"
    template_params = {
        "class_name": name,
        "header_filename": header_filename,
        "namespace": namespace
    }

    console.print("\n[bold blue]Creating class files...[/bold blue]")

    # Create header file
    header_path = os.path.join(header_output_dir, header_filename)
    creator.create_file_from_template(
        "c++/class_template.h.jinja",
        header_path,
        **template_params
    )

    # Create source file
    source_path = os.path.join(source_output_dir, f"{to_pascal_case(name)}.cpp")
    creator.create_file_from_template(
        "c++/class_template.cpp.jinja",
        source_path,
        **template_params
    )


def create_test_files(template_type: str, name: str, namespace: str, output_dir: str, creator: ModuleCreator):
    """Create test files based on template type."""
    test_output_dir = os.path.join(output_dir, "test")
    if namespace:
        test_output_dir = compose_namespace_path(namespace, test_output_dir)
        os.makedirs(test_output_dir, exist_ok=True)

    test_params = {
        "class_name": name,
        "header_filename": f"{to_pascal_case(name)}.h",
        "namespace": namespace,
        "test_suite_name": f"{name}Test"
    }

    if template_type == "gtest_fixture":
        test_params.update({
            "param_test_fixture_name": f"{name}TestFixture",
            "param_type": "int",
            "param_suite_name": f"{name}TestSuite",
            "param_values": ["1", "2", "3"]
        })
        template_name = "gtest_fixture_template.cpp.jinja"
    elif template_type == "gtest_parametrized":
        template_name = "gtest_parametrized_template.cpp.jinja"
    else:
        template_name = "gtest_simple_template.cpp.jinja"

    console.print("\n[bold blue]Creating test files...[/bold blue]")
    test_path = os.path.join(test_output_dir, f"{to_pascal_case(name)}Test.cpp")
    creator.create_file_from_template(
       f"gtest/{template_name}",
        test_path,
        **test_params
    )


def create_qrc_file(name: str, output_dir: str, creator: ModuleCreator):
    """Create Qt resource file."""
    console.print("\n[bold blue]Creating QRC file...[/bold blue]")
    if not os.path.exists(os.path.join(output_dir,"resources")):
        os.makedirs(os.path.join(output_dir, "resources"), exist_ok=True)

    qrc_path = os.path.join(output_dir,"resources", f"{name.lower()}.qrc")
    creator.create_file_from_template(
       "qt/qrc_template.qrc.jinja",
        qrc_path,
        prefix=name,
        create_examples=True
    )


if __name__ == "__main__":
    cli()