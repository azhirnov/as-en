
wget -O 'resources.zip' 'https://getfile.dokpub.com/yandex/get/https://disk.yandex.ru/d/Q3JOqJEUSxRMLQ'

HASH=$(sha256sum "resources.zip" | awk '{print $1}')
EXPECTED="082B489649B1B444BB2453350D3AB2F5DF277C0E007148F65B8AC7D2B67F60FD"

if [[ "${HASH,,}" == "${EXPECTED,,}" ]]; then
	unzip -f -d . 'resources.zip'
	rm resources.zip
    exit 0
else
    echo "*** HASH MISMATCH ***"
    echo "Actual: $HASH"
	read -p "press any key..."
    exit 1
fi
