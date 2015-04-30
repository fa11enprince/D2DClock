#pragma once
#ifdef _DEBUG
#include <sstream>
#endif
#include "GlobalConst.h"

// ���̃N���X�̐�����
// http://msdn.microsoft.com/ja-jp/library/windows/desktop/ff381400%28v=vs.85%29.aspx
template <class DERIVED_TYPE> 
class BaseWindow
{
public:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        DERIVED_TYPE *pThis = NULL;

        if (uMsg == WM_NCCREATE)
        {
            // Param �p�����[�^�[���L���X�g���� CREATESTRUCT �\���̂��擾
            CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT *>(lParam);
            // CREATESTRUCT �\���̂� lpCreateParams �����o�[�́ACreateWindowEx��
            // �w�肵������ void �|�C���^�[�ł��B
            // lpCreateParams ���L���X�g���āA
            // �Ǝ��̃f�[�^�\���ւ̃|�C���^�[���擾���܂��B
            pThis = reinterpret_cast<DERIVED_TYPE *>(pCreate->lpCreateParams);
            // SetWindowLongPtr �֐����Ăяo���āA�Ǝ��̃f�[�^�\���ւ̃|�C���^�[�ɓn���܂��B
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));

            pThis->m_hwnd = hwnd;
        }
        else
        {
            // SetWindowLongPtr�Őݒ肵���|�C���^�[���擾
            pThis = reinterpret_cast<DERIVED_TYPE *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        }
        if (pThis)
        {
            return pThis->HandleMessage(uMsg, wParam, lParam);
        }
        else
        {
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }

    BaseWindow() : m_hwnd(NULL) { }

    BOOL Create(
        PCWSTR lpWindowName,
        DWORD dwStyle,
        //DWORD dwExStyle = 0,   // MOD
        DWORD dwExStyle = WS_EX_LAYERED,   // ���߃��C���[
        int x = CW_USEDEFAULT,
        int y = CW_USEDEFAULT,
        int nClientWidth = GlobalConst::CLIENT_AREA_SIZE,
        int nClientHeight = GlobalConst::CLIENT_AREA_SIZE,
        HWND hWndParent = 0,
        HMENU hMenu = 0
        )
    {
        WNDCLASS wc = { 0 };

        wc.lpfnWndProc   = DERIVED_TYPE::WindowProc;
        // �悭����`���� wc.hInstance = hInstance;�̂悤�Ȍ`����
        // WinMain��hInstance��n�����A����͂�������Ȃ��AGetModuleHandle()�𗘗p
        wc.hInstance     = GetModuleHandle(NULL);   // �E�B���h�E�ɓo�^�����n���h�����擾
        wc.lpszClassName = ClassName();
        // �J�[�\������X�^�C����
        wc.hCursor = static_cast<HCURSOR>(
            LoadImage(NULL,
            MAKEINTRESOURCE(IDC_ARROW),
            IMAGE_CURSOR,
            0,
            0,
            LR_DEFAULTSIZE | LR_SHARED)
            );
        RegisterClass(&wc);

        // �N���C�A���g�̃T�C�Y���w��T�C�Y�ɂ���
        RECT rc = { 0, 0, nClientWidth, nClientHeight };
        AdjustWindowRectEx(&rc, dwStyle, FALSE, 0);
        int nWidth = rc.right - rc.left;
        int nHeight = rc.bottom - rc.top;

        // ���̂Ƃ���CW_USEDEFAULT�������Ȃ�
        if (dwStyle == WS_POPUP && x == CW_USEDEFAULT && y == CW_USEDEFAULT)
        {
            // �^�X�N�o�[���������G���A�̋�`�擾
            RECT workerArea;
            SystemParametersInfo(SPI_GETWORKAREA, 0, &workerArea, 0);

            // �擾�����G���A�̉E��������ɕ\��
            x = workerArea.right - nClientWidth;
            y = workerArea.bottom - nClientHeight;
#ifdef _DEBUG
            std::wstringstream ss1;
            ss1 << L"taskbar" << L"\n"
                << L"top " << workerArea.top << L"\n"
                << L"left " << workerArea.left << L"\n"
                << L"bottom " << workerArea.bottom << L"\n"
                << L"right " << workerArea.right << L"\n";

            MessageBox(NULL, ss1.str().c_str(), NULL, MB_OK);
            std::wstringstream ss2;
            ss2 << L"x : " << x << L"y : " << y;
            MessageBox(NULL, ss2.str().c_str(), NULL, MB_OK);
#endif
        }

        m_hwnd = CreateWindowEx(
            dwExStyle, 
            ClassName(),
            lpWindowName,
            dwStyle,
            x, y, nWidth, nHeight,
            hWndParent,    // �e�E�B���h�E
            hMenu,
            GetModuleHandle(NULL),
            this   // �ǉ��̃A�v���P�[�V�����f�[�^
            );

        return (m_hwnd ? TRUE : FALSE);
    }

    HWND Window() const { return m_hwnd; }

protected:
    virtual PCWSTR  ClassName() const = 0;
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

    HWND m_hwnd;
};