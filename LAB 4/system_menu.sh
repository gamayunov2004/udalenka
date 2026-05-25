#!/usr/bin/env bash

while true; do
  echo ""
  echo "Меню системной информации"
  echo "1. Имя текущего пользователя (USER)"
  echo "2. Домашний каталог (HOME)"
  echo "3. Текущий каталог (PWD)"
  echo "4. Имя скрипта (0)"
  echo "5. PID текущего процесса ($$)"
  echo "6. Количество аргументов (#)"
  echo "7. Код завершения последней команды (?)"
  echo "0. Выход"
  read -r -p "Выберите пункт: " choice

  case "$choice" in
    1) echo "USER=$USER" ;;
    2) echo "HOME=$HOME" ;;
    3) echo "PWD=$PWD" ;;
    4) echo "Имя скрипта: $0" ;;
    5) echo "PID скрипта: $$" ;;
    6) echo "Количество аргументов: $#" ;;
    7) echo "Код завершения последней команды: $?" ;;
    0) echo "Завершение работы"; break ;;
    *) echo "Неизвестный пункт меню" ;;
  esac
done

exit 0
