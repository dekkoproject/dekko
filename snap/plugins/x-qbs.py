import os
import snapcraft
import subprocess
from snapcraft import common

class QbsPlugin(snapcraft.BasePlugin):

    @classmethod
    def schema(cls):
        schema = super().schema()
        schema['properties']['options'] = {
            'type': 'array',
            'minitems': 1,
            'uniqueItems': True,
            'items': {
                'type': 'string',
            },
            'default': [],
        }
        
        schema['properties']['profile'] = {
            'enum': ['gcc', 'clang'],
            'default': 'gcc',
        }
        
        schema['properties']['build-variant'] = {
            'enum': ['debug', 'release'],
            'default': 'release',
        }   
        
        schema['properties']['qt-version'] = {
            'enum': ['qt4', 'qt5'],
        }
        
        # Qt version must be specified
        schema['required'].append('qt-version')

        # Inform Snapcraft of the properties associated with building and
        # pulling so it can mark those steps dirty if they change in the YAML.
        schema['build-properties'].append('options')
        schema['pull-properties'].append('qt-version')

        return schema

    def __init__(self, name, options, project):
        super().__init__(name, options, project)

        self.build_packages.append('qbs')
        self.build_packages.append('gcc')
        self.build_packages.append('clang')
        if self.options.qt_version == 'qt5':
            self.build_packages.extend(['qt5-qmake', 'qtbase5-dev'])
        elif self.options.qt_version == 'qt4':
            self.build_packages.extend(['qt4-qmake', 'libqt4-dev'])
        else:
            raise RuntimeError('Unsupported Qt version: {!r}'.format(
                self.options.qt_version))

    def build(self):
        super().build()

        env = self._build_environment()
        # TODO: is this already defined in snapcraft somewhere?
        arch = subprocess.getoutput("dpkg-architecture -qDEB_HOST_MULTIARCH")

        # a unique'ish name for snap builds. Hopefully this shouldn't clash with
        # any other local profiles.
        buildProfile = 'snapcraft'+self.options.qt_version
        
        # Setup the toolchains, there will only be gcc or clang by default
        self.run(['qbs', 'setup-toolchains', '--detect'], env=env)
        
        # Setup a qt4 or qt5 profile. It will auto set gcc as it's build profile
        self.run(['qbs', 'setup-qt', '/usr/bin/qmake', buildProfile], env=env)
        
        # Switch buildprofile to clang if required
        if self.options.profile == 'clang':
            self.run(['qbs', 'config', 
                      'profiles.'+ buildProfile + '.baseProfile', 
                      self.options.profile], 
                      env=env)
        # Run the build.
        self.run(['qbs', 'build',
                  '-d', self.builddir,
                  '-f', self.sourcedir,
                  '-j', '4',
                  self.options.build_variant,
                  'qbs.installRoot:' + self.installdir, 
                  'project.binDir:/usr/bin',
                  'project.libDir:/usr/lib/' + arch,
                  'project.qmlDir:/usr/lib/' + arch + '/qt5/qml',
                  'project.dataDir:/usr/share/dekko',
                  'profile:' + buildProfile] + self.options.options, 
                 env=env)

    def _build_environment(self):
        env = os.environ.copy()
        env['QT_SELECT'] = self.options.qt_version
        return env
        
        
