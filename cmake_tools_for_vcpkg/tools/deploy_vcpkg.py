import os
import subprocess
import shutil
import json
import stat
import gc
import sys
from time import sleep
from git import Repo, exc
import coloredlogs, logging
logger = logging.getLogger(__name__)
coloredlogs.install(level='INFO', logger=logger)
logger.addHandler(logging.StreamHandler(sys.stdout))
# Constants
VCPKG_REPO_URL = "git@srv-gitexample.com:mysw_dev/vcpkg.git"  # Replace with actual VCPKG repo URL
PROJECT_DIR = os.path.dirname(os.path.abspath(__file__))
TMP_DIR = os.path.join(PROJECT_DIR, "tmp")
VCPKG_CLONE_PATH = os.path.join(TMP_DIR, "vcpkg_temp")

# Paths
vcpkg_json_path = os.path.join(PROJECT_DIR, "..","..", "vcpkg.json")
portfile_template_path = os.path.join(PROJECT_DIR, "cmake", "portfile_template.cmake")
portfile_output_path = os.path.join(TMP_DIR, "ports")

# Create tmp directory
os.makedirs(TMP_DIR, exist_ok=True)

# Utilities
def find_git_root(path):
    """Find the root Git directory by traversing upwards."""
    while not os.path.isdir(os.path.join(path, ".git")):
        parent = os.path.dirname(path)
        if parent == path:
            raise exc.InvalidGitRepositoryError(f"No .git directory found in {PROJECT_DIR} or its parents.")
        path = parent
    return path

def get_project_name():
    """Read the project name from vcpkg.json."""
    with open(vcpkg_json_path, "r") as f:
        data = json.load(f)
    return data["name"]

def handle_remove_readonly(func, path, exc_info):
    """Error handler for removing read-only files."""
    os.chmod(path, stat.S_IWRITE)
    func(path)

def delete_with_retries(path, retries=5, delay=2):
    """Retry deletion with exponential backoff on failure."""
    os.chdir(PROJECT_DIR)
    for attempt in range(retries):
        try:
            shutil.rmtree(path, onerror=handle_remove_readonly)
            logger.info(f"Deleted {path} successfully.")
            return
        except Exception as e:
            if attempt < retries - 1:
                logger.warning(f"Retry {attempt + 1}/{retries} to delete {path} failed due to: {e}. Retrying...")
                sleep(delay * (2 ** attempt))  # Exponential backoff
            else:
                logger.error(f"Failed to delete {path} after {retries} attempts: {e}")

# Git operations
def clone_vcpkg_repo():
    """Clone the vcpkg repository."""
    if os.path.exists(VCPKG_CLONE_PATH):
        delete_with_retries(VCPKG_CLONE_PATH)
    Repo.clone_from(VCPKG_REPO_URL, VCPKG_CLONE_PATH).close()

def add_and_commit(repo, message):
    """Add all changes and commit to the provided repository."""
    repo.git.add(A=True)
    repo.index.commit(message)

# Portfile creation

def create_portfile(repo_url, last_tag, project_name):
    """Generate portfile.cmake from the template."""
    os.makedirs(portfile_output_path, exist_ok=True)

    with open(portfile_template_path, "r") as template_file:
        template_content = template_file.read()
    
    portfile_content = template_content.replace("<REPO>", repo_url).replace("<REF>", last_tag)
    port_folder_path = os.path.join(portfile_output_path, project_name)
    portfile_path = os.path.join(port_folder_path, "portfile.cmake")
    
    if not os.path.exists(port_folder_path):
        logger.info(f"Creating {port_folder_path}")
        os.makedirs(port_folder_path, exist_ok=True)
    
    logger.info(f"Creating {portfile_path}")
    with open(portfile_path, "w") as portfile_file:
        portfile_file.write(portfile_content)
    shutil.copyfile(vcpkg_json_path, os.path.join(portfile_output_path, project_name, "vcpkg.json"))
    return port_folder_path

# Vcpkg operations
def run_vcpkg_commands(project_name):
    """Run vcpkg format-manifest and x-add-version commands."""
    vcpkg_json_path_local = os.path.join("ports", project_name, "vcpkg.json")
    try:
        subprocess.run(["vcpkg", "format-manifest", vcpkg_json_path_local], check=True)
        subprocess.run([
            "vcpkg",
            "--x-builtin-ports-root=./ports",
            "--x-builtin-registry-versions-dir=./versions",
            "x-add-version", "--all", "--verbose", "--overwrite-version"
        ], check=True)
    except subprocess.CalledProcessError as e:
        logger.error(f"Error while running vcpkg commands: {e}")
        raise

# Main workflow
try:
    # Get project and repo details
    project_name = get_project_name()
    git_root = find_git_root(PROJECT_DIR)
    
    # Retrieve repository info
    repo = Repo(git_root)
    repo_url = next(repo.remote().urls)
    last_tag = repo.git.describe("--tags", "--abbrev=0")
    logger.info(f"Last tag: {last_tag}")
    # Get the commit hash of the tag
    tag_commit_hash = repo.git.rev_parse(last_tag)
    logger.info(f"Commit hash of the last tag: {tag_commit_hash}")
    del repo  # Free resources
    #run setup.py to generate vcpkg.json
    subprocess.run(["python", os.path.join(os.path.dirname(__file__), "setup.py")], check=True)
    # Generate portfile.cmake and clone vcpkg repository
    port_folder_path=create_portfile(repo_url, tag_commit_hash, project_name)
    clone_vcpkg_repo()
    shutil.copytree(port_folder_path, os.path.join(VCPKG_CLONE_PATH, "ports", project_name),dirs_exist_ok=True)

    # Commit the new portfile to vcpkg repository
    repo = Repo(VCPKG_CLONE_PATH)
    add_and_commit(repo, f"{project_name} files added to the registry")
    
    # Run vcpkg commands
    os.chdir(VCPKG_CLONE_PATH)
    run_vcpkg_commands(project_name)
    
    # Finalize by committing the registry addition and pushing
    add_and_commit(repo, f"{project_name} added to the registry")
    repo.git.push()
    
    # Show the last commit hash
    last_commit = repo.head.commit.hexsha
    logger.info(f"Last commit hash in vcpkg repository (update vcpkg-configuration.json with this baseline):")
    logger.info(f"Baseline: {last_commit}")
    
    # Clean up
    del repo
    gc.collect()  # Force garbage collection
    
finally:
    # Ensure temporary vcpkg clone and ports are deleted
    delete_with_retries(TMP_DIR)

logger.info(f"Process completed: {project_name} added to the vcpkg registry.")