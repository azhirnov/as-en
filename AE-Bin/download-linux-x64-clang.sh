
wget -O external.zip "https://getfile.dokpub.com/yandex/get/https://disk.yandex.ru/d/bwsEUz7LsibJrg"

HASH=$(sha256sum "external.zip" | awk '{print $1}')
EXPECTED="D1EB92F881104D830146FB862ABA45489538C25458F271E232CBD1808352683F"

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
