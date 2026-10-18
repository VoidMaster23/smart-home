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
	cd controller && python3 generate_sources.py && cmake -S . -B build
	clang-format --dry-run --Werror $(shell git ls-files 'controller/*.cc' 'controller/*.cpp' 'controller/*.h' 'controller/*.hpp')
	run-clang-tidy -p controller/build
	cpplint --recursive controller/src
	cd frontend && npm run lint

clean:
	docker compose down -v
	rm -rf controller/build controller/sources.cmake controller/headers.cmake

# 4. Stop
down:
	docker compose down

# 5. Enter Container
shell:
	docker compose exec controller bash

rebuild: clean build logs