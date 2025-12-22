import os

def collect_files(source_dir, extensions):
    files = []
    if not os.path.exists(source_dir):
        return files

    for root, _, file_list in os.walk(source_dir):
        for file in file_list:
            if any(file.endswith(ext) for ext in extensions):
                full_path = os.path.join(root, file)
                rel_path = os.path.relpath(full_path, start="src").replace("\\", "/")
                files.append(rel_path)

    files.sort()  # Keep order consistent
    return files

def generate_cmake_list(var_name, files, output_file):
    with open(output_file, "w") as f:
        f.write(f"set({var_name}\n")
        for file in files:
            f.write(f"    {file}\n")
        f.write(")\n")

    print(f"Generated {output_file} ({len(files)} files)")

if __name__ == "__main__":
    cpp_files = collect_files("src", [".cpp"])
    h_files = collect_files("src/include", [".h", ".hpp"])

    generate_cmake_list("SOURCES", cpp_files, "src/sources.cmake")
    generate_cmake_list("HEADERS", h_files, "src/headers.cmake")

    if os.path.exists("tests"):
        test_cpp_files = collect_files("tests", [".cpp"])
        generate_cmake_list("TEST_SOURCES", test_cpp_files, "src/test_sources.cmake")