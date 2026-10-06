import importlib.util
from pathlib import Path
import subprocess
import tempfile
import unittest

SCRIPT = Path(__file__).resolve().parents[1] / 'release_policy.py'
spec = importlib.util.spec_from_file_location('release_policy', SCRIPT)
policy = importlib.util.module_from_spec(spec)
spec.loader.exec_module(policy)


class VersionPolicyTests(unittest.TestCase):
    def test_numeric_comparison(self):
        policy.require_bump('2.0.10', '2.0.9')
        policy.require_bump('3.0.0', '2.99.99')
        for current in ('2.0.9', '2.0.8', '1.99.99'):
            with self.assertRaises(ValueError):
                policy.require_bump(current, '2.0.9')

    def test_cmake_version(self):
        self.assertEqual(policy.cmake_version(
            '# project(aircraft_simulation_core VERSION 9.0.0)\n'
            'project(aircraft_simulation_core VERSION 2.0.1 LANGUAGES CXX)'), '2.0.1')
        for version in ('2.0.1.1', '02.0.1', '${VERSION}', '2.0.1-rc.1'):
            with self.assertRaises(ValueError):
                policy.cmake_version(f'project(aircraft_simulation_core VERSION {version})')

    def test_tag_identity(self):
        self.assertFalse(policy.validate_tag('2.0.1', '2.0.1'))
        self.assertTrue(policy.validate_tag('2.0.1-rc.12', '2.0.1'))
        for tag in ('v2.0.1', '2.0.2', '2.0.1.1', '2.0.1-rc.0', '2.0.1-rc.01',
                    '2.0.1-beta.1', '2.0.1-rc.1\n'):
            with self.assertRaises(ValueError):
                policy.validate_tag(tag, '2.0.1')

    def test_stable_requires_main_but_rc_can_be_on_branch(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)

            def git(*args):
                subprocess.run(['git', *args], cwd=root, check=True, capture_output=True)

            git('init', '-b', 'main')
            git('config', 'user.email', 'test@example.com')
            git('config', 'user.name', 'Policy Test')
            cmake = root / 'CMakeLists.txt'
            cmake.write_text('project(aircraft_simulation_core VERSION 2.0.0)')
            git('add', '.')
            git('commit', '-m', 'Initial')
            git('checkout', '-b', 'candidate')
            cmake.write_text('project(aircraft_simulation_core VERSION 2.0.1)')
            git('commit', '-am', 'Candidate')
            git('tag', '2.0.1-rc.1')
            git('tag', '2.0.1')

            def check(tag):
                return subprocess.run(['python3', str(SCRIPT), '--tag', tag, '--main-ref', 'main'],
                                      cwd=root, capture_output=True, text=True)

            self.assertEqual(check('2.0.1-rc.1').returncode, 0)
            self.assertNotEqual(check('2.0.1').returncode, 0)
            git('checkout', 'main')
            git('merge', '--ff-only', 'candidate')
            self.assertEqual(check('2.0.1').returncode, 0)
            git('checkout', 'HEAD~1')
            self.assertNotEqual(check('2.0.1').returncode, 0)


if __name__ == '__main__':
    unittest.main()
