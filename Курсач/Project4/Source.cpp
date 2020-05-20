#include <windows.h>   
#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
#include <string>
#include "resource.h";



HINSTANCE hInst;
ATOM MyRegisterClass( HINSTANCE hInstance );
BOOL InitInstance( HINSTANCE, int );
LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK DlgProc( HWND, UINT, WPARAM, LPARAM );
LPCTSTR szWindowClass = "Vasyck Dmytro";
LPCTSTR szTitle = "Лабиринт";


HBRUSH MyBrush;
HWND edit;
std::ifstream lvl( "levels.txt" );
static int x, y, startX, startY, color1_b = 0, color2_b = 0, color3_b = 0, color1 = 255, color2 = 0, color3 = 0, sek = 0, min = 0;
bool empty_lvl = false;
char **temp;



void write_time()
{    
    std::ofstream time_write( "time.txt", std::ios::app );
    time_write << min << " min " << ": " << sek << " sek " << std::endl;
    time_write.close();
}

void create_lvl( NOTIFYICONDATA nid )
{

    lvl >> empty_lvl;
    if ( empty_lvl )
    {
        write_time();
        Shell_NotifyIcon( NIM_DELETE, &nid );
        PostQuitMessage( 0 );
    }
    lvl >> x >> y;
    temp = new char *[x]; // массив указателей
    for ( int i = 0; i < x; i++ )
    {
        temp[i] = new char[y]; // инициализация указателей
        for ( int j = 0; j < y; j++ )
        {
            lvl >> temp[i][j];
            if ( temp[i][j] == 's' )
            {
                startY = i;
                startX = j;
            }
        }
    }
}

void color_shapes( HDC hdc )
{
    MyBrush = CreateSolidBrush( RGB( color1, color2, color3 ) );
    SelectObject( hdc, MyBrush );
}

void color_block( HDC hdc )
{
    MyBrush = CreateSolidBrush( RGB( color1_b, color2_b, color3_b ) );
    SelectObject( hdc, MyBrush );
}




int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{

    MSG msg;
    MyRegisterClass( hInstance );
    if ( !InitInstance( hInstance, nCmdShow ) )
    {
        return FALSE;
    }
    while ( GetMessage( &msg, NULL, 0, 0 ) )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }
    return msg.wParam;
}
ATOM MyRegisterClass( HINSTANCE hInstance )
{
    NOTIFYICONDATA nid;
    memset( &nid, 0, sizeof( NOTIFYICONDATA ) );
    nid.cbSize = sizeof( NOTIFYICONDATA );
    nid.uID = 100;
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    nid.uCallbackMessage = WM_USER + 200;
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof( WNDCLASSEX );
    wcex.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;	// стиль окна
    wcex.lpfnWndProc = (WNDPROC)WndProc; // оконная процедура
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;		// указатель приложения
    wcex.hIcon = LoadIcon( NULL, IDI_SHIELD );		// опре-деление иконки
    wcex.hCursor = LoadCursor( NULL, IDC_ARROW );    // опре-деление курсора
    wcex.hbrBackground = CreateSolidBrush( RGB( 255, 255, 255 ) );
    wcex.lpszMenuName = MAKEINTRESOURCE( IDR_MENU1 );		// определение меню
    wcex.lpszClassName = szWindowClass;	// имя класса
    wcex.hIconSm = NULL;
    create_lvl( nid );
    return RegisterClassEx( &wcex ); // регистрация класса окна
}
BOOL InitInstance( HINSTANCE hInstance, int nCmdShow )
{
    HWND hWnd;
    hInst = hInstance;
    hWnd = CreateWindow( szWindowClass,
                         szTitle,
                         WS_OVERLAPPEDWINDOW | WS_MINIMIZE | WS_SYSMENU,
                         CW_USEDEFAULT,
                         CW_USEDEFAULT,
                         CW_USEDEFAULT,
                         CW_USEDEFAULT,
                         NULL,
                         NULL,
                         hInstance,
                         NULL
    );

    if ( !hWnd )
    {
        return FALSE;
    }
    ShowWindow( hWnd, nCmdShow );		// Показать окно
    UpdateWindow( hWnd );			// Обновить окно
    return TRUE;				//Успешное завершение функции
}


LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    RECT rc;
    HDC hdc, hdc2;
    RECT rt;
    char timer_c[64];
    NOTIFYICONDATA nid;
    memset( &nid, 0, sizeof( NOTIFYICONDATA ) );
    nid.cbSize = sizeof( NOTIFYICONDATA );
    nid.hWnd = hWnd;
    nid.uID = 100;
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    nid.uCallbackMessage = WM_USER + 200;
    static int  posX = startX, posY = startY, sizeX, sizeY;
 
    rc.left = posX * ( sizeX / y ) + 9;
    rc.top = posY * ( sizeY / x ) + 5;
    rc.right = posX * ( sizeX / y ) + ( sizeX / y ) - 9;
    rc.bottom = posY * ( sizeY / x ) - 15 + ( sizeY / x );

    switch ( message )
    {

        case WM_CREATE:
        {
            nid.hIcon = LoadIcon( NULL, IDI_SHIELD );
            Shell_NotifyIcon( NIM_ADD, &nid );
            SetTimer( hWnd, 1, 1000, NULL );
        }
        break;

        case WM_TIMER:
        {
            sek += 1;
            if ( sek == 60 )
            {
                sek = 0;
                min += 1;
            }
            InvalidateRect( hWnd, NULL, FALSE );
        }
        break;


        case WM_PAINT:
        {
            hdc = BeginPaint( hWnd, &ps );
            ValidateRect( hWnd, NULL );
            if ( sek < 10 )
            {
                wsprintf( timer_c, "time: 0%d : 0%d", min, sek );
            }
            else
            {
                wsprintf( timer_c, "time: 0%d : %d", min, sek );
            }
            SetWindowText( hWnd, timer_c );

            if ( temp[posY][posX] == 'f' )
            {
                delete[] temp;
                create_lvl( nid );
                posX = startX;
                posY = startY;
                InvalidateRect(hWnd, NULL, TRUE);
            }

            GetClientRect( hWnd, &rt );
            sizeX = rt.right - rt.left;
            sizeY = rt.bottom - rt.top + 20;

            color_block( hdc );
            for ( int i = 0; i < x; i++ )
            {
                for ( int j = 0; j < y; j++ )
                {
                    if ( temp[i][j] == '#' )
                    {
                        Rectangle( hdc, j*( sizeX / y ), i*( sizeY / x ), ( j + 1 )*( sizeX / y ), ( i + 1 )*( sizeY / x ) - 10 );
                    }
                }
            }

            color_shapes( hdc );
            Rectangle( hdc, posX*( sizeX / y ) + 9, posY*( sizeY / x ) + 5, posX*( sizeX / y ) + ( sizeX / y ) - 9, posY*( sizeY / x ) - 15 + ( sizeY / x ) );
            EndPaint( hWnd, &ps );
        }
        break;


        case WM_KEYDOWN:
        {

            switch ( wParam )
            {

                case 0x57:
                {  //Ww
                    if ( temp[posY - 1][posX] != '#' )
                    {   
                        posY -= 1;
                        rc.top = posY * ( sizeY / x ) + 5;
                        InvalidateRect( hWnd, &rc, TRUE );
                    }
                
                }
                break;

                case 0x53:
                { //Ss   
                    if ( temp[posY + 1][posX] != '#' )
                    {   
                        posY += 1;
                        rc.bottom = posY * ( sizeY / x ) - 15 + ( sizeY / x );
                        InvalidateRect( hWnd, &rc, TRUE );
                    }
                }
                break;

                case 0x41:
                {  //Aa
                    if ( temp[posY][posX - 1] != '#' )
                    {
                        posX -= 1;
                        rc.left = posX * ( sizeX / y ) + 9;
                        InvalidateRect( hWnd, &rc, TRUE );
                    }
                }
                break;

                case 0x44:
                { //Dd
                    if ( temp[posY][posX + 1] != '#' )
                    {
                        posX += 1;
                        rc.right = posX * ( sizeX / y ) + ( sizeX / y ) - 9;
                        InvalidateRect( hWnd, &rc, TRUE ); 
                    }
                }
                break;
            }
            break;
        }
        break;


        case WM_COMMAND:
        {
            switch ( LOWORD( wParam ) )
            {

                case EXIT:
                {
                    PostQuitMessage( 0 );
                    Shell_NotifyIcon( NIM_DELETE, &nid );
                }
                break;

                case ABOUT_US:
                {
                    MessageBox( hWnd, "Данная программа была написана за пару дней", "about us", MB_OK );
                }
                break;

                case RED:
                {
                    color1 = 255;
                    color2 = 0;
                    color3 = 0;
                    InvalidateRect( hWnd, NULL, TRUE );
                }
                break;

                case BLUE:
                {
                    color1 = 0;
                    color2 = 191;
                    color3 = 255;
                    InvalidateRect( hWnd, NULL, TRUE );
                }
                break;

                case YELLOW:
                {
                    color1 = 255;
                    color2 = 255;
                    color3 = 0;
                    InvalidateRect( hWnd, NULL, TRUE );
                }
                break;

                case GREEN:
                {
                    color1 = 0;
                    color2 = 255;
                    color3 = 0;
                    InvalidateRect( hWnd, NULL, TRUE );
                }
                break;

                case BLACK:
                {
                    color1 = 0;
                    color2 = 0;
                    color3 = 0;
                    InvalidateRect( hWnd, NULL, TRUE );
                }
                break;

                case WHITE:
                {
                    color1 = 255;
                    color2 = 255;
                    color3 = 255;
                    InvalidateRect( hWnd, NULL, TRUE );
                }
                break;

                case RED_B:
                {
                    color1_b = 255;
                    color2_b = 0;
                    color3_b = 0;
                    InvalidateRect( hWnd, NULL, TRUE );
                }
                break;

                case BLUE_B:
                {
                    color1_b = 0;
                    color2_b = 191;
                    color3_b = 255;
                    InvalidateRect( hWnd, NULL, TRUE );
                }
                break;

                case YELLOW_B:
                {
                    color1_b = 255;
                    color2_b = 255;
                    color3_b = 0;
                    InvalidateRect( hWnd, NULL, TRUE );
                }
                break;

                case GREEN_B:
                {
                    color1_b = 0;
                    color2_b = 255;
                    color3_b = 0;
                    InvalidateRect( hWnd, NULL, TRUE );
                }
                break;

                case BLACK_B:
                {
                    color1_b = 0;
                    color2_b = 0;
                    color3_b = 0;
                    InvalidateRect( hWnd, NULL, TRUE );
                }
                break;

                case WHITE_B:
                {
                    color1_b = 255;
                    color2_b = 255;
                    color3_b = 255;
                    InvalidateRect( hWnd, NULL, TRUE );
                }
                break;

                case RESULTS:
                {
                    DialogBox( hInst, MAKEINTRESOURCE( DLG ), 0, DlgProc );
                }
                break;

            }
        }
        break;


        case WM_DESTROY:
        {

            PostQuitMessage( 0 );
            Shell_NotifyIcon( NIM_DELETE, &nid );
            KillTimer( hWnd, 1 );
        }
        break;


        default:
        return DefWindowProc( hWnd, message, wParam, lParam );
    }
    return 0;
}

// for time
LRESULT CALLBACK DlgProc( HWND hWndDlg, UINT uMsgDlg, WPARAM wParams, LPARAM lParam )
{

    switch ( uMsgDlg )
    {

        case WM_INITDIALOG:{
            edit = GetDlgItem( hWndDlg, EDIT1 );
            std::ifstream read( "time.txt" );
            std::ostringstream stream;
            std::string temp;
            while ( std::getline( read, temp ) ){
               stream << temp << "\r\n";
            }
            SetWindowText( edit, stream.str().c_str() );
            SetFocus( edit );
        }
        break;

        case WM_CLOSE:{
            EndDialog( hWndDlg, 0 );
        }
        break;
    }
    return 0;
}

