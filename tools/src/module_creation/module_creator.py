import os
import json
from jinja2 import Environment, FileSystemLoader
from rich import print
from tools.src.source_management.file_synchronizer import FileSynchronizer


class ModuleCreator:
    def __init__(self, module_name: str, module_type: str = "app", use_gtest: bool = True):
        self.module_name = module_name
        self.module_type = module_type
        self.project_root = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))

        self.module_path = os.path.join(self.project_root, self.module_type, self.module_name)
        if self.module_type == "app":
            self.module_path = os.path.join(self.project_root, "app")

        self.templates_path = os.path.join(self.project_root, ".resources", "templates")
        self.file_synchronizer = FileSynchronizer(self.module_path)
        self.gtest = use_gtest

    def _get_folder_structure(self):
        if self.module_type == "lib":
            return {
                "src": ["src", self.module_name],
                "include": ["include", self.module_name],
                "test": ["test"],
                "docs": ["docs"],
                ".cmake": [".cmake"],
                "resources": ["resources"]
            }
        else:
            return {
                "src": ["src"],
                "include": ["include"],
                "test": ["test"],
                "docs": ["docs"],
                ".cmake": [".cmake"],
                "resources": ["resources"]
            }

    def _create_module_json(self):
        module_json = {
            "name": self.module_name,
            "version": "0.1.0",
            "type": self.module_type,
            "description": f"{self.module_name} module of type {self.module_type}"
        }
        module_json_path = os.path.join(self.module_path, "package.json")
        with open(module_json_path, 'w') as f:
            json.dump(module_json, f, indent=2)
        print(f"[bold green]Module configuration saved to[/bold green] {module_json_path}")

    def _get_project_data(self):
        project_json_path = os.path.join(self.project_root, "project.json")
        if not os.path.exists(project_json_path):
            print(f"[bold red]Project configuration file not found at {project_json_path}.[/bold red]")
            return {}
        try:
            with open(project_json_path, 'r') as f:
                return json.load(f)
        except json.JSONDecodeError:
            print("[bold red]Failed to parse project.json.[/bold red]")
            return {}

    def _create_module_structure(self):
        for folder_paths in self._get_folder_structure().values():
            folder_path = os.path.join(self.module_path, *folder_paths)
            os.makedirs(folder_path, exist_ok=True)

    def _create_files(self):
        project_info = self._get_project_data()
        files_config = {
            "README.md": {
                "template": "README_template.jinja",
                "params": {
                    "project_name": self.module_name,
                    "module_type": self.module_type
                }
            },
            "LICENSE": {
                "template": "LICENSE_template.jinja",
                "params": {
                    "project_name": self.module_name,
                    "organization": project_info.get("organization", "Unknown Organization"),
                    "contact_email": project_info.get("contact_email")
                }
            },
            "include/module.h": {
                "template": "c++/module_template.h.jinja",
                "params": {"target_name": self.module_name}
            },
            "CMakeLists.txt": {
                "template": f"cmake/{self.module_type}_CMakeLists.txt.jinja",
                "params": {
                    "target_name": self.module_name,
                    "sources": f"{self.module_name.upper().replace('-', '_').replace(' ','_')}_SOURCES",
                    "headers": f"{self.module_name.upper().replace('-', '_').replace(' ','_')}_HEADERS",
                    "resources": f"{self.module_name.upper().replace('-', '_').replace(' ','_')}_RESOURCES"
                }
            },
            "src/main.cpp": {
                "template": "c++/main_template.cpp.jinja",
                "params": {
                    "target_name": self.module_name,
                    "app": self.module_type == "app"
                }
            },
            "test/CMakeLists.txt": {
                "template": "cmake/test_CMakeLists.txt.jinja",
                "params": {
                    "test_target": self.module_name+"_test",
                    "use_gtest": self.gtest,
                    "sources": f"{self.module_name.upper().replace('-', '_')}_TEST_SOURCES",

                }
            },
            "test/test_main.cpp": {
                "template": "c++/test_main_template.cpp.jinja",
                "params": {
                    "target_name": self.module_name,
                    "use_gtest": self.gtest
                }
            },
            "docs/Doxyfile":{
                "template": "docs/doxyfile_template.jinja",
                "params": {
                    "project_name": self.module_name,
                }
            }

        }

        for rel_path, config in files_config.items():
            self.create_file_from_template(
                template_name=config["template"],
                output_path=os.path.join(self.module_path, rel_path),
                **config["params"]
            )

    def create_file_from_template(self, template_name: str, output_path: str, **kwargs):
        template_path = os.path.join(self.templates_path, template_name)
        if not os.path.exists(template_path):
            print(f"[bold red]Template not found: {template_path}[/bold red]")
            return

        env = Environment(loader=FileSystemLoader(self.templates_path),

                          trim_blocks=True,
                          lstrip_blocks=True
                          )
        template = env.get_template(template_name)
        content = template.render(**kwargs)

        os.makedirs(os.path.dirname(output_path), exist_ok=True)
        with open(output_path, 'w') as f:
            f.write(content)
        print(f"[bold green]Created[/bold green] {os.path.relpath(output_path, self.project_root)}")

    def create_module(self) -> bool:
        # Check if module already exists
        if os.path.exists(self.module_path):
            print(f"[bold red]Module '{self.module_name}' already exists at {self.module_path}[/bold red]")
            return False

        try:
            # Create module structure and files
            self._create_module_structure()
            self._create_module_json()
            self._create_files()
            self.file_synchronizer.synchronize()

            print(f"[bold green]Successfully created module '{self.module_name}' at {self.module_path}[/bold green]")
            return True
        except Exception as e:
            print(f"[bold red]Failed to create module: {str(e)}[/bold red]")
            return False