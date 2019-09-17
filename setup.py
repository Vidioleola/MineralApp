#!/usr/bin/env python

import os
import platform
import setuptools


# Detailed description from README.md
def readme():
    with open('README.md') as f:
        return f.read()


# Platform-dependent options
setup_requires = ["pillow", "ttkthemes", "reportlab"]
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
                            'packages' : ['PIL', 'ttkthemes', 'reportlab'],
                            'excludes' : ['numpy'],
                            'iconfile' : 'icon/mineralapp.icns',
                    }
                },
        )
    setup_requires += ['py2app'],
else:
    opts = dict()


# Version
with open('mineralapp/version.py') as fp:
    version = "".join(fp.readlines()).split('=')[1].strip().strip('"')


# Setup!
setuptools.setup(
    name = 'MineralApp',
    version = version,
    description = 'Mineral collection manager',
    long_description = readme(),
    url = 'https://github.com/SimoneCnt/MineralApp',
    author = 'Simone Conti',
    author_email = 'simonecnt@gmail.com',
    license = 'GPLv3',
    packages = setuptools.find_packages(),
    setup_requires = setup_requires,
    entry_points = {
        'gui_scripts' : [ 'mineralapp = mineralapp.mineralapp:main_gui' ]
    },
    **opts,
)

