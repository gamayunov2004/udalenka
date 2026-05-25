import re
import sys

if len(sys.argv) != 3:
    print("Usage: python3 calc_syscall_percent.py TRACE_FILE PROGRAM_TIME_SEC")
    sys.exit(1)

trace_file = sys.argv[1]
program_time = float(sys.argv[2])
total_syscall_time = 0.0

with open(trace_file, "r", encoding="utf-8", errors="ignore") as f:
    for line in f:
        match = re.search(r"<([0-9.]+)>", line)
        if match:
            total_syscall_time += float(match.group(1))

percent = total_syscall_time / program_time * 100
print(f"Суммарное время системных вызовов: {total_syscall_time:.6f} сек")
print(f"Общее время программы: {program_time:.2f} сек")
print(f"Доля времени системных вызовов: {percent:.4f}%")
