#build 
FROM ubuntu:latest AS builder

#install-deps
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    python3 \
    libpaho-mqtt-dev \
    nlohmann-json3-dev \
    libgtest-dev \
    && rm -rf /var/lib/apt/lists/*

RUN cd /usr/src/googletest \
    && mkdir -p build && cd build \
    && cmake .. \
    && make -j"$(nproc)" \
    && cp lib/*.a /usr/lib/ \
    && ldconfig

WORKDIR /app

#copy source code
COPY generate_sources.py .
COPY src/ src/

# 3. Run Source Generation
RUN python3 generate_sources.py

# 4. Build with CMake
RUN mkdir build && cd build \
    && cmake ../src \
    && make -j$(nproc)


FROM ubuntu:latest

RUN apt-get update && apt-get install -y \
    libpaho-mqtt-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY --from=builder /app/build/controller .

CMD [ "./controller" ]