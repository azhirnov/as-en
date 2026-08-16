
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://getfile.dokpub.com/yandex/get/https://disk.yandex.ru/d/Q3JOqJEUSxRMLQ', 'resources.zip' )"

@echo off
for /f "delims=" %%H in ('powershell -NoProfile -Command ^ "(Get-FileHash -Path 'resources.zip' -Algorithm SHA256).Hash"') do (
    set "HASH=%%H"
)

if /i "%HASH%"=="082B489649B1B444BB2453350D3AB2F5DF277C0E007148F65B8AC7D2B67F60FD" (
	powershell Expand-Archive resources.zip -DestinationPath "." -Force
	del "resources.zip"
    exit /b 0
) else (
    echo *** HASH MISMATCH ***
    echo Actual: %HASH%
	pause
    exit /b 1
)
