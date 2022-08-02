#!/bin/bash

for WHEEL in $1/*.whl; do
    auditwheel repair $WHEEL -w wheelhouse
done
