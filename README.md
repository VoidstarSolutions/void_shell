# Void Shell

[![Build status](https://badge.buildkite.com/8d52f60b475beb11840952302942d35d29674347a54537bc28.svg)](https://buildkite.com/voidstar-solutions/void-shell)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=void_shell&metric=alert_status)](https://sonarcloud.io/dashboard?id=void_shell)

## Introduction

Void Shell is an open source, lightweight, statically allocated shell with no dependencies on standard libraries.  It can be used in both desktop applications as well as embedded systems, although it is tailored for lightweight embedded processors.  All printing is handled through the excellent [mpaland/printf](https://github.com/mpaland/printf) library.  The library includes a CMake build system and is configured as an interface library so that it inherits all of the compilation and linking options of the library including it.
