rm -rf "../../../../../AE-Data/external/source/stb"
rm -rf "temp"
git clone "../../../../../3party/stb" "temp"
mkdir "../../../../../AE-Data/external/source/stb"
cp "temp/stb_image.h" "../../../../../AE-Data/external/source/stb/stb_image.h"
cp "temp/stb_image_write.h" "../../../../../AE-Data/external/source/stb/stb_image_write.h"
cp "temp/stb_rect_pack.h" "../../../../../AE-Data/external/source/stb/stb_rect_pack.h"
cp "temp/stb_truetype.h" "../../../../../AE-Data/external/source/stb/stb_truetype.h"
cp "temp/LICENSE" "../../../../../AE-Data/external/source/stb/LICENSE"
rm -rf "temp"
read -p "press any key..."