
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://getfile.dokpub.com/yandex/get/https://disk.yandex.ru/d/p1LCm6EKYyTWVw', 'external.zip' )"

@echo off
for /f "delims=" %%H in ('powershell -NoProfile -Command ^ "(Get-FileHash -Path 'external.zip' -Algorithm SHA256).Hash"') do (
    set "HASH=%%H"
)

if /i "%HASH%"=="636B853EDF6640519AB4529DD2F1B2B8FE7755F0517752A051F981ABDCFE6E1A" (
	powershell Expand-Archive external.zip -DestinationPath "." -Force
	del "external.zip"
    exit /b 0
) else (
    echo *** HASH MISMATCH ***
    echo Actual: %HASH%
	pause
    exit /b 1
)
