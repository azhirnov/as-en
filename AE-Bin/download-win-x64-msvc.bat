
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://getfile.dokpub.com/yandex/get/https://disk.yandex.ru/d/nypQ26FY8abeog', 'external.zip' )"

@echo off
for /f "delims=" %%H in ('powershell -NoProfile -Command ^ "(Get-FileHash -Path 'external.zip' -Algorithm SHA256).Hash"') do (
    set "HASH=%%H"
)

if /i "%HASH%"=="6E4C929CBE16DE1CA99DCFF129AA401964B71E07E20F2CA968E477D36F3F614B" (
	powershell Expand-Archive external.zip -DestinationPath "." -Force
	del "external.zip"
    exit /b 0
) else (
    echo *** HASH MISMATCH ***
    echo Actual: %HASH%
	pause
    exit /b 1
)
