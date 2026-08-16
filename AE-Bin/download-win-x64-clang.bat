
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://getfile.dokpub.com/yandex/get/https://disk.yandex.ru/d/mSNgD1m5krSvmw', 'external.zip' )"

@echo off
for /f "delims=" %%H in ('powershell -NoProfile -Command ^ "(Get-FileHash -Path 'external.zip' -Algorithm SHA256).Hash"') do (
    set "HASH=%%H"
)

if /i "%HASH%"=="4B4719946188704AA5D687071784F500AC4D50EA92054BC80E75515E5DD8C946" (
	powershell Expand-Archive external.zip -DestinationPath "." -Force
	del "external.zip"
    exit /b 0
) else (
    echo *** HASH MISMATCH ***
    echo Actual: %HASH%
	pause
    exit /b 1
)
