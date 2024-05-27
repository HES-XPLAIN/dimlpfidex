# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'dimlpfidex'
copyright = '2023-2024, HES-XPLAIN'
author = 'HES-XPLAIN'
release = '0.0.0'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    'breathe',
    'exhale',
]

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']

# -- Breathe configuration ---------------------------------------------------
breathe_projects = {
    "common": "../build/docs/doxygen/xml/",
    "dimlp": "../build/docs/doxygen/xml/",
    "fidex": "../build/docs/doxygen/xml/",
}
breathe_default_project = "common"

# Setup the exhale extension
exhale_args = {
    # These arguments are required
    "containmentFolder":     "./api",
    "rootFileName":          "library_root.rst",
    "doxygenStripFromPath":  "../build/docs/doxygen",
    # Heavily encouraged optional argument (see docs)
    "rootFileTitle":         "Library API",
    # Suggested optional arguments
    "createTreeView":        True,
    # TIP: if using the sphinx-bootstrap-theme, you need
    # "treeViewIsBootstrap": True,
    "exhaleExecutesDoxygen": False,
    "exhaleDoxygenStdin":    "INPUT = ../build"
}

# Tell sphinx what the primary language being documented is.
primary_domain = 'cpp'

# Tell sphinx what the pygments highlight language should be.
highlight_language = 'cpp'
