# truck-code

This repository contains a collection of code for platooning a string of automated trucks via cooperative adaptive cruise control (CACC).

# Installation

In order to install 

```
git clone
```

Next, install the binaries locally by running the following command:

```
make install
```

# Testing

Tests are an integral feature for ensuring that the codebase is functioning as expected prior to deployment. To this end, all individual features are tested using the Boost.Test [Unit Test Framework](https://www.boost.org/doc/libs/1_43_0/libs/test/doc/html/utf.html). If you have just installed this repository or and planning to develop features on top of the current codebase, it is *highly recommeneded* that you run the tests locally. To install Boost.Test on a Debian-style Linux system (e.g. QNX) run:

```
sudo apt-get install libboost-all-dev
```

Once you have installed Boost.Test, you can run the tests by calling:

```
blank
```

# Documentation

## Doxygen

All classes, files, and methods are documented using the [Doxygen](http://www.doxygen.nl/) framework. This documentation can be found online at [here](https://aboudykreidieh.github.io/truck-code/), and can be located locally from within the docs/ folder of the repository.

If you are modifying the repository locally and would like to update the documentation as well, you can do so by running the following command from the repository's main directory:

```
doxygen docs/.config
```

## GitHub Wiki

In addition to **blank**, **blank**. The github wiki can be found [here](https://github.com/AboudyKreidieh/truck-code/wiki).

# Contributors


