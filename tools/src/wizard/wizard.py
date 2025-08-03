import click
from typing import List, Dict, Any, Callable
from dataclasses import dataclass
from abc import ABC, abstractmethod


@dataclass
class WizardStep:
    """Represents a single step in the wizard"""
    name: str
    prompt: str
    type: type
    validator: Callable[[Any], bool] = lambda x: True
    choices: List[str] = None
    default: Any = None
    help_text: str = ""


class BaseWizard(ABC):
    """Base class for creating wizards"""

    def __init__(self, title: str):
        self.title = title
        self.steps: List[WizardStep] = []
        self.results: Dict[str, Any] = {}
        self.setup_steps()

    @abstractmethod
    def setup_steps(self) -> None:
        """Define the wizard steps - must be implemented by subclasses"""
        pass

    @abstractmethod
    def process_results(self) -> None:
        """Process the collected results - must be implemented by subclasses"""
        pass

    def add_step(self, step: WizardStep) -> None:
        """Add a step to the wizard"""
        self.steps.append(step)

    def run(self) -> Dict[str, Any]:
        """Run the wizard and collect inputs"""
        click.echo(f"\n=== {self.title} ===\n")

        for step in self.steps:
            while True:
                try:
                    if step.help_text:
                        click.echo(f"Help: {step.help_text}")
                    if step.choices:
                        click.echo(f"Choices: {', '.join(step.choices)}")
                        value = click.prompt(
                            step.prompt,
                            type=click.Choice(step.choices),
                            default=step.default
                        )
                    else:
                        value = click.prompt(
                            step.prompt,
                            type=step.type,
                            default=step.default
                        )

                    if step.validator(value):
                        self.results[step.name] = value
                        break
                    else:
                        click.echo("Invalid input. Please try again.")
                except click.Abort:
                    if click.confirm("Do you want to exit the wizard?"):
                        raise click.Abort()
                except Exception as e:
                    click.echo(f"Error: {str(e)}")
                    if not click.confirm("Do you want to try again?"):
                        raise click.Abort()

        self.process_results()
        return self.results


def wizard_command(wizard_class):
    """Decorator to create a click command from a wizard class"""

    @click.command()
    def wrapper():
        try:
            wizard = wizard_class()
            wizard.run()
        except click.Abort:
            click.echo("\nWizard cancelled.")
            return

    return wrapper

