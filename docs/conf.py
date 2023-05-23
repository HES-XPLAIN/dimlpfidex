# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'DimlpFidex'
copyright = "2023, HES-XPLAIN"
author = (
    '"Jean-Marc Boutay <jean-marc.boutay@heige.ch>", '
    '"Guido Bologna <guido.bologna@hesge.ch>"'
)
release = '0.0.0'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = ['breathe']

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']



# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']

# -- Breathe configuration ---------------------------------------------------
breathe_projects = {
    "dimlp": "../build/docs/doxygen/xml/",
    "fidex": "../build/docs/doxygen/xml/",
    "fidexglo": "../build/docs/doxygen/xml/",
    "hyperlocust": "../build/docs/doxygen/xml/",
}
breathe_default_project = "dimlpfidex"
