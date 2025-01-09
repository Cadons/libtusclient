# VPM (Vcpkg Project Manager)

**VPM** is a tool designed for managing CMake projects with Vcpkg integration. It simplifies project setup, updates configurations based on the environment, and ensures everything is in place for smooth CMake-Vcpkg usage.

## Features:
- **Initialization**: `init.bat` (Windows) or `init.sh` (Linux) will initialize and configure a new CMake-Vcpkg project.
- **Configuration Update**: `vpm.bat` (Windows) or `vpm.sh` (Linux) updates your project's configuration files based on the local environment.
- **File Creation**: If any essential files are missing, VPM will automatically generate them. Existing source files are left untouched.
- **Configuration Management**: If your project already has a CMake configuration, VPM won't overwrite it. If you're experiencing issues, like failing tests due to conflicting CMake settings, you can manually copy your CMake configuration back into the updated `CMakeLists.txt` after running VPM. Conflicting settings will be avoided automatically.

## How It Works:

### 1. **Initialization (init.bat/init.sh)**
   - Run the `init` script to set up your project and configure it for CMake and Vcpkg.
   - This will set up the necessary directories, files, and environment variables required for a functional CMake project with Vcpkg integration.

### 2. **Configuration Update (vpm.bat/vpm.sh)**
   - Once the initial setup is done, run the `vpm` script to update your project configuration. This tool will adjust CMake settings based on your local environment, ensuring proper integration with Vcpkg.

### 3. **File Management**
   - **Missing Files**: VPM will create any missing files, such as CMake files or Vcpkg integration files, to ensure the project is properly configured.
   - **Source Files**: If the project already has source files, they will not be modified.
   
### 4. **Restoring Configuration (For Existing Projects)**
   - If your project has an existing CMake configuration that you need to keep, run the `vpm` tool to update the environment. You may then manually restore any custom configurations into the updated `CMakeLists.txt`, ensuring to resolve any conflicts.

## Important Notes:
- **Don't modify the core VPM structure**: The internal structure of VPM should not be changed unless you are sure of what you're doing. If in doubt, refer to this README.
- **Keep your configurations intact**: If you're using an existing CMake setup, ensure to copy back your custom configuration after running VPM to avoid overwriting any specific settings.
