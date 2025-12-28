# -----------------------------------------------------------------------------
# STAGE 1: BUILDER
# -----------------------------------------------------------------------------
FROM ubuntu:latest AS builder

# install-deps
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    python3 \
    libpaho-mqtt-dev \
    nlohmann-json3-dev \
    libgtest-dev \
    libssl-dev \
    qt6-base-dev \
    qt6-declarative-dev \
    libgl1-mesa-dev \
    && rm -rf /var/lib/apt/lists/*

# Build GoogleTest
RUN cd /usr/src/googletest \
    && mkdir -p build && cd build \
    && cmake .. \
    && make -j"$(nproc)" \
    && cp lib/*.a /usr/lib/ \
    && ldconfig

# Build Paho MQTT C++
RUN git clone https://github.com/eclipse/paho.mqtt.cpp.git \
    && cd paho.mqtt.cpp \
    && mkdir build && cd build \
    && cmake .. -DPAHO_BUILD_DOCUMENTATION=FALSE -DPAHO_BUILD_SAMPLES=FALSE \
    && make -j$(nproc) \
    && make install \
    && ldconfig

WORKDIR /app

COPY generate_sources.py .
COPY src/ src/

# 3. Run Source Generation
RUN python3 generate_sources.py

# 4. Build with CMake
RUN mkdir build && cd build \
    && cmake ../src \
    && make -j$(nproc)


# -----------------------------------------------------------------------------
# STAGE 2: RUNNER (The Final Image)
# -----------------------------------------------------------------------------
FROM ubuntu:latest

# Install Runtime Dependencies
RUN apt-get update && apt-get install -y \
    # 1. Paho C Runtime
    libpaho-mqtt-dev \
    # 2. Qt6 Core Libraries
    libqt6quick6 \
    libqt6widgets6 \
    libqt6gui6 \
    libqt6core6 \
    # 3. Qt6 QML Modules
    qml6-module-qtquick \
    qml6-module-qtquick-controls \
    qml6-module-qtquick-layouts \
    qml6-module-qtqml \
    qml6-module-qtqml-workerscript \
    qml6-module-qtquick-templates \
    libqt6quicktemplates2-6 \
    # 4. Graphics Drivers
    libgl1 \
    libfontconfig1 \
    && rm -rf /var/lib/apt/lists/*

# Copy the Paho C++ Lib we built manually in Stage 1
COPY --from=builder /usr/local/lib/libpaho-mqttpp3.so.1 /usr/lib/
RUN ln -s /usr/lib/libpaho-mqttpp3.so.1 /usr/lib/libpaho-mqttpp3.so && ldconfig

WORKDIR /app
COPY --from=builder /app/build/controller .

CMD [ "./controller" ]