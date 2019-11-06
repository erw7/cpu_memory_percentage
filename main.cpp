#define UNICODE

#include <windows.h>
#include <pdh.h>

#include <exception>
#include <iostream>
#include <iomanip>
#include <locale>
#include <sstream>

class PerformanceCounter {
  public:
    PerformanceCounter();
    std::wstring print_percentage();
  protected:
    int m_percentage;
    std::wstring m_label;
  private:
    std::wstring m_low_fg_color;
    std::wstring m_medium_fg_color;
    std::wstring m_high_fg_color;

    virtual int get_percentage() = 0;
    std::wstring print_graph();
    std::wstring print_fg_color();
};

PerformanceCounter::PerformanceCounter() {
  this->m_low_fg_color = L"#[fg=#8AC6F2]";
  this->m_medium_fg_color = L"#[fg=#CAE682]";
  this->m_high_fg_color = L"#[fg=#E5786C]";
}

std::wstring PerformanceCounter::print_graph() {
  // std::wstring bar[] = {L"  ", L"▁", L" ▂", L" ▃", L" ▄", L" ▅", L" ▆", L" ▇", L" █"};
  std::wstring bar[] = {L"  ", L"\u2581", L"\u2582", L"\u2583", L"\u2584", L"\u2585", L"\u2586", L"\u2587", L"\u2588"};
  int number_of_class = sizeof(bar) / sizeof(bar[0]);
  float class_width = 100 / (number_of_class - 1);
  for (int rank = 0; rank < number_of_class; rank++) {
    if (this->m_percentage <= class_width * rank) {
      return bar[rank];
    }
  }
  return L"";
}

std::wstring PerformanceCounter::print_fg_color() {
  if (this->m_percentage >= 80 ) {
    return this->m_high_fg_color;
  } else if (this->m_percentage >= 30) {
    return this->m_medium_fg_color;
  } else {
    return this->m_low_fg_color;
  }
}

std::wstring PerformanceCounter::print_percentage() {
  std::wstringstream wss;
  wss << this->m_label
    <<  this->print_fg_color()
    <<  L" " << this->print_graph()
    << std::setw(3) << this->m_percentage << L"%";
  return wss.str();
}

class CPUPerformanceCounter : public PerformanceCounter
{
  public:
    CPUPerformanceCounter();
  private:
    int get_percentage();
};

CPUPerformanceCounter::CPUPerformanceCounter() {
  this->m_label= L"CPU:";
  this->m_percentage = this->get_percentage();
}

int CPUPerformanceCounter::get_percentage() {
  PDH_HQUERY hQuery;
  PDH_HCOUNTER hCounter;
  PDH_FMT_COUNTERVALUE fntValue;

  PDH_STATUS stError = PdhOpenQuery(NULL, 0, &hQuery);
  if ( stError != ERROR_SUCCESS) {
    std::error_code ec(stError, std::generic_category());
    throw std::system_error(ec, "system error!");
  }

  PdhAddCounter(hQuery, L"\\Processor(_Total)\\% Processor Time", 0, &hCounter);
  PdhCollectQueryData(hQuery);

  Sleep(100);

  PdhCollectQueryData(hQuery);
  PdhGetFormattedCounterValue(hCounter, PDH_FMT_LARGE, NULL, &fntValue);
  PdhCloseQuery(hQuery);
  return fntValue.largeValue;
}

class MemoryPerformanceCounter : public PerformanceCounter
{
  public:
    MemoryPerformanceCounter();
  private:
    int get_percentage();
};

MemoryPerformanceCounter::MemoryPerformanceCounter() {
  this->m_label= L"Mem";
  this->m_percentage = this->get_percentage();
}

int MemoryPerformanceCounter::get_percentage() {
  MEMORYSTATUSEX statex;
  statex.dwLength = sizeof(statex);
  GlobalMemoryStatusEx(&statex);
  return statex.dwMemoryLoad;
}

int main(void)
{
  setlocale(LC_CTYPE, "");
  try {
    CPUPerformanceCounter cpu_perf;
    MemoryPerformanceCounter mem_perf;

    std::wcout << cpu_perf.print_percentage() << L"#[fg=#969696] "
      << mem_perf.print_percentage() << std::endl;
  }
  catch (std::system_error& e) {
    std::cerr << "Failed creeate CPUPerformanceCounter.";
    return 1;
  }

  return 0;
}
