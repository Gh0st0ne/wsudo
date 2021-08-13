#include "wsudo/corio.h"

#include <catch2/catch_all.hpp>

#include <sstream>

using namespace wsudo;

TEST_CASE("Coroutine IO", "[corio]") {
  const wchar_t *const gpl3_path = L"..\\LICENSE";
  constexpr size_t gpl3_size_lf = 35149;
  constexpr size_t gpl3_size_crlf = 35823;

  CorIO corio;
  auto fileToken = corio.openForReading(gpl3_path, FILE_FLAG_SEQUENTIAL_SCAN);

  auto task = fileToken.readToEnd();
  task.resume();
  int waitCycles = 0;
  while (!task.await_ready()) {
    if (++waitCycles == 10) {
      FAIL("Coroutine IO took too long.");
      return;
    }
    Sleep(100);
  }
  auto size = task.await_resume().size();
  if (size == gpl3_size_lf || size == gpl3_size_crlf) {
    SUCCEED("Coroutine IO read the correct number of bytes.");
  } else {
    std::stringstream s;
    s << "GPLv3 should be " << gpl3_size_lf << " (LF) or " << gpl3_size_crlf
      << " (CRLF) bytes; read " << size << ".";
    FAIL(s.str());
  }
}
