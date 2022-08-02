# To release a version of the pyplane Python extension module on pypi.org

(Based on Carsten Nielsen's commit message on commit 210f9991)

Make sure the version number in `src/CMakeLists.txt` on the master branch is what you want it to be, and greater than that of any previous release.

First build a docker container:
```
cd src/pc/pypi
sudo docker build -t giveitaname .
```

To run it
```
sudo docker run -it giveitaname /bin/bash
```

Inside the docker container clone the repository from INI's gitlab and run cmake:
```
git clone https://code.ini.uzh.ch/CoACH/CoACH_Teensy_interface.git
cd CoACH_Teensy_interface
mkdir build && cd build
cmake -DCMAKE_PREFIX_PATH=/opt/python/cp35-cp35m/ ../src
```
We need to pass the prefix path because the Dockerfile only installs
PyBind11 for the python3 installations in /opt and not for the system
python which is 2.7. It doesn't matter which /opt/python installation
is chosen.

After that is done we can
```
make uploadwheels
```
which will first execute the `buildwheels` and `repairwheels` targets
before uploading the generated wheels to pypi using twine.

Twine will ask for authentication. This can be prevented by placing a
`.pypirc` in the root user home directory with the following content
which makes twine use your pypi API token.
```
[pypi]
username = __token__
password = YOUR_API_TOKEN
```

Don't forget to tag the release in the repository using a tag that matches the version number, and if tagging locally, don't forget to push the tag to the server.
