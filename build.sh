set -eu

cd $(dirname $0)
cv=/opt/homebrew/Cellar/opencv/4.7.0_1/

jsoncpp=./lib/jsoncpp/
jsoncppsrc=${jsoncpp}src/lib_json/

inc="-I ${cv}include/opencv4 -I ${jsoncpp}/include"

libsrc=""
libsrc="${libsrc} ${jsoncppsrc}json_reader.cpp"
libsrc="${libsrc} ${jsoncppsrc}json_value.cpp"
libsrc="${libsrc} ${jsoncppsrc}json_writer.cpp"


lib="-L${cv}lib/"
lib="${lib} -lopencv_core -lopencv_imgcodecs"
zig c++ -std=c++20 -Wall -O2 ${inc} ${lib} src/main.cpp ${libsrc} -o gumomiramap
time ./gumomiramap
