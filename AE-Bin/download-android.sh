
wget -O external.zip "https://getfile.dokpub.com/yandex/get/https://disk.yandex.ru/d/0e-aoTMjIZ3emA"

HASH=$(sha256sum "external.zip" | awk '{print $1}')
EXPECTED="23D6010F1BE671CA13DE5350C1F3E86FC6439690A7CBF278120D0A56A1F9E618"

if [[ "${HASH,,}" == "${EXPECTED,,}" ]]; then
	unzip -f -d . 'external.zip'
	rm external.zip
    exit 0
else
    echo "*** HASH MISMATCH ***"
    echo "Actual: $HASH"
	read -p "press any key..."
    exit 1
fi
