import os


def normalize_path(path):
    """Normalize path to use forward slashes, regardless of OS."""
    return os.path.abspath(path).replace(os.path.sep, "/")


def count_lines(file_path):
    """Count number of lines in a file, ignoring errors."""
    try:
        with open(file_path, "r", encoding="utf-8", errors="ignore") as f:
            return sum(1 for _ in f)
    except Exception as e:
        print(f"Warning: Could not read {file_path}: {e}")
        return 0


def scan_directory(base_dir, exclude_patterns):
    base_dir = normalize_path(base_dir)

    cpp_files = 0
    cpp_lines = 0
    py_files = 0
    py_lines = 0

    for root, dirs, files in os.walk(base_dir):
        root_norm = normalize_path(root)

        for file in files:
            full_path = f"{root_norm}/{file}"
            if exclude_patterns in full_path:
                continue

            ext = os.path.splitext(file)[1]
            if ext in [".cpp", ".hpp"]:
                cpp_files += 1
                cpp_lines += count_lines(full_path)
            elif ext == ".py":
                py_files += 1
                py_lines += count_lines(full_path)

    return cpp_files, cpp_lines, py_files, py_lines


def main():
    directory = "../"
    exclude = "thirdparty"

    abs_dir = normalize_path(directory)
    print(f"Scanning directory: {abs_dir}")

    cpp_files, cpp_lines, py_files, py_lines = scan_directory(abs_dir, exclude)

    total_files = cpp_files + py_files
    total_lines = cpp_lines + py_lines

    print(f"\nC++ files (.cpp/.hpp): {cpp_files} files, {cpp_lines} lines")
    print(f"Python files (.py):    {py_files} files, {py_lines} lines")
    print(f"----------------------------------------")
    print(f"Grand total:           {total_files} files, {total_lines} lines")


if __name__ == "__main__":
    main()
