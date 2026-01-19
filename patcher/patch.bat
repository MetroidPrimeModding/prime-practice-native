<# : install.bat
:: Based on https://stackoverflow.com/a/15885133/1683264

@echo off
setlocal

set SCRIPT_DIR="%~dp0"
cd %SCRIPT_DIR%

echo Select Input ISO
echo.

for /f "delims=" %%I in ('powershell -noprofile "iex (${%~f0} | out-string)"') do call :install "%%~I"
goto :EOF

:install
set INPUT_ISO=%1
echo Patching ISO...
main.exe iso -i %INPUT_ISO% -o metroid-prime-practice-mod.iso -m prime-practice
pause

goto :EOF

: end Batch portion / begin PowerShell hybrid chimera #>

Add-Type -AssemblyName System.Windows.Forms
$f = new-object Windows.Forms.OpenFileDialog
$f.InitialDirectory = "."
$f.Filter = "Metroid Prime (*.iso)|*.iso|All Files (*.*)|*.*"
$f.ShowHelp = $false
$f.Multiselect = $false
[void]$f.ShowDialog()
if ($f.Multiselect) { $f.FileNames } else { $f.FileName }
