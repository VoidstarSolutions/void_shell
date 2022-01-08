#!/bin/bash

set -e

echo "--- Run Unit Tests, Generate Coverage"

ceedling test:all gcov:all utils:gcov
