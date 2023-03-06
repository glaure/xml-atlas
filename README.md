# xml-atlas
XML viewer editor project


[![Github All Releases](https://img.shields.io/github/downloads/glaure/xml-atlas/total)]()
[![Build](https://github.com/glaure/xml-atlas/actions/workflows/app_xml_atlas_ci.yml/badge.svg)](https://github.com/glaure/xml-atlas/actions/workflows/app_xml_atlas_ci.yml)

Open Source XML Editor that is heavily inspired by XML Marker (http://symbolclick.com/).



# Qt

Please install a valid Qt version.
The current reference version is Qt 5.15.2

## Windows deployment

Debug:

```
%PATH_TO_QT%/msvc2019_64/bin/windeployqt.exe --no-quick-import --no-system-d3d-compiler build64/Debug/
```

Release

```
%PATH_TO_QT%/msvc2019_64/bin/windeployqt.exe --no-quick-import --no-system-d3d-compiler build64/Release
```

