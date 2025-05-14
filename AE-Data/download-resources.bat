
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://getfile.dokpub.com/yandex/get/https://disk.yandex.ru/d/3BpBTkqZMeIxsQ', 'resources.zip' )"
powershell Expand-Archive resources.zip -DestinationPath "."
pause
