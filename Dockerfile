# syntax=docker/dockerfile:1.4.1

ARG UE_TAG=4.27.2

FROM ue:${UE_TAG} AS builder
ARG UE_DIRECTORY=/opt/ue

USER root
RUN DEBIAN_FRONTEND=noninteractive apt-get update && \
    apt-get install -y --no-install-recommends \
    libcurl4-openssl-dev \
    make \
    cmake \
    jq && \
    rm -rf /var/lib/apt/lists/*
RUN adduser --system --group --home /opt/uebox uebox
RUN mkdir -p /opt/projects && chown -R uebox:uebox /opt/projects
USER uebox

RUN mkdir -p /opt/projects/uebox
COPY --chown=uebox:uebox ./ ./

RUN ./setup.sh -u ${UE_DIRECTORY}
RUN cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release .
RUN cmake --build . --target UEBox-Linux-Test
RUN ${UE_DIRECTORY}/Engine/Build/BatchFiles/RunUAT.sh BuildCookRun -clean -compile -cook -stage -package -pak -nop4 -ue4exe=UE4Editor-Cmd -archive -archivedirectory=Game -clientconfig=Test -targetplatform=Linux -project=/opt/projects/uebox/UEBox.uproject

FROM adamrehn/ue4-runtime:20.04-cudagl11.0
USER root
RUN deluser --remove-home ue4
RUN addgroup --system --gid 1000 uebox && \
    adduser --system --home /opt/uebox --uid 1000 --gid 1000 uebox && \
    usermod -a -G audio,video,pulse,pulse-access uebox
USER uebox
WORKDIR /opt/uebox
COPY --from=builder --chown=uebox:uebox /opt/projects/uebox/Game/LinuxNoEditor ./
