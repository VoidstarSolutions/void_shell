#!bin/bash

set -e

echo "--- Make sure everything is cleaned up"

rm -rf build

echo "--- Build the demo app"

bear -- ceedling release

if [[ $? -ne 0 ]]; then
  echo "^^^ +++"
  echo "Build failed!!"
fi
