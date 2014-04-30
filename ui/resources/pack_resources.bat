echo off

set OutDir=%1
rem replace \ to /
set "OutDir=%OutDir:\=/%"
rem call mkdir "%OutDir%" 2>nul & set ERRORLEVEL=0
rem copy /Y "%WinOutDir%obj\global_intermediate\ui\ui_resources\ui_resources_100_percent.pak" "%WinOutDir%ui_resources.pak"

python "../tools/grit/grit/format/repack.py" "%OutDir%ui_resources.pak" "%OutDir%obj/global_intermediate/ui/ui_resources/ui_resources_100_percent.pak" "%OutDir%obj/global_intermediate/ui/app_locale_settings/app_locale_settings_en-US.pak" "%OutDir%obj/global_intermediate/ui/ui_strings/ui_strings_en-US.pak"

