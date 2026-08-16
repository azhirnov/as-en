
wget -O external.zip "https://getfile.dokpub.com/yandex/get/https://disk.yandex.ru/d/-RL5-ffLXJ50BA"

HASH=$(sha256sum "external.zip" | awk '{print $1}')
EXPECTED="CFCF706189070D3A453E1A4B9B7363FAFC257FA2B3268DA8B09678EF7B5AB6F7"

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
