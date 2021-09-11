git_branch=$(git rev-parse --abbrev-ref HEAD)

sonar-scanner -Dsonar.branch.name=$git_branch