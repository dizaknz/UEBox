# syntax=docker/dockerfile:1.4.1

ARG USER=ue
ARG USER_HOME=/opt/ue
ARG UE_DIRECTORY=/opt/ue
# full or minimal
ARG BUILD_TYPE=full

FROM ubuntu:20.04 AS builder-source
ARG USER
ARG USER_HOME
ARG UE_DIRECTORY
ARG BUILD_TYPE

USER root
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
      git build-essential xdg-user-dirs xdg-utils \
      wget curl && \
  rm -rf /var/lib/apt/lists/*

RUN adduser --system --group --home ${USER_HOME} ${USER}

RUN mkdir -p ${UE_DIRECTORY}
RUN chown -R ${USER}:${USER} ${UE_DIRECTORY}

USER ${USER}

# copy source in
COPY --from=ue_source_dir --chown=${USER}:${USER} ./ ${UE_DIRECTORY}

ENV UE_DIRECTORY ${UE_DIRECTORY}
WORKDIR ${UE_DIRECTORY}
RUN ls -ltr Engine/
RUN bash -c "pushd ${UE_DIRECTORY} > /dev/null" && ./Setup.sh && ./GenerateProjectFiles.sh -makefile
RUN [ "${BUILD_TYPE}" = "full" ] && { echo "Performing full engine build"; make StandardSet; } || { echo "Performing minimal engine build"; make RequiredTools; } 
RUN make UnrealPak && bash -c "popd > /dev/null"

# Build installed UE
FROM builder-source AS installed
ARG USER
ARG UE_DIRECTORY

USER ${USER}

# installed build
RUN mkdir -p /opt/projects/ue-installed/
RUN ${UE_DIRECTORY}/Engine/Build/BatchFiles/RunUAT.sh BuildGraph -target="Make Installed Build Linux" \
      -script=Engine/Build/InstalledEngineBuild.xml -set:HostPlatformOnly=true -set:WithLinuxAArch64=false \
      -set:WithDDC=false -set:HostPlatformDDCOnly=true -set:GameConfigurations="Test" \
      -set:BuiltDirectory="/opt/projects/ue-installed"

RUN rm -rf ${UE_DIRECTORY}/*; mv /opt/projects/ue4-installed/Linux/* ${UE_DIRECTORY}/
WORKDIR ${UE_DIRECTORY}

# Copy UE
FROM ubuntu:20.04 as final
ARG USER
ARG USER_HOME
ARG UE_DIRECTORY

RUN adduser --system --group --home ${USER_HOME} ${USER}

RUN mkdir -p ${UE_DIRECTORY}
RUN chown -R ${USER}:${USER} ${UE_DIRECTORY}

USER ${USER}

COPY --from=installed --chown=${USER}:${USER} ${UE_DIRECTORY} ${UE_DIRECTORY}
COPY --from=installed --chown=${USER}:${USER} ${USER_HOME}/.config/Epic/UnrealEngine ${USER_HOME}/.config/Epic/UnrealEngine

ENV UE_DIRECTORY ${UE_DIRECTORY}
WORKDIR ${UE_DIRECTORY}
