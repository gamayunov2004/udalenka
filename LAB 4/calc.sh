#!/usr/bin/env bash

read -r -p "Введите первое целое число: " a
read -r -p "Введите второе целое число: " b
read -r -p "Введите операцию (+, -, *, /): " op

if ! [[ "$a" =~ ^-?[0-9]+$ ]] || ! [[ "$b" =~ ^-?[0-9]+$ ]]; then
  echo "Ошибка: оба значения должны быть целыми числами"
  exit 1
fi

case "$op" in
  +)
    result=$((a + b))
    ;;
  -)
    result=$((a - b))
    ;;
  \*)
    result=$((a * b))
    ;;
  /)
    if [[ "$b" -eq 0 ]]; then
      echo "Ошибка: деление на ноль"
      exit 2
    fi
    result=$((a / b))
    ;;
  *)
    echo "Ошибка: неизвестная операция"
    exit 3
    ;;
esac

echo "Результат: $result"
exit 0
