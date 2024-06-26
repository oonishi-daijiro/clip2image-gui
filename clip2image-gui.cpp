#include <windows.h>

#include <commdlg.h>
#include <windef.h>

#include <opencv4/opencv2/opencv.hpp>

#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

#pragma comment(lib, "comdlg32.lib")

bool setBitmapFromClipboard(HANDLE &data) {
  if (!IsClipboardFormatAvailable(CF_BITMAP))
    return true;
  OpenClipboard(NULL);
  GlobalLock(data);
  data = GetClipboardData(CF_BITMAP);
  CloseClipboard();
  return false;
}

cv::Mat HBITMAP2MAT(HBITMAP hBitmap) {
  BITMAP bmp;
  GetObject(hBitmap, sizeof(BITMAP), &bmp);

  HDC hDC = GetDC(NULL);
  HDC hMemDC = CreateCompatibleDC(hDC);
  SelectObject(hMemDC, hBitmap);

  int dataSize = bmp.bmWidth * bmp.bmHeight * (bmp.bmBitsPixel / 8);
  auto data = std::make_unique<BYTE[]>(dataSize);

  GetBitmapBits(hBitmap, dataSize, data.get());

  cv::Mat mat(bmp.bmHeight, bmp.bmWidth, CV_8UC4, data.get());

  cv::Mat matCopy;
  mat.copyTo(matCopy);

  DeleteDC(hMemDC);
  ReleaseDC(NULL, hDC);

  return matCopy;
}

void writeBitmapToFile(std::wstring path, HBITMAP bitmapImage) {
  auto bmpMat = HBITMAP2MAT(bitmapImage);
  cv::imwrite(std::filesystem::path(path).string(), bmpMat);
  return;
}

std::wstring getImageFilePath(HWND hwnd) {
  WCHAR filename[10000];

  OPENFILENAMEW ofn;
  ofn.lStructSize = sizeof(OPENFILENAME);
  ZeroMemory(&ofn, sizeof(ofn));

  ofn.hInstance = GetModuleHandle(NULL);

  ofn.lStructSize = sizeof(ofn);
  ofn.lpstrFile = filename;
  ofn.hwndOwner = hwnd;
  ofn.nMaxFile = sizeof(filename);
  ofn.lpstrFilter = L"Image file\0*.png;*.bmp;*.jpg;*.gif;\0\0";
  ofn.nFilterIndex = 1;
  ofn.lpstrFileTitle = NULL;
  ofn.nMaxFileTitle = 0;
  ofn.lpstrInitialDir = NULL;
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

  GetSaveFileNameW(&ofn);
  return {filename};
}

HWND setupWindow() {
  HINSTANCE hInstance = GetModuleHandle(NULL);
  HWND own = CreateWindow(TEXT("STATIC"), TEXT("scrnshot2clipbrd"), SW_HIDE, 0,
                          0, 0, 0, NULL, NULL, hInstance, NULL);
  return own;
}

int WINAPI WinMain(HINSTANCE hInstanc, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
                   int nCmdShow) {
  SetProcessDPIAware();
  HWND hwnd = setupWindow();
  HANDLE screenshopBmp;

  if (setBitmapFromClipboard(screenshopBmp)) {
    MessageBoxA(hwnd, "There is no image data on clipboard", "Clip2image",
                MB_OK);
    return 0;
  }

  std::wstring filepath = getImageFilePath(hwnd);

  if (filepath == L"") {
    return 0;
  }

  std::filesystem::path fp = filepath;
  if (fp.extension().string() == "") {
    filepath += L".png";
  }
  writeBitmapToFile(filepath, (HBITMAP)screenshopBmp);
  CloseHandle(hwnd);
  return 0;
}
