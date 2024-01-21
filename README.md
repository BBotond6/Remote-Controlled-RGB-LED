# RGB LED Controller

## Overview
This project originally started as a university project, using an Arduino Uno, RGB LED, photo transistor, and a remote controller.
The primary goal was to enable users to control individual color channels and save custom colors to the EEPROM.

## Legacy Description

For those interested in the original project and its features, the legacy description is available in [PrevREADME.md](./PrevREADME.md).

## Updates

Recently, the project underwent a significant update, expanding its capabilities.
The new version uses a simple LED strip remote controller, but after decoding it can be used with any remote controller,
allowing users to save and recall more colors from the EEPROM effortlessly.
One notable improvement is the ability to adjust the brightness not only for individual colours, but also for saved colour profiles containing more than one colour.
Currently the project is used to control a 12V RGB LED strip.

## Schematic drawing for RGB LED strip



## Project Structure and Unit Testing

The project was structurally redesigned to make it suitable for unit testing. The most important updates are:

- **Structural Refactoring:** The project's architecture was redesigned to integrate unit testing. This refactoring ensures a modular and testable codebase.
- **GoogleTest Integration:** GoogleTest was used as the primary framework for unit testing.
  This powerful testing library provides a comprehensive set of tools for writing and executing tests.
- **CMake Integration:** A CMake file was introduced to streamline the build process for unit tests.
  CMake used for facilitating the configuration and generation of build files.
- **Cross-Platform Build Scripts:** To enhance accessibility, build scripts were created for Windows and Linux.
  These scripts automate the compilation, execution, and cleanup of unit tests, promoting a consistent and efficient testing workflow.
