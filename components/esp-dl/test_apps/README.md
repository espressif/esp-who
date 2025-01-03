
Steps to run esp-dl cases:

- Build
  - . ${IDF_PATH}/export.sh
  - pip install idf_build_apps
  - python test_apps/build_apps.py test_apps/esp-dl -t esp32p4 -m test_apps/esp-dl/models

- Test
  - pip install -r test_apps/requirement.txt
  - pytest test_apps/esp-dl --target esp32p4