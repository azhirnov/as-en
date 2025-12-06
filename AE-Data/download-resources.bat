
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://getfile.dokpub.com/yandex/get/https://disk.yandex.ru/d/GC-lD9H1kBmYKw', 'resources.zip' )"

@echo off
for /f "delims=" %%H in ('powershell -NoProfile -Command ^ "(Get-FileHash -Path 'resources.zip' -Algorithm SHA256).Hash"') do (
    set "HASH=%%H"
)

if /i "%HASH%"=="C8CD32E056C5EE40471C83C2D3833B199A7BE536F861A96C74177D1B85694004" (
	powershell Expand-Archive resources.zip -DestinationPath "." -Force
	del "resources.zip"
    exit /b 0
) else (
    echo *** HASH MISMATCH ***
    echo Actual: %HASH%
	pause
    exit /b 1
)
