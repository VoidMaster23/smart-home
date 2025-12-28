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
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

RUN cd /usr/src/googletest \
    && mkdir -p build && cd build \
    && cmake .. \
    && make -j"$(nproc)" \
    && cp lib/*.a /usr/lib/ \
    && ldconfig

RUN git clone https://github.com/eclipse/paho.mqtt.cpp.git \
    && cd paho.mqtt.cpp \
    && mkdir build && cd build \
    && cmake .. -DPAHO_BUILD_DOCUMENTATION=FALSE -DPAHO_BUILD_SAMPLES=FALSE \
    && make -j$(nproc) \
    && make install \
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

COPY --from=builder /usr/local/lib/libpaho-mqttpp3.so.1 /usr/lib/
RUN ldconfig

WORKDIR /app
COPY --from=builder /app/build/controller .

CMD [ "./controller" ]