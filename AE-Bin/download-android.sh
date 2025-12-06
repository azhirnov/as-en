
wget -O external.zip "https://getfile.dokpub.com/yandex/get/https://disk.yandex.ru/d/Dg6uluTCelp_kw"

HASH=$(sha256sum "external.zip" | awk '{print $1}')
EXPECTED="7E7F675883AC34EC16909002FD6CB1238703DADA87455E61862C3651899AD1A7"

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
