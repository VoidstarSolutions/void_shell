sonar-scanner \
  -Dsonar.projectKey=void_shell \
  -Dsonar.sources=. \
  -Dsonar.cfamily.build-wrapper-output=bw-output \
  -Dsonar.host.url=http://localhost:9000 \
  -Dsonar.login=2bdae852ef3f2970a817c54984341dee306ec16f \
  -Dsonar.cfamily.threads=8 \
  -Dsonar.cfamily.cache.enabled=true \
  -Dsonar.cfamily.cache.path=./build/analysis_cache