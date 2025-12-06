
wget -O 'resources.zip' 'https://getfile.dokpub.com/yandex/get/https://disk.yandex.ru/d/GC-lD9H1kBmYKw'

HASH=$(sha256sum "resources.zip" | awk '{print $1}')
EXPECTED="C8CD32E056C5EE40471C83C2D3833B199A7BE536F861A96C74177D1B85694004"

if [[ "${HASH,,}" == "${EXPECTED,,}" ]]; then
	unzip -f -d . 'resources.zip'
	rm resources.zip
    exit 0
else
    echo "*** HASH MISMATCH ***"
    echo "Actual: $HASH"
	read -p "press any key..."
    exit 1
fi
