
wget -O external.zip "https://getfile.dokpub.com/yandex/get/https://disk.yandex.ru/d/QfvgJL2ypzGVAA"

HASH=$(sha256sum "external.zip" | awk '{print $1}')
EXPECTED="EAE27EC285B040C87EE6657B3F9A9BF68723ECF6047DFDE12FC77E89683B2859"

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
