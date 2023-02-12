set -eu

cv=/opt/homebrew/Cellar/opencv/4.7.0_1/
inc="-I ${cv}include/opencv4"

lib="-L${cv}lib/"
lib="${lib} -lopencv_core -lopencv_imgcodecs"
zig c++ -std=c++20 -Wall -O2 ${inc} ${lib} src/main.cpp -o gumomiramap
time ./gumomiramap
