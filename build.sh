
CXX=g++
SOURCES="main.cpp X11.cpp"

DEBUG='-g'

LINK_X11='-lX11 -lXfixes -lXcomposite'

$CXX $SOURCES -o main $(pkg-config --cflags --libs cairo opencv4) $LINK_X11 $DEBUG
