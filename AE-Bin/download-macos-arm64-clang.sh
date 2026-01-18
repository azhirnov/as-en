
wget -O external.zip "https://getfile.dokpub.com/yandex/get/https://disk.yandex.ru/d/Q8NB7CPxVOp1cg"

HASH=$(sha256sum "external.zip" | awk '{print $1}')
EXPECTED="67800CEBB740C2F2180D115367D313816327B0F812986E7BC43CCA17BDE71469"

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
