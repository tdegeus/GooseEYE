from setuptools_scm import get_version
from skbuild import setup

project_name = "GooseEYE"

setup(
    name=project_name,
    description="Image based geometrical statistics",
    version=get_version(),
    license="GPLv3",
    author="Tom de Geus",
    author_email="tom@geus.me",
    url=f"https://github.com/tdegeus/{project_name}",
    packages=[f"{project_name}"],
    package_dir={"": "python"},
    cmake_install_dir=f"python/{project_name}",
    cmake_minimum_required_version="3.13",
    python_requires=">=3.6",
)
