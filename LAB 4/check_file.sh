#!/usr/bin/env bash

if [[ $# -ne 1 ]]; then
  echo "Ошибка: нужно передать путь к файлу"
  exit 1
fi

file_path="$1"

if [[ -f "$file_path" ]]; then
  echo "Файл '$file_path' существует"
  exit 0
fi

echo "Файл '$file_path' не найден"
read -r -p "Создать файл? (y/n): " answer

case "$answer" in
  y|Y|yes|YES|д|Д)
    touch "$file_path"
    echo "Файл '$file_path' создан"
    ;;
  *)
    echo "Файл не был создан"
    exit 2
    ;;
esac

exit 0
