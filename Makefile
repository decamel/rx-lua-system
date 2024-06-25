.PHONY: build
build:
	cmake --build --preset debug

.PHONY: config
config:
	cmake --preset dev


.PHONY: run
run: build
	@echo "--------------------- Running Server at 7000 with main.lua ---------------------"
	@./build/ultron-server --script=./examples/main.lua

.PHONY: con
con:
	nc 127.0.0.1 7000

.PHONY: test
test: build test-core test-adapters

.PHONY: test-adapters
test-adapters:
	@./build/adapters/tests/ultron_adapters_tests

.PHONY: test-core
test-core:
	@./build/core/tests/ultron_core_tests
