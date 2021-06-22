import os.path

import setuptools


rootdir = os.path.abspath(os.path.join(__file__, '..', '..', '..'))
srcdir = os.path.join(rootdir, 'src')


setuptools.setup(
    name='openinput-testsuite',
    modules=[
        'testsuite',
        'pages',
    ],
    ext_modules=[
        setuptools.Extension(
            name='_testsuite',
            sources=['_testsuite.c'],
            extra_compile_args=[f'-I{srcdir}'],
            extra_link_args=['-ltestsuite'],
        )
    ],
)
