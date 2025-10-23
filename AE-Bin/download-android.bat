
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://getfile.dokpub.com/yandex/get/https://disk.yandex.ru/d/Dg6uluTCelp_kw', 'external.zip' )"
powershell Expand-Archive external.zip -DestinationPath "."
del "external.zip"
