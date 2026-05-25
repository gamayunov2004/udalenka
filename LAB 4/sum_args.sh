#!/usr/bin/env bash

if [[ $# -lt 2 || $# -gt 5 ]]; then
  echo "Ошибка: нужно передать от 2 до 5 чисел"
  exit 1
fi

sum=0

for value in "$@"; do
  if ! [[ "$value" =~ ^-?[0-9]+$ ]]; then
    echo "Ошибка: '$value' не является целым числом"
    exit 2
  fi
  sum=$((sum + value))
done

echo "Сумма: $sum"
exit 0
