sonar-scanner \
  -Dsonar.organization=voidstarsolutions \
  -Dsonar.projectKey=void_shell \
  -Dsonar.sources=./source/ \
  -Dsonar.host.url=https://sonarcloud.io \
  -Dsonar.cfamily.build-wrapper-output=build/bw-output \
  -Dsonar.cfamily.gcov=build/artifacts/gcov/sonar_coverage.xml