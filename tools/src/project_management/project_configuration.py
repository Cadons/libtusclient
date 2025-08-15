from dataclasses import dataclass
from typing import List, Optional
import json
import os

@dataclass
class SubtreeConfig:
    name: str
    git_url: str
    main_branch: str
    staging_branch: str
    path: str

    @classmethod
    def from_dict(cls, data: dict) -> 'SubtreeConfig':
        return cls(
            name=data['name'],
            git_url=data['git_url'],
            main_branch=data['main_branch'],
            staging_branch=data['staging_branch'],
            path=data['path']
        )


@dataclass
class ProjectConfig:
    name: str
    organization: str
    package_manager: str
    version: str
    subtrees: List[SubtreeConfig]

    @classmethod
    def load(cls, file_path: str) -> 'ProjectConfig':
        """Load configuration from a JSON file."""
        if not os.path.exists(os.path.abspath(file_path)):
            raise FileNotFoundError(f"Configuration file not found: {file_path}")

        with open(os.path.abspath(file_path), 'r') as f:
            data = json.load(f)

        return cls(
            name=data['name'],
            organization=data['organization'],
            package_manager=data['package_manager'],
            version=data['version'],
            subtrees=[SubtreeConfig.from_dict(st) for st in data.get('subtrees', [])]
        )

    def save(self, file_path: str) -> None:
        """Save configuration to a JSON file."""
        data = {
            'name': self.name,
            'organization': self.organization,
            'package_manager': self.package_manager,
            'version': self.version,
            'subtrees': [
                {
                    'name': st.name,
                    'git_url': st.git_url,
                    'main_branch': st.main_branch,
                    'staging_branch': st.staging_branch,
                    'path': st.path
                }
                for st in self.subtrees
            ]
        }

        with open(file_path, 'w') as f:
            json.dump(data, f, indent=2)

    def add_subtree(self, subtree: SubtreeConfig) -> None:
        """Add a new subtree configuration."""
        if any(st.name == subtree.name for st in self.subtrees):
            raise ValueError(f"Subtree with name '{subtree.name}' already exists")
        self.subtrees.append(subtree)

    def remove_subtree(self, name: str) -> Optional[SubtreeConfig]:
        """Remove a subtree configuration by name."""
        for i, subtree in enumerate(self.subtrees):
            if subtree.name == name:
                return self.subtrees.pop(i)
        return None

    def get_subtree(self, name: str) -> Optional[SubtreeConfig]:
        """Get a subtree configuration by name."""
        for subtree in self.subtrees:
            if subtree.name == name:
                return subtree
        return None

    def update_subtree(self, name: str, **kwargs) -> bool:
        """Update a subtree configuration."""
        subtree = self.get_subtree(name)
        if not subtree:
            return False

        for key, value in kwargs.items():
            if hasattr(subtree, key):
                setattr(subtree, key, value)

        return True
