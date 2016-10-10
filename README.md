Annabell
========

A cognitive neural architecture able to learn and communicate through natural language.

Installation and usage
----------------------

In order to build the project the following libraries are needed:

 - **pthread** and **gtest**: [Download from GoogleTest project](https://github.com/google/googletest)
 - **pcre**: [Download from pcre.org](http://pcre.org/)

Additionally, installation and usage instructions are provided in the document **annabell-x.x.x_man.pdf**, where x.x.x refers to the version number.

Building from source
--------------------

This project uses GNU Autotools (http://www.gnu.org/software/automake/manual/html_node/Autotools-Introduction.html#Autotools-Introduction).

The recommended way of building from source is as follows:

 - run `autoreconf --install` from root directory
 - `mkdir build`
 - `cd build`
 - `../configure`
 - `make`

That will configure `build` subdirectory to build the project, avoiding clutter on the root directory and generating all .o and binaries there. In particular it will generate the `annabell` program in `build/src/annabell`
