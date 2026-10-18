import os

def collect_files(search_dir, extensions):
    files = []
    if not os.path.exists(search_dir):
        return files

    for root, _, file_list in os.walk(search_dir):
        for file in file_list:
            if any(file.endswith(ext) for ext in extensions):
                full_path = os.path.join(root, file)
                # CRITICAL: Relativize to ROOT (.) so output is "src/main.cpp"
                rel_path = os.path.relpath(full_path, start=".").replace("\\", "/")
                files.append(rel_path)
    files.sort()
    return files

def generate_cmake_list(var_name, files, output_file):
    with open(output_file, "w") as f:
        f.write(f"set({var_name}\n")
        for file in files:
            f.write(f"    {file}\n")
        f.write(")\n")

if __name__ == "__main__":
    # Scan src for .cpp
    cpp_files = collect_files("src", [".cpp"])
    # Scan src/include for headers
    h_files = collect_files("src/include", [".h", ".hpp"])

    # Output to ROOT
    generate_cmake_list("SOURCES", cpp_files, "sources.cmake")
    generate_cmake_list("HEADERS", h_files, "headers.cmake")