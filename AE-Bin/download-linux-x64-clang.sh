
wget -O external.zip "https://getfile.dokpub.com/yandex/get/https://disk.yandex.ru/d/kop1jSW6Mtm7Ww"

HASH=$(sha256sum "external.zip" | awk '{print $1}')
EXPECTED="3E698F16398C710E856531E63A699DE44F89314D3EA451C47BC365837DF20823"

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
