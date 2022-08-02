# Copy all files needed to build a wheel.
configure_file(pyplane/__init__.py ${CMAKE_CURRENT_BINARY_DIR}/pyplane/__init__.py COPYONLY)
configure_file(setup.cfg ${CMAKE_CURRENT_BINARY_DIR}/setup.cfg COPYONLY)
configure_file(util/build-wheels.sh ${CMAKE_CURRENT_BINARY_DIR}/ COPYONLY)
configure_file(util/repair-wheels.sh ${CMAKE_CURRENT_BINARY_DIR}/ COPYONLY)

# This configure_file call will replace the @PROJECT_VERSION@ in setup.py.in with the current
# plane project version defined in CMake.
configure_file(setup.py.in ${CMAKE_CURRENT_BINARY_DIR}/setup.py @ONLY)

# This gets a little hacky. Because we need to create a package for each python
# version we need to run CMake multiple times to pick up and configure the build
# against each version of python one at a time.
#
# First we are going to define a command to build a single wheel.
# The remove_directory is necessary to start the setuptools from a clean slate
# every time we build a wheel. Otherwise it will try to put multiple libraries into
# a single wheel.
# We then copy our generated library file into the pyplane directory that will be packaged.
# The pyplane directory also contains the __init__.py from earlier and its contents will be
# placed into the end users site-packages directory.
# The $<...> magic are CMake Generator Expressions. There may be a better way to get the name
# of a generated file, but I couldn't find it.
add_custom_target(buildwheel DEPENDS pyplane)
add_custom_command(TARGET buildwheel POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E remove_directory build/
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:pyplane> pyplane/
		   COMMAND ${PYTHON_EXECUTABLE} setup.py bdist_wheel --soname $<TARGET_FILE_PREFIX:pyplane>$<TARGET_FILE_BASE_NAME:pyplane>$<TARGET_FILE_SUFFIX:pyplane>
                   )

# Next we define a command that builds all wheels. This calls a small utility script that
# deletes the CMakeCache to build cleanly for each python version.
add_custom_target(buildwheels)
add_custom_command(TARGET buildwheels POST_BUILD
                   COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/util/build-wheels.sh
                   WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                   )

# After building the wheels we must use the auditwheel utility to repair them.
# Auditwheel will edit the module name to the format required by pypi and bundle
# any non-base system libraries into the wheel.
add_custom_target(repairwheels DEPENDS buildwheels)
add_custom_command(TARGET repairwheels POST_BUILD
                   COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/util/repair-wheels.sh ${CMAKE_CURRENT_BINARY_DIR}/dist
                   )

# Finally we can upload the wheels. There must either be a .pypirc file in the home directory with an API
# token or we can authenticate with username and password when the command runs.
add_custom_target(uploadwheels DEPENDS repairwheels)
add_custom_command(TARGET uploadwheels POST_BUILD
                   COMMAND /opt/python/cp37-cp37m/bin/python -m twine upload wheelhouse/*
                   )
