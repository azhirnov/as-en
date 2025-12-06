
wget -O external.zip "https://getfile.dokpub.com/yandex/get/https://disk.yandex.ru/d/KAzusnOEwdPKOg"

HASH=$(sha256sum "external.zip" | awk '{print $1}')
EXPECTED="FAEE8233AAA64E56768010CBA8A8B96EC01B65E556F052B603596B30C4DE36E9"

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
