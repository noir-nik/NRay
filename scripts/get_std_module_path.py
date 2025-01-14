import json
import subprocess
import os

STD_MODULE_SRC_NAME = "STD_MODULE_SRC"
JSON_FILE_NAME = "libc++.modules.json"

def get_std_module_source_path():
    result = subprocess.run(['clang++', f'-print-file-name={JSON_FILE_NAME}'], stdout=subprocess.PIPE)
    
    if result.returncode == 0:
        json_path = result.stdout.decode('utf-8').strip()
        if not os.path.exists(json_path):
            return ""
        with open(json_path, 'r') as file:
            data = json.load(file)
            for module in data.get('modules', []):
                if module.get('logical-name') == 'std':
                    source_path = module.get('source-path', '')
                    source_path = os.path.abspath(os.path.join(os.path.dirname(json_path), source_path))
                    source_path = os.path.split(source_path)[0]
                    source_path = source_path.replace('\\', '/').replace(' ', '\\ ')
                    return source_path
    return ""

if __name__ == "__main__":
    print(f'{STD_MODULE_SRC_NAME} := {get_std_module_source_path()}')

