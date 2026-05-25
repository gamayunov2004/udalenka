#!/usr/bin/env bash

if [[ $# -ne 1 ]]; then
  echo "Ошибка: нужно передать одно число N"
  exit 1
fi

n="$1"

if ! [[ "$n" =~ ^[0-9]+$ ]] || [[ "$n" -eq 0 ]]; then
  echo "Ошибка: N должно быть положительным целым числом"
  exit 2
fi

for ((i = 1; i <= n; i += 2)); do
  echo "$i"
done

exit 0
