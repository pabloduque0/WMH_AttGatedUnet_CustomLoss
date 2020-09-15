FROM ubuntu:18.04

RUN apt-get update && apt-get -y install \
        python3 \
        python3-dev \
        git \
        wget \
        unzip \
        cmake \
        build-essential \
        pkg-config \
        libatlas-base-dev \
        gfortran \
        libgtk2.0-dev \
        libavcodec-dev \
        libavformat-dev \
        libswscale-dev \
        libjpeg-dev \
        libpng-dev \
        libtiff-dev \
        libv4l-dev \
        python3-pip \
        python3-setuptools \
        g++ \
        liblapack-dev \
        libblas-dev \
        libboost-dev \
        libarmadillo-dev \
        libpulse-dev \
        qtcreator \
        qt5-default \
        libnss3 \
        libasound2 \
        libglu1-mesa \
        libinsighttoolkit4-dev \
        libgl1-mesa-dev \
        libqt5webengine5 \
        libqt5webenginewidgets5 \
        curl

RUN pip3 install --upgrade pip && pip3 install --upgrade setuptools==41.0.0 && python3 -m pip install \
                                                                                              numpy \
                                                                                              SimpleITK \
                                                                                              scipy \
                                                                                              sklearn \
                                                                                              opencv-python \
                                                                                              matplotlib \
                                                                                              itk \
                                                                                              tensorflow \
                                                                                              keras \
                                                                                              tensorflow_addons

RUN git clone "https://github.com/MIC-DKFZ/HD-BET" && cd HD-BET && pip3 install -e .

ADD coregister_app/ coregister_app/
ADD edison/ edison/
ADD edison_app/ edison_app/

ADD slicer/ slicer/

RUN cd /coregister_app && /bin/bash -c qmake coregister_app.pro && /bin/bash -c make && cd ..
RUN cd /edison && /bin/bash -c qmake edison.pro && /bin/bash -c make && cd ..
RUN cd /edison_app && /bin/bash -c qmake edison_app.pro && /bin/bash -c make && cd ..

ADD src/ wmhseg_example/
