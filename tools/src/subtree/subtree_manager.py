import os
import subprocess
from typing import Optional, List

import click
from rich.console import Console
from rich.table import Table

from tools.src.project_management.project_configuration import ProjectConfig, SubtreeConfig

console = Console()


class SubtreeManager:
    @staticmethod
    def find_project_config() -> str:
        """Find the project.json file in the parent directory of 'tools'."""
        current_dir = os.path.dirname(os.path.abspath(__file__))
        if os.path.basename(current_dir) == 'tools':
            parent_dir = os.path.dirname(current_dir)
        else:
            parent_dir = current_dir


        config_path = os.path.join(parent_dir,"..","..","..", 'project.json')

        if not os.path.exists(config_path):
            raise FileNotFoundError(f"Cannot find project.json in {parent_dir}")

        return config_path

    def __init__(self):

        self.config_path = self.find_project_config()
        self.config = ProjectConfig.load(self.config_path)
        self.root_dir = os.path.dirname(self.config_path)
        os.chdir(self.root_dir)


    def _run_git_command(self, command: List[str], check: bool = True) -> subprocess.CompletedProcess:
        """Execute a git command and return the result."""
        try:
            result = subprocess.run(
                ["git"] + command,
                check=check,
                capture_output=True,
                text=True
            )
            return result
        except subprocess.CalledProcessError as e:
            console.print(f"[bold red]Git command failed:[/bold red] {e.stderr}")
            raise

    def add_subtree(self, subtree: SubtreeConfig) -> bool:
        """Add a new subtree to the repository."""
        try:
            if not self._handle_existing_path(subtree):
                return False

            if not self._add_or_update_subtree(subtree):
                return False

            self._update_configuration(subtree)
            console.print(f"[green]Successfully added/updated subtree {subtree.name}[/green]")
            return True

        except Exception as e:
            return self._handle_error(e, subtree.name)

    def _handle_existing_path(self, subtree: SubtreeConfig) -> bool:
        """Handle case when subtree path already exists."""
        if not os.path.exists(os.path.join(self.root_dir, subtree.path)):
            return True

        console.print(f"[yellow]Warning: Path '{subtree.path}' already exists.[/yellow]")
        if not click.confirm("Do you want to remove the existing directory and re-add the subtree?", default=False):
            return False

        self._run_git_command(["rm", "-rf", subtree.path])
        self._run_git_command(["commit", "-m", f"Remove existing {subtree.path} before re-adding as subtree"])
        return True



    def _add_or_update_subtree(self, subtree: SubtreeConfig) -> bool:
        """Add new subtree or update existing one."""
        try:
            self._run_git_command([
                "subtree", "add",
                "--prefix", subtree.path,
                "--squash",
                subtree.name, subtree.main_branch
            ])
            return True
        except subprocess.CalledProcessError as e:
            return self._handle_existing_prefix(e, subtree)

    def _handle_existing_prefix(self, error: subprocess.CalledProcessError, subtree: SubtreeConfig) -> bool:
        """Handle case when subtree prefix already exists."""
        if "prefix already exists" not in error.stderr:
            raise error

        if not click.confirm("Prefix already exists. Try to pull updates instead?", default=True):
            return False

        self._run_git_command([
            "subtree", "pull",
            "--prefix", subtree.path,
            "--squash",
            subtree.name, subtree.main_branch
        ])
        return True

    def _update_configuration(self, subtree: SubtreeConfig) -> None:
        """Update and save configuration with new subtree."""
        self.config.add_subtree(subtree)
        self.config.save(self.config_path)

    def _handle_error(self, error: Exception, subtree_name: str) -> bool:
        """Handle errors during subtree operations."""
        if isinstance(error, subprocess.CalledProcessError):
            if "already exists" in error.stderr:
                console.print(f"[yellow]Remote {subtree_name} already exists.[/yellow]")
            elif "working tree" in error.stderr:
                console.print("[bold red]Error:[/bold red] Must be run from the root of the git repository")
            else:
                console.print(f"[bold red]Git command failed:[/bold red] {error.stderr}")
        else:
            console.print(f"[bold red]Failed to add subtree:[/bold red] {str(error)}")
        return False

    def remove_subtree(self, name: str) -> bool:
        """Remove a subtree from the repository."""
        subtree = self.config.get_subtree(name)
        if not subtree:
            console.print(f"[yellow]No subtree found with name: {name}[/yellow]")
            return False

        try:
            # Remove the subtree files
            self._run_git_command(["rm", "-r", subtree.path])

            # Remove the remote
            self._run_git_command(["remote", "remove", name])

            # Update configuration
            self.config.remove_subtree(name)
            self.config.save(self.config_path)

            console.print(f"[green]Successfully removed subtree {name}[/green]")
            return True

        except Exception as e:
            console.print(f"[bold red]Failed to remove subtree:[/bold red] {str(e)}")
            return False

    def pull_subtree(self, name: str, branch: Optional[str] = None) -> bool:
        """Pull updates from a subtree's remote repository."""
        subtree = self.config.get_subtree(name)
        if not subtree:
            console.print(f"[yellow]No subtree found with name: {name}[/yellow]")
            return False

        try:
            target_branch = branch or subtree.main_branch
            self._run_git_command([
                "subtree", "pull",
                "--prefix", subtree.path,
                "--squash",
                name, target_branch
            ])

            console.print(f"[green]Successfully pulled updates for subtree {name}[/green]")
            return True

        except Exception as e:
            console.print(f"[bold red]Failed to pull subtree:[/bold red] {str(e)}")
            return False

    def push_subtree(self, name: str, branch: Optional[str] = None) -> bool:
        """Push local changes to a subtree's remote repository."""
        subtree = self.config.get_subtree(name)
        url = self.config.get_subtree(name).git_url
        if not subtree:
            console.print(f"[yellow]No subtree found with name: {name}[/yellow]")
            return False

        try:
            target_branch = branch or subtree.staging_branch
            self._run_git_command([
                "subtree", "push",
                "--prefix", subtree.path,
                url, target_branch
            ])

            console.print(f"[green]Successfully pushed changes for subtree {name}[/green]")
            return True

        except Exception as e:
            console.print(f"[bold red]Failed to push subtree:[/bold red] {str(e)}")
            return False

    def list_subtrees(self) -> None:
        """Display all configured subtrees in a table."""
        table = Table(show_header=True, header_style="bold magenta")
        table.add_column("Name", style="cyan")
        table.add_column("Path", style="green")
        table.add_column("Git URL", style="bold blue", overflow="fold")
        table.add_column("Main Branch", style="yellow")
        table.add_column("Staging Branch", style="yellow")

        for subtree in self.config.subtrees:
            table.add_row(
                subtree.name,
                subtree.path,
                subtree.git_url,
                subtree.main_branch,
                subtree.staging_branch
            )

        console.print(table)


@click.group()
def cli():
    """Manage Git subtrees for your project."""
    pass


@cli.command()
@click.argument('name')
@click.argument('git_url')
@click.argument('path')
@click.option('--main-branch', default='main', help='Main branch name')
@click.option('--staging-branch', default='staging', help='Staging branch name')
def add(name: str, git_url: str, path: str, main_branch: str, staging_branch: str):
    """Add a new subtree."""
    if not path.startswith('lib/'):
        console.print("[bold red]Error:[/bold red] The subtree path must be inside the 'lib/' directory")
        return

    if '..' in path:
        console.print("[bold red]Error:[/bold red] Invalid path: directory traversal not allowed")
        return

    manager = SubtreeManager()
    subtree = SubtreeConfig(name, git_url, main_branch, staging_branch, path)
    manager.add_subtree(subtree)

@cli.command()
@click.argument('name')
def remove(name: str):
    """Remove a subtree."""
    manager = SubtreeManager()
    manager.remove_subtree(name)


@cli.command()
@click.argument('name')
@click.option('--branch', help='Branch to pull from')
def pull(name: str, branch: Optional[str]):
    """Pull updates from a subtree's remote."""
    manager = SubtreeManager()
    manager.pull_subtree(name, branch)


@cli.command()
@click.argument('name')
@click.option('--branch', help='Branch to push to')
def push(name: str, branch: Optional[str]):
    """Push changes to a subtree's remote."""
    manager = SubtreeManager()
    manager.push_subtree(name, branch)


@cli.command(name='list')
def list_cmd():
    """List all configured subtrees."""
    manager = SubtreeManager()
    manager.list_subtrees()


if __name__ == '__main__':
    cli()
