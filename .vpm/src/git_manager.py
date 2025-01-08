import os
import git
import logging
import coloredlogs

# Set up logger with colored output
logger = logging.getLogger("CMakeInitializer")
class GitRepositoryManager:
    def __init__(self, repo_path):
        """
        Initializes the Git repository manager.
        
        Args:
            repo_path (str): Path to the Git repository.
        """
        self.repo_path = repo_path
        self.repo = None

        # Check if the repository exists at the given path
        if os.path.exists(repo_path):
            try:
                self.repo = git.Repo(repo_path)
                logger.info(f"Git repository successfully opened at: {repo_path}")
            except Exception as e:
                logger.critical(f"Error while opening the Git repository: {e}")
        else:
            logger.error(f"Repository path not found: {repo_path}")

    def configure_tools_repo(self):
        """
        Configures the Git repository by initializing and updating submodules.
        
        Returns:
            bool: True if configuration is successful, False otherwise.
        """
        logger.info("Configuring CMakeTools...")

        try:
            # Initialize and update Git submodules
            self.repo.git.submodule("init")
            self.repo.git.submodule("update", "--remote")
            logger.info("Submodules initialized and updated.")
        except Exception as e:
            logger.critical(f"An error occurred while configuring submodules: {e}")
            return False

        return True

    def add_to_gitignore(self, gitignore_path, entry):
        """
        Adds a specified entry to the .gitignore file if it doesn't already exist.
        
        Args:
            gitignore_path (str): Path to the .gitignore file.
            entry (str): The entry to add to the .gitignore file.
        
        Returns:
            bool: True if the entry was added or already exists, False if an error occurred.
        """
        # Check if the .gitignore file exists
        if not os.path.exists(gitignore_path):
            logger.error(f"Gitignore file '{gitignore_path}' not found.")
            return False

        try:
            # Open the .gitignore file and add the entry if it doesn't already exist
            with open(gitignore_path, "r+") as gitignore_file:
                gitignore_content = gitignore_file.read()
                if entry not in gitignore_content:
                    gitignore_file.seek(0, os.SEEK_END)
                    gitignore_file.write(f"\n{entry}")
                    logger.info(f"Added '{entry}' to .gitignore.")
                else:
                    logger.warning(f"'{entry}' already exists in .gitignore.")
        except Exception as e:
            logger.error(f"An error occurred while modifying the .gitignore file: {e}")
            return False

        return True
    def get_untracked_files(self):
        """
        Retrieves a list of untracked files in the repository.

        Returns:
            list: A list of untracked files.
        """
        return  self.repo.untracked_files
    def get_repo_remote_url(self):
        """
        Retrieves the remote URL of the Git repository.

        Returns:
            str: The remote URL of the Git repository.
        """
        return self.repo.remotes.origin.url