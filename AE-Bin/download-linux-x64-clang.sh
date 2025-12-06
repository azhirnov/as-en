
wget -O external.zip "https://getfile.dokpub.com/yandex/get/https://disk.yandex.ru/d/hjMLsFw3Er6zrw"

HASH=$(sha256sum "external.zip" | awk '{print $1}')
EXPECTED="0C87906148368F20A32FB7F171E030735BF009752A2A157E8D46522096DFE4D6"

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
