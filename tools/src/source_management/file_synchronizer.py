import os
import glob

class FileSynchronizer:
    def __init__(self, module_path):
        self.module_path= module_path
        self.module_name = os.path.basename(module_path)
        self.header_variable_name = f"{self.module_name.upper().replace('-', '_').replace(' ','_')}_HEADERS"
        self.source_variable_name = f"{self.module_name.upper().replace('-', '_').replace(' ','_')}_SOURCES"
        self.test_source_variable_name = f"{self.module_name.upper().replace('-', '_').replace(' ','_')}_TEST_SOURCES"
        self.resource_variable_name = f"{self.module_name.upper().replace('-', '_').replace(' ','_')}_RESOURCES"
        self.header_files = []
        self.source_files = []
        self.resource_files = []
        self.header_file_extensions = ['.h', '.hpp']
        self.source_file_extensions = ['.cpp', '.c', '.cc', '.cxx', '.mm']
        self.test_source_files = ['.cpp', '.h', '.hpp']
        self.resource_file_extensions = ['.qrc', '.rc', '.ui', '.json', '.xml', '.txt', '.md', '.yaml', '.yml', '.ini', '.conf', '.cfg', '.properties', '.csv', '.svg', '.png', '.jpg', '.jpeg', '.gif',',vtk','stl','.qml']

    def scan_folder(self, extensions, folder, relative_to_module=True):
        output = []
        for ext in extensions:
            output.extend(glob.glob(os.path.join(self.module_path, folder, '**', f'*{ext}'), recursive=True))
        if not output:
            return []

        base_path = self.module_path if relative_to_module else os.path.join(self.module_path, folder)
        output = [os.path.relpath(file, base_path) for file in output]
        output = [file.replace('\\', '/') for file in output]
        result = list(set(output))
        result.sort()
        return result
    def synchronize(self):
        """
        Synchronize header and source files in the module path.
        """

        # Remove duplicates
        self.header_files = self.scan_folder(self.header_file_extensions, 'include/',
                                             relative_to_module=True)
        self.source_files = self.scan_folder(self.source_file_extensions, 'src/',
                                             relative_to_module=True)
        self.test_source_files = self.scan_folder(self.test_source_files, 'test/',
                                                  relative_to_module=False)
        self.resource_files = self.scan_folder(self.resource_file_extensions, 'resources/',
                                               relative_to_module=True)

        #write cmake/module_name_sources.cmake
        cmake_sources_path = os.path.join(self.module_path, '.cmake', f'{self.module_name}_sources.cmake')
        os.makedirs(os.path.dirname(cmake_sources_path), exist_ok=True)
        with open(cmake_sources_path, 'w') as f:
            # Headers
            f.write(f"set({self.header_variable_name}\n")
            for header in self.header_files:
                f.write(f"    {header}\n")
            f.write(")\n\n")

            # Sources
            f.write(f"set({self.source_variable_name}\n")
            for source in self.source_files:
                f.write(f"    {source}\n")
            f.write(")\n\n")

            # Test sources
            f.write(f"set({self.test_source_variable_name}\n")
            for test_source in self.test_source_files:
                f.write(f"    {test_source}\n")
            f.write(")\n\n")

            # Resources
            f.write(f"set({self.resource_variable_name}\n")
            for resource in self.resource_files:
                f.write(f"    {resource}\n")
            f.write(")\n")
        #create cmake/sources.cmake that includes all module sources
        cmake_sources_include_path = os.path.join(self.module_path, '.cmake', 'sources.cmake')
        #check if include is already present if missing append otherwise do nothing
        include_str = f"include(${{CMAKE_CURRENT_LIST_DIR}}/{self.module_name}_sources.cmake)\n"
        if os.path.exists(cmake_sources_include_path):
            with open(cmake_sources_include_path, 'r') as f:
                content = f.read()
            if include_str not in content:
                with open(cmake_sources_include_path, 'a') as f:
                    f.write(include_str)
        else:
            os.makedirs(os.path.dirname(cmake_sources_include_path), exist_ok=True)
            with open(cmake_sources_include_path, 'w') as f:
                f.write(include_str)

        print(f"[bold green]Synchronized files for module '{self.module_path}'[/bold green]")


