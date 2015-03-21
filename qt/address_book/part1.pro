QT += widgets

SOURCES =	addressbook.cpp\
			main.cpp

HEADERS =	addressbook.h

QMAKE_PROJECT_NAME = ab_part1

# install
target.path = .
INSTALLS += target
