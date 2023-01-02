# Mverse

A minimalist feature less object file visualizer that uses opengl

## Dependencies

* glfw
* opengl >= 3.3
* stb\_image

## Installation

```
$ cd mverse
$ sudo make install
```

Before running Mverse set the following environment variables (on your
`.bashrc`):
```
export MVERSE_VERTEX=/usr/share/mverse/dummy.vsh
export MVERSE_FRAGMENT=/usr/share/mverse/dummy.fsh
```

## Usage
```
$ mverse [-v vertexshader] [-f fragmentshader] objfile
```
