import sys
import subprocess

STD_MODULE_SRC_NAME = "STD_MODULE_SRC"

def get_clang_path():
    if sys.platform == "win32":
        result = subprocess.run(['where', 'clang'], stdout=subprocess.PIPE)
    else:
        result = subprocess.run(['which', 'clang'], stdout=subprocess.PIPE)

    if result.returncode == 0:
        path = result.stdout.decode('utf-8').splitlines()[0]
        path = path.replace('\\', '/').replace('.exe', '').replace(' ', '\\ ')
        return path.replace('/bin/clang', '/share/libc++/v1')
    else:
        return ""

if __name__ == "__main__":
    print(f'{STD_MODULE_SRC_NAME} := {get_clang_path()}')
