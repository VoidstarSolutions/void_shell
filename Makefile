.PHONY: all
all: debug release

.PHONY: debug
debug:
	@bash scripts/configure.sh debug
	@bash scripts/build.sh debug

.PHONY: release
release:
	@bash scripts/configure.sh
	@bash scripts/build.sh

.PHONY: test
test:
	@ceedling

.PHONY: clean
clean:
	@rm -rf build/
