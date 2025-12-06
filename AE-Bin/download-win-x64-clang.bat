
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://getfile.dokpub.com/yandex/get/https://disk.yandex.ru/d/ONijcekKBKxbJg', 'external.zip' )"

@echo off
for /f "delims=" %%H in ('powershell -NoProfile -Command ^ "(Get-FileHash -Path 'external.zip' -Algorithm SHA256).Hash"') do (
    set "HASH=%%H"
)

if /i "%HASH%"=="468A72BFE9547233E6388FEE5CF6C8FF2F1DDF8E9DE2EA0D491AF9A1E2921841" (
	powershell Expand-Archive external.zip -DestinationPath "." -Force
	del "external.zip"
    exit /b 0
) else (
    echo *** HASH MISMATCH ***
    echo Actual: %HASH%
	pause
    exit /b 1
)
