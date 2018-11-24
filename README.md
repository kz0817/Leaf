

# How to build

    ./autogen.sh
    ./confgure
    make

If some depending libraies are installed under /usr/local, the environment
variable ACLOCAL_PATH is required like,

    ACLOCAL_PATH=/usr/local/share/aclocal ./autogen.sh

or make the file named 'rc.local' with the following content

    ACLOCAL_PATH=/usr/local/share/aclocal
