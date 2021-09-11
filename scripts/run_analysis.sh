#!/bin/sh

if [[ "$BUILDKITE_PULL_REQUEST" == "false" ]]
then
sonar-scanner \
-Dsonar.branch.name=$BUILDKITE_BRANCH
else
sonar-scanner \ 
-Dsonar.pullrequest.base=$BUILDKITE_PULL_REQUEST_BASE_BRANCH \
-Dsonar.pullrequest.branch=$BUILDKITE_BRANCH \
-Dsonar.pullrequest.key=$BUILDKITE_PULL_REQUEST
fi