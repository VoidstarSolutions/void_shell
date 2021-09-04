sonar-scanner \
  -Dsonar.organization=voidstarsolutions \
  -Dsonar.projectKey=void_shell \
  -Dsonar.sources=./source \
  -Dsonar.host.url=https://sonarcloud.io \
  -Dsonar.cfamily.compile-commands=build/compile_commands.json \
  -Dsonar.coverageReportPaths=build/artifacts/gcov/sonar_coverage.xml