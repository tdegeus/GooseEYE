project = "GooseEYE"
copyright = "2017-2022, Tom de Geus"
author = "Tom de Geus"

extensions = [
    "sphinx_tabs.tabs",
    "sphinx.ext.autosummary",
    "sphinx.ext.mathjax",
    "sphinx.ext.todo",
]

autodoc_type_aliases = {"Iterable": "Iterable", "ArrayLike": "ArrayLike", "DTypeLike": "DTypeLike"}
templates_path = ["_templates"]
exclude_patterns = ["_build", "Thumbs.db", ".DS_Store"]
html_theme = "furo"
