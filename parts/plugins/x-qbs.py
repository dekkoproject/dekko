import os
import snapcraft
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

        buildProfile = 'snapcraft'+self.options.qt_version
        
        self.run(['qbs', 'setup-toolchains', '--detect'], env=env)
        
        self.run(['qbs', 'setup-qt', '/usr/bin/qmake', buildProfile], env=env)
        
        if self.options.profile == 'clang':
            self.run(['qbs', 'config', 
                      'profile.'+ buildProfile + '.baseProfile', 
                      self.options.profile], 
                      env=env)
            
        self.run(['qbs', 'build',
                  '-d', self.builddir,
                  '-f', self.sourcedir,
                  '--show-progress',
                  self.options.build_variant,
                  'qbs.installRoot:' + self.installdir, 
                  'profile:' + buildProfile] + self.options.options, 
                 env=env)

    def _build_environment(self):
        env = os.environ.copy()
        env['QT_SELECT'] = self.options.qt_version
        return env
        
        
