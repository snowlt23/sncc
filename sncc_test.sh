
rettest() {
  echo $2 | ./sncc > main.s
  gcc -otest.out main.s
  ./test.out
  RETCODE=$?
  if [ $RETCODE = $3 ]; then
    echo "[OK] $1"
  else
    echo "[ERROR] $1, expected $3, but got $RETCODE"
  fi
}

rettest "retcode: 2" "2" 2
rettest "retcode: 9" "9" 9
rettest "retcode: 1 + 2" "1 + 2" 3
rettest "retcode: 2 - 1" "2 - 1" 1
rettest "retcode: 1 + 10 - 2" "1 + 10 - 2" 9
