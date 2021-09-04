sonar-scanner \
  -Dsonar.organization=voidstarsolutions \
  -Dsonar.projectKey=void_shell \
  -Dsonar.sources=./ \
  -Dsonar.host.url=https://sonarcloud.io \
  -Dsonar.cfamily.compile-commands=build/compile_commands.json \
  -Dsonar.cfamily.gcov=build/artifacts/gcov/sonar_coverage.xml