
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://getfile.dokpub.com/yandex/get/https://disk.yandex.ru/d/ONijcekKBKxbJg', 'external.zip' )"
powershell Expand-Archive external.zip -DestinationPath "."
del "external.zip"
		