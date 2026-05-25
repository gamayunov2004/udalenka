#!/usr/bin/env bash

avgfile() {
  local dir_path="$1"

  if [[ -z "$dir_path" ]]; then
    echo "Ошибка: нужно передать путь к директории"
    exit 1
  fi

  if [[ ! -d "$dir_path" ]]; then
    echo "Ошибка: директория '$dir_path' не существует"
    exit 2
  fi

  local total_size=0
  local file_count=0
  local item
  local file_size

  for item in "$dir_path"/*; do
    if [[ -f "$item" && ! -L "$item" ]]; then
      file_size=$(stat -c "%s" "$item")
      total_size=$((total_size + file_size))
      file_count=$((file_count + 1))
    fi
  done

  if [[ "$file_count" -eq 0 ]]; then
    echo "В директории нет обычных файлов для подсчета"
    exit 3
  fi

  echo "Количество файлов: $file_count"
  echo "Суммарный размер: $total_size байт"
  echo "Средний размер файла: $((total_size / file_count)) байт"
}

avgfile "$1"
