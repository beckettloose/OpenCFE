#!/bin/sh

# Configures the mbed python venv to ignore ssl certs

cat <<__EOF__ >./mbed-os/venv/pip.conf
[global]
trusted-host = pypi.org
               files.pythonhosted.org
__EOF__

