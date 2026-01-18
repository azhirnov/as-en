
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://getfile.dokpub.com/yandex/get/https://disk.yandex.ru/d/Blg4lnbBt-5PIA', 'external.zip' )"

@echo off
for /f "delims=" %%H in ('powershell -NoProfile -Command ^ "(Get-FileHash -Path 'external.zip' -Algorithm SHA256).Hash"') do (
    set "HASH=%%H"
)

if /i "%HASH%"=="3F7929C06FF1EB34B00D557D3807F8CF270142203C5FB2EBE47CD62F47D6DE70" (
	powershell Expand-Archive external.zip -DestinationPath "." -Force
	del "external.zip"
    exit /b 0
) else (
    echo *** HASH MISMATCH ***
    echo Actual: %HASH%
	pause
    exit /b 1
)
