
wget -O external.zip "https://getfile.dokpub.com/yandex/get/https://disk.yandex.ru/d/OR1hu3y9uTpdtw"

HASH=$(sha256sum "external.zip" | awk '{print $1}')
EXPECTED="D5123E4197970B57351E3B8F594178C00E10EC2063FB99C954B614F01E7AE646"

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
