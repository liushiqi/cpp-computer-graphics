FROM nvidia/opengl:1.2-glvnd-devel

ENV NVIDIA_DRIVER_CAPABILITIES ${NVIDIA_DRIVER_CAPABILITIES},display

RUN ln -fs /usr/share/zoneinfo/Asia/Shanghai /etc/localtime

RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
    mesa-utils \
    gcc \
    g++ \
    cmake \
    git \
    libglfw3 \
    libglfw3-dev

WORKDIR /usr/src/graphics/

COPY . .

RUN cmake -DCMAKE_BUILD_TYPE=Release -S /usr/src/graphics -B /usr/src/graphics/build

RUN make -C /usr/src/graphics/build computer_graphics -j8

ENTRYPOINT [ "/usr/src/graphics/build/computer_graphics" ]
