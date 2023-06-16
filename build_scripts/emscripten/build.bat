cd ../..
rmdir /Q /S "_build_ems"
mkdir _build_ems
cd _build_ems

emcmake cmake -DCMAKE_BUILD_TYPE=Debug -DAE_DISABLE_THREADS=OFF ".." && cmake --build . --config Debug && cd .. && emrun index.html --port 9090
