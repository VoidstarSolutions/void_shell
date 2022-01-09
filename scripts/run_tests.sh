#!/bin/bash

set -e

echo "--- Run Unit Tests, Generate Coverage"

ceedling test:all

echo "--- Generate Code Coverage Reports"

ceedling gcov:all utils:gcov
