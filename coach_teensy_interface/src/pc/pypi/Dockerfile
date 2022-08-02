from quay.io/pypa/manylinux2014_x86_64:latest

RUN yum -y install openssl-devel wget opencv-devel

RUN cd && \
    wget https://github.com/Kitware/CMake/releases/download/v3.18.2/cmake-3.18.2.tar.gz && \
    tar -xvf cmake-3.18.2.tar.gz && \
    cd cmake-3.18.2 && \
    ./bootstrap --parallel=$(nproc --all) && \
    make -j$(nproc --all) && \
    make install && \
    cd && \
    rm -Rf cmake-3.18.2.tar.gz && rm -Rf cmake-3.18.2

RUN cd && \
    git clone https://github.com/pybind/pybind11 && \
    cd pybind11 && mkdir build && cd build && \
    for PY in /opt/python/*; do cmake -DCMAKE_PREFIX_PATH="${PY}" -DBUILD_TESTING=0 -DCMAKE_INSTALL_PREFIX="${PY}" ..; make install; rm -f CMakeCache.txt; rm -Rf CMakeFiles; done

RUN cd && \
    git clone https://github.com/jaraco/zipp && cd zipp && git checkout v3.1.0 && \
    /opt/python/cp37-cp37m/bin/python setup.py install && \
    cd

RUN /opt/python/cp37-cp37m/bin/pip install twine
