#!/bin/bash

PLATFORM="x86_64"
BUILD_PROFILE="debug"
CONFIG_NAME="$PLATFORM-$BUILD_PROFILE"
SEARCH_DIR="../build/$CONFIG_NAME/tests/bin"

for entry in "$SEARCH_DIR"/*
do
    echo "Running test: $entry"
    $entry
    echo "-------------------------------------------------"
    echo "-------------------------------------------------"
done

