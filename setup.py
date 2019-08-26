#!/usr/bin/env python

import os
import platform
import setuptools
from setuptools import setup


# Detailed description from README.md
def readme():
    with open('README.md') as f:
        return f.read()


# Platform-dependent options
print(platform.system())
if platform.system()=='Linux':
    opts = dict(
            include_package_data=True,
            data_files = [(d, [os.path.join(d,f) for f in files]) for d, folders, files in os.walk('share') if files],
        )
elif platform.system()=='Darwin':
    opts = dict(
            data_files = ['manual.pdf'],
            app = ['mineralapp/mineralapp.py'],
            options = {'py2app': {
                            'packages' : ['PIL', 'ttkthemes'],
                            'excludes' : ['numpy'],
                            'iconfile' : 'icon/mineralapp.icns',
                    }
                },
            setup_requires=['py2app'],
        )
else:
    opts = dict()

print(opts)

# Setup!
setup(
    name = 'MineralApp',
    version = '1.1',
    description = 'Mineral collection manager',
    long_description = readme(),
    url = 'https://github.com/SimoneCnt/MineralApp',
    author = 'Simone Conti',
    author_email = 'simonecnt@gmail.com',
    license = 'GPLv3',
    packages = setuptools.find_packages(),
    entry_points = {
        'gui_scripts' : [ 'mineralapp = mineralapp.mineralapp:main_gui' ]
    },
    **opts,
)

