
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://getfile.dokpub.com/yandex/get/https://disk.yandex.ru/d/GC-lD9H1kBmYKw', 'resources.zip' )"
powershell Expand-Archive resources.zip -DestinationPath "."
pause
