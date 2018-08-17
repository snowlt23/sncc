char EOF = -1;

int _WSTATUS(int status) {
  return status & 127;
}

int WIFEXITED(int status) {
  return _WSTATUS(status) == 0;
}

int WIFSIGNALED(int status) {
  return (_WSTATUS(status) != 127) && (_WSTATUS(status) != 0);
}
