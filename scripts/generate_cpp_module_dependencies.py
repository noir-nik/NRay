import os
import re

MODULES_BUILD_DIR = "$(_MBD)"
OBJS_BUILD_DIR = "$(_OBD)"
CPP_MODULE_EXTENSION = ".cppm"
CPP_EXTENSION = ".cpp"
LINES_TO_SCAN = 50

def extract_imports(file_path):
    """Extract module imports from the first 30 lines of a file."""
    imports = []
    with open(file_path, "r") as file:
        for i, line in enumerate(file):
            if i >= LINES_TO_SCAN:
                break

            match = re.match(r"\s*(?:import|module)\s+([a-zA-Z0-9_\.]+)", line)
            if match:
                module_name = match.group(1)
                imports.append(module_name)

            match = re.match(r"\s*export\s+import\s+([a-zA-Z0-9_\.]+)", line)
            if match:
                module_name = match.group(1)
                imports.append(module_name)
    return imports

def find_files_with_extension(directories, extension):
    """Recursively find all files with the given extension in a list of directories."""
    matched_files = []
    for root_dir in directories:
        for root, _, files in os.walk(root_dir):
            for file in files:
                if file.endswith(extension):
                    matched_files.append(os.path.join(root, file))
    return matched_files

def generate_dependencies(files, extension):
    """Generate Makefile-compatible dependency rules."""
    dependencies = []
    for file in files:
        base_name = os.path.splitext(os.path.basename(file))[0]
        imports = extract_imports(file)

        if extension == CPP_MODULE_EXTENSION:
            target = f"{MODULES_BUILD_DIR}/{base_name}.pcm"
        elif extension == CPP_EXTENSION:
            target = f"{OBJS_BUILD_DIR}/{base_name}.o"
        else:
            continue

        deps = " ".join(f"{MODULES_BUILD_DIR}/{imp}.pcm" for imp in imports)
        # deps += f" {file.replace("\\", "/")}"
        rule = f"{target}: {deps}"
        dependencies.append(rule) 
    return dependencies

def main():
    dirs = ["source", "tests"]

    cppm_files = find_files_with_extension(dirs, CPP_MODULE_EXTENSION)
    cpp_files = find_files_with_extension(dirs, CPP_EXTENSION)
    cpp_files.append("main.cpp")

    cppm_dependencies = generate_dependencies(cppm_files, CPP_MODULE_EXTENSION)
    cpp_dependencies = generate_dependencies(cpp_files, CPP_EXTENSION)

    # with open("cpp_module_dependencies.mk", "w") as file:
    #     file.write("# Auto-generated Makefile dependencies\n")
    #     for dep in cppm_dependencies + cpp_dependencies:
    #         file.write(dep + "\n")

    print("# Auto-generated Makefile dependencies")
    for dep in cppm_dependencies + cpp_dependencies:
        print(dep)


if __name__ == "__main__":
    main()

