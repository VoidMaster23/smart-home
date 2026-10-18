.PHONY: all build up logs shell clean rebuild lint

# Default
all: build up logs

# 1. Build (Python script runs INSIDE the Docker build process)
build:
	@echo "🔌 allowing Docker to access the local display..."
	export DISPLAY=:0 && xhost +local:docker
	@echo "🚀 Building and starting containers..."
	docker compose up -d --build


# 2. Start (No rebuild)
up:
	docker compose up -d

# 3. Watch Logs
logs_controller:
	docker compose logs -f controller

controller:
	docker compose build controller && docker compose up  -d controller

lint:
	python3 generate_sources.py && cmake -S . -B build
	clang-format --dry-run --Werror $(shell git ls-files '*.cc' '*.cpp' '*.h' '*.hpp')
	run-clang-tidy -p build
	cpplint --recursive src/

# 4. Stop
down:
	docker compose down

# 5. Enter Container
shell:
	docker compose exec controller bash

# 6. Clean
clean:
	docker compose down -v
	rm -rf src/build src/sources.cmake src/headers.cmake

rebuild: clean build logs