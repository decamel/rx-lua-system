.PHONY: build
build:
	cmake --build --preset debug

.PHONY: config
config:
	cmake --preset dev


.PHONY: run
run: build
	@echo "--------   Running Server   --------"
	@./build/ultron-server --script=./examples/main.lua

.PHONY: test
test: build test-core

.PHONY: test-core
test-core:
	@./build/core/tests/ultron_core_tests
