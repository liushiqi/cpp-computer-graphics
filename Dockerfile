FROM nvidia/opengl:1.2-glvnd-devel

ARG ENABLE_VULKAN=FALSE

ENV NVIDIA_DRIVER_CAPABILITIES ${NVIDIA_DRIVER_CAPABILITIES},display

RUN ln -fs /usr/share/zoneinfo/Asia/Shanghai /etc/localtime

RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
    mesa-utils \
    gcc \
    g++ \
    cmake \
    git \
    python3 \
    libglfw3 \
    libglfw3-dev \
    vulkan-validationlayers-dev

WORKDIR /usr/src/graphics/

COPY . .

RUN cmake -DCMAKE_BUILD_TYPE=Release -S /usr/src/graphics -B /usr/src/graphics/build -DGRAPHICS_USE_VULKAN=$ENABLE_VULKAN

RUN make -C /usr/src/graphics/build computer_graphics -j16

ENTRYPOINT [ "/usr/src/graphics/build/computer_graphics" ]
