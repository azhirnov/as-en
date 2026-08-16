
wget -O external.zip "https://getfile.dokpub.com/yandex/get/https://disk.yandex.ru/d/p1LCm6EKYyTWVw"

HASH=$(sha256sum "external.zip" | awk '{print $1}')
EXPECTED="636B853EDF6640519AB4529DD2F1B2B8FE7755F0517752A051F981ABDCFE6E1A"

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
