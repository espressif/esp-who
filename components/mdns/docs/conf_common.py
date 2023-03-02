# flake8: noqa
from esp_docs.conf_docs import *

extensions += [
    'sphinx_copybutton',
    # Needed as a trigger for running doxygen
    'esp_docs.esp_extensions.dummy_build_system',
    'esp_docs.esp_extensions.run_doxygen',
]

# link roles config
github_repo = 'espressif/esp-protocols'

# context used by sphinx_idf_theme
html_context['github_user'] = 'espressif'
html_context['github_repo'] = 'esp-protocols'

# Extra options required by sphinx_idf_theme
project_slug = 'esp-idf'  # >=5.0
versions_url = 'https://github.com/espressif/esp-protocols/docs/docs_versions.js'

idf_targets = ['esp32']
languages = ['en', 'zh_CN']
