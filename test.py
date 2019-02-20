from seekgzip import reader
import gzip


def generate_txt_file():
  test_gzip = "testfile.gz"
  with gzip.open(test_gzip, "wt+") as g:
    for i in range(10000):
      g.write(str(i) + "\n")


def generate_bin_file():
  test_gzip = "testbinfile.gz"
  c = b"\x00"
  with gzip.open(test_gzip, "wb+") as g:
    for i in range(10000):
      g.write(c)


def test_readline():
  r = reader("./testfile.gz")
  expected_num = 0
  while True:
    line = r.readline()
    if not line:
      break
    assert line == str(expected_num)
    expected_num += 1

  assert expected_num == 10000


def test_seek_whence_0():
  r = reader("./testfile.gz")
  r.seek(4)
  s = r.read(2)
  assert s == "2\n"


def test_seek_whence_1():
  r = reader("./testfile.gz")
  r.read(2)
  r.read(2)
  r.seek(-4, 1)
  s = r.read(2)
  assert s == "0\n"


def test_seek_whence_2():
  r = reader("./testfile.gz")
  r.seek(5, 2)
  s = r.read(5)
  assert s == "9999\n"


def test_read_bin():
  r = reader("./testbinfile.gz")
  data = r.read(2)
  assert data == "\x00\x00"


def main():
  generate_txt_file()
  test_readline()
  test_seek_whence_0()
  test_seek_whence_1()
  test_seek_whence_2()
  generate_bin_file()
  test_read_bin()


if __name__ == "__main__":
  main()
