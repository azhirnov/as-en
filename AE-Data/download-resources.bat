
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://getfile.dokpub.com/yandex/get/<res-editor-data>', 'resources.zip' )"
powershell Expand-Archive resources.zip -DestinationPath "."
pause
