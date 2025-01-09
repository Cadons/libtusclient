import os
import os
from pathlib import Path
class ProjectUtilities:
    @staticmethod
    def get_template_path()->Path:
        """
        Returns the path to the template directory.
        """
        return Path(os.path.join(os.path.dirname(__file__),".." ,'.template'))
    @staticmethod
    def get_assets_path()->Path:
        """
        Returns the path to the assets directory.
        """
        return Path(os.path.join(os.path.dirname(__file__), "..", '.assets'))
    @staticmethod
    def get_script_path()->Path:
        """
        Returns the path to the script directory.
        """
        return Path(os.path.dirname(__file__))
    @staticmethod
    def get_resources_path()->Path:
        """
        Returns the path to the resources directory.
        """
        return Path(os.path.join(os.path.dirname(__file__), "..", '.resources'))
    @staticmethod
    def get_project_path()->Path:
        """
        Returns the path to the project directory.
        """
        return Path(os.path.dirname(os.path.dirname(__file__),"..",".."))
    @staticmethod
    def get_cache_path()->Path:
        """
        Returns the path to the cache directory.
        """
        return Path(os.path.join(os.path.dirname(__file__), "..", '.cache'))