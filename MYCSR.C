// mycsr.c
#define STRICT
#include <windows.h>
#include "mycsr.h"

// Work
HINSTANCE ghInst;
BYTE szClassName[] = "MyCsrWClass";
BOOL fArrow;
BOOL fIBeam;
HGLOBAL ghgArrowMem;
HGLOBAL ghgIBeamMem;
int nArrowType;
int nIBeamType;
int nNoIcon;
int nLockRes;

// ini
BYTE szIniFile[] = "MYCSR.INI";
BYTE szSctEnv[] = "Env";
BYTE szEntNoIcon[] = "NoDesktopIcon";
BYTE szEntLockRes[] = "LockCursorResource";
BYTE szSctType[] = "Type";
BYTE szEntArrow[] = "ARROW";
BYTE szEntIBeam[] = "I-BEAM";

// data
#define ARROW_MAX	9
LPBYTE lpszArrowNames[ARROW_MAX] = {
	"<DEFAULT>",
	"Large",
	"Middle",
	"Small",
	"Large (Lefty)",
	"Middle (Lefty)",
	"Small (Lefty)",
	"3D Type-1",
	"3D Type-2"
};

// data
#define IBEAM_MAX	8
LPBYTE lpszIBeamNames[IBEAM_MAX] = {
	"<DEFAULT>",
	"Large",
	"Middle",
	"Small",
	"Type-1",
	"Type-2",
	"Type-3",
	"Type-4"
};

//
//	Windows Main
//
int PASCAL WinMain(
	HINSTANCE hInstance, HINSTANCE hInstPrev, LPSTR lpszCmdLine, int nCmdShow )
{
	HWND hWnd;
	MSG msg;
	WNDCLASS wc;

	if( hInstPrev != NULL ){
		hWnd = FindWindow( szClassName, NULL );
		if( hWnd != NULL ){
			ShowWindow( hWnd, SW_SHOWMINIMIZED );
			BringWindowToTop( hWnd );
		}
		return( FALSE );
	}

	if( hInstPrev == NULL ){
		wc.style			= NULL;
		wc.lpfnWndProc		= MainWndProc;
		wc.cbClsExtra		= 0;
		wc.cbWndExtra		= 0;
		wc.hInstance		= hInstance;
		wc.hIcon			= LoadIcon( hInstance, "MYCSR" );
		wc.hCursor			= LoadCursor( NULL, IDC_ARROW );
		wc.hbrBackground	= GetStockObject( WHITE_BRUSH );
		wc.lpszMenuName		= NULL;
		wc.lpszClassName	= szClassName;
		RegisterClass( &wc );
	}

	ghInst = hInstance;

	hWnd = CreateWindow( szClassName, "MyCsr",
		WS_OVERLAPPEDWINDOW | WS_ICONIC,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL );

	if( nNoIcon == 0 ){
		ShowWindow( hWnd, SW_SHOWMINIMIZED );
		UpdateWindow( hWnd );
		BringWindowToTop( hWnd );
	} else {
		ShowWindow( hWnd, SW_HIDE );
	}

	while( GetMessage( &msg, NULL, 0, 0 )){
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	return( msg.wParam );
}

//
//	Main Window Proc
//
LRESULT CALLBACK MainWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	BYTE szBuf[100];
	FARPROC lpProc;
	HMENU hMenu;
	BOOL fRet;

	switch( message ){
	case WM_CREATE:
		hMenu = GetSystemMenu( hWnd, FALSE );
		ModifyMenu( hMenu, SC_RESTORE,
			MF_BYCOMMAND | MF_STRING | MF_ENABLED, SC_RESTORE, "&Setup..." );
		DeleteMenu( hMenu, SC_SIZE, MF_BYCOMMAND );
		DeleteMenu( hMenu, SC_MINIMIZE, MF_BYCOMMAND );
		DeleteMenu( hMenu, SC_MAXIMIZE, MF_BYCOMMAND );
		DrawMenuBar( hWnd );
		LoadIniFile();
		SaveSystemCursor();
		if( fArrow == FALSE || fIBeam == FALSE ){
			wsprintf( szBuf, "System Cursor Save Error (%d:%d)", fArrow, fIBeam );
			MessageBox( hWnd, szBuf, "MyCsr", MB_OK | MB_ICONEXCLAMATION );
		}
		CursorChangeArrow( nArrowType );
		CursorChangeIBeam( nIBeamType );
		return( 0L );

	case WM_QUERYOPEN:
		lpProc = MakeProcInstance( (FARPROC)SetupDlgProc, ghInst );
		fRet = DialogBox( ghInst, (LPSTR)"SetupBox", hWnd, (DLGPROC)lpProc );
		FreeProcInstance( lpProc );
		if( fRet == TRUE ){
			SaveIniFile();
			CursorChangeArrow( nArrowType );
			CursorChangeIBeam( nIBeamType );
		}
		if( nNoIcon != 0 ){
			ShowWindow( hWnd, SW_HIDE );
		}
		return( 0L );

	case WM_CLOSE:
		SaveIniFile();
		LoadSystemCursor();
		DestroyWindow( hWnd );
		return( 0L );
	
	case WM_DESTROY:
		PostQuitMessage( 0 );
		return( 0L );

	}
	return( DefWindowProc( hWnd, message, wParam, lParam ));
}

//
//	Setup Dialog Proc
//
BOOL CALLBACK SetupDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	int i, j;
	RECT rect;

	switch( message ){
	case WM_INITDIALOG:
		for( i = 0; i < ARROW_MAX; i++ ){
			SendDlgItemMessage(
				hDlg, IDD_ARROWLIST, LB_ADDSTRING, 0, (LPARAM)lpszArrowNames[i] );
		}
		SendDlgItemMessage( hDlg, IDD_ARROWLIST, LB_SETCURSEL, nArrowType, 0L );
		for( i = 0; i < IBEAM_MAX; i++ ){
			SendDlgItemMessage(
				hDlg, IDD_IBEAMLIST, LB_ADDSTRING, 0, (LPARAM)lpszIBeamNames[i] );
		}
		SendDlgItemMessage( hDlg, IDD_IBEAMLIST, LB_SETCURSEL, nIBeamType, 0L );
		CheckDlgButton( hDlg, IDD_NOICON, nNoIcon );
		CheckDlgButton( hDlg, IDD_LOCKRES, nLockRes );
		SetDlgItemText( hDlg, IDD_IBEAMAREA, "I-BEAM Cursor Test Area" );
	//	SendDlgItemMessage( hDlg, IDD_IBEAMAREA, WM_SETFONT, NULL, (LONG)TRUE );
		GetWindowRect( hDlg, &rect );
		i = ( GetSystemMetrics( SM_CXSCREEN ) - ( rect.right - rect.left )) / 2;
		j = ( GetSystemMetrics( SM_CYSCREEN ) - ( rect.bottom - rect.top )) / 2;
		SetWindowPos( hDlg, NULL, i, j, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE );
		return( TRUE );

	case WM_COMMAND:
		switch( wParam ){
		case IDD_ARROWLIST:
			switch( HIWORD( lParam )){
			case LBN_SELCHANGE:
				CursorChangeArrow(
					(int)SendDlgItemMessage( hDlg, IDD_ARROWLIST, LB_GETCURSEL, 0, 0L ));
			}
			return( TRUE );
		case IDD_IBEAMLIST:
			switch( HIWORD( lParam )){
			case LBN_SELCHANGE:
				CursorChangeIBeam(
					(int)SendDlgItemMessage( hDlg, IDD_IBEAMLIST, LB_GETCURSEL, 0, 0L )); 
			}
			return( TRUE );
		case IDOK:
			nArrowType = (int)SendDlgItemMessage( hDlg, IDD_ARROWLIST, LB_GETCURSEL, 0, 0L );
			nIBeamType = (int)SendDlgItemMessage( hDlg, IDD_IBEAMLIST, LB_GETCURSEL, 0, 0L );
			nNoIcon = IsDlgButtonChecked( hDlg, IDD_NOICON );
			nLockRes = IsDlgButtonChecked( hDlg, IDD_LOCKRES );
			EndDialog( hDlg, TRUE );
			return( TRUE );
		case IDCANCEL:
			CursorChangeArrow( nArrowType );
			CursorChangeIBeam( nIBeamType );
			EndDialog( hDlg, FALSE );
			return( TRUE );
		}
	}
	return( FALSE );
}

//
//	Load Ini File
//
BOOL LoadIniFile( VOID )
{
	nNoIcon = GetPrivateProfileInt( szSctEnv, szEntNoIcon, 0, szIniFile );
	nLockRes = GetPrivateProfileInt( szSctEnv, szEntLockRes, 0, szIniFile );

	nArrowType = GetPrivateProfileInt( szSctType, szEntArrow, 0, szIniFile );
	nArrowType = min( ARROW_MAX, max( 0, nArrowType ));

	nIBeamType = GetPrivateProfileInt( szSctType, szEntIBeam, 0, szIniFile );
	nIBeamType = min( IBEAM_MAX, max( 0, nIBeamType ));

	return( TRUE );
}

//
//	Save Ini File
//
BOOL SaveIniFile( VOID )
{
	BYTE szBuf[50];

	wsprintf( szBuf, "%1d", nNoIcon );
	WritePrivateProfileString( szSctEnv, szEntNoIcon, szBuf, szIniFile );

	wsprintf( szBuf, "%1d", nLockRes );
	WritePrivateProfileString( szSctEnv, szEntLockRes, szBuf, szIniFile );

	wsprintf( szBuf, "%1d", nArrowType );
	WritePrivateProfileString( szSctType, szEntArrow, szBuf, szIniFile );

	wsprintf( szBuf, "%1d", nIBeamType );
	WritePrivateProfileString( szSctType, szEntIBeam, szBuf, szIniFile );

	return( TRUE );
}

//
//	Load System Cursor
//
BOOL LoadSystemCursor( VOID )
{
	if( fArrow == TRUE ){
		CursorChangeArrow( 0 );
	}

	if( ghgArrowMem != NULL ){
		GlobalFree( ghgArrowMem );
	}

	if( fIBeam == TRUE ){
		CursorChangeIBeam( 0 );
	}
	
	if( ghgIBeamMem != NULL ){
		GlobalFree( ghgIBeamMem );
	}

	if( nLockRes != 0 ){
		GlobalUnlock( (GLOBALHANDLE)LoadCursor( NULL, IDC_IBEAM ));
    }
	
	return( TRUE );
}

//
//	Save System Cursor
//
BOOL SaveSystemCursor( VOID )
{
	UINT uLen;
	LPBYTE lpbMem;
	HCURSOR hCsr;
	HCURSOR hSaveCsr;

	fArrow = FALSE;
	hCsr = LoadCursor( NULL, IDC_ARROW );
	hSaveCsr = SetCursor( hCsr );
	GlobalLock( hCsr );
	uLen = (UINT)GlobalSize( hCsr );
	ghgArrowMem = GlobalAlloc( GHND, (DWORD)uLen );
	if( ghgArrowMem != NULL ){
		lpbMem = GlobalLock( ghgArrowMem );
		if( CursorToMemory( IDC_ARROW, lpbMem ) == TRUE ){
			fArrow = TRUE;
		}
		GlobalUnlock( ghgArrowMem );
	}
	GlobalUnlock( hCsr );
	SetCursor( hSaveCsr );

	fIBeam = FALSE;
	hCsr = LoadCursor( NULL, IDC_IBEAM );
	GlobalLRUNewest( hCsr );
	hSaveCsr = SetCursor( hCsr );
	GlobalLock( hCsr );
	uLen = (UINT)GlobalSize( hCsr );
	ghgIBeamMem = GlobalAlloc( GHND, (DWORD)uLen );
	if( ghgIBeamMem != NULL ){
		lpbMem = GlobalLock( ghgIBeamMem );
		if( CursorToMemory( IDC_IBEAM, lpbMem ) == TRUE ){
			fIBeam = TRUE;
		}
		GlobalUnlock( ghgIBeamMem );
	}
	GlobalUnlock( hCsr );
    SetCursor( hSaveCsr );
    
	return( TRUE );
}

//
//	Cursor Change Arrow
//
BOOL CursorChangeArrow( int nNo )
{
	BOOL fRet;
	LPBYTE lpbUsrCsr;
	HCURSOR hUsrCsr;

	if( ghgArrowMem == NULL ){
		return( FALSE );
	}

	ShowCursor( FALSE );

	if( nNo == 0 ){
		lpbUsrCsr = GlobalLock( ghgArrowMem );
	} else {
		hUsrCsr = LoadCursor( ghInst, MAKEINTRESOURCE( IDD_ARROWL + nNo - 1 ));
		lpbUsrCsr = (LPBYTE)LockResource( hUsrCsr );
	}

	fRet = MemoryToCursor( lpbUsrCsr, IDC_ARROW );
	
	if( nNo == 0 ){
		GlobalUnlock( ghgArrowMem );
	} else {
		UnlockResource( hUsrCsr );
		DestroyCursor( hUsrCsr );
	}

	ShowCursor( TRUE );

	return( fRet );
}

//
//	Cursor Change I-BEAM
//
BOOL CursorChangeIBeam( int nNo )
{
	BOOL fRet;
	LPBYTE lpbUsrCsr;
	HCURSOR hUsrCsr;
	UINT uLockCount;

	if( ghgIBeamMem == NULL ){
		return( FALSE );
	}

	ShowCursor( FALSE );

	if( nNo == 0 ){
		lpbUsrCsr = GlobalLock( ghgIBeamMem );
	} else {
		hUsrCsr = LoadCursor( ghInst, MAKEINTRESOURCE( IDD_IBEAML + nNo - 1 ));
		lpbUsrCsr = (LPBYTE)LockResource( hUsrCsr );
	}

	fRet = MemoryToCursor( lpbUsrCsr, IDC_IBEAM );
	
	if( nNo == 0 ){
		GlobalUnlock( ghgIBeamMem );
	} else {
		UnlockResource( hUsrCsr );
		DestroyCursor( hUsrCsr );
	}

	if( nLockRes != 0 ){
		hUsrCsr = LoadCursor( NULL, IDC_IBEAM );
		uLockCount = GlobalFlags( (GLOBALHANDLE)hUsrCsr );
		uLockCount &= GMEM_LOCKCOUNT;
		if( uLockCount == 0 ){
			GlobalLock( (GLOBALHANDLE)hUsrCsr );
		}
	} else {
		hUsrCsr = LoadCursor( NULL, IDC_IBEAM );
		GlobalUnlock( (GLOBALHANDLE)hUsrCsr );
	}

	ShowCursor( TRUE );

	return( fRet );
}

//
//	Memory To Cursor
//
BOOL MemoryToCursor( LPBYTE lpbMem, LPCSTR lpcCsr )
{
	BOOL fRet;
	UINT uCnt;
	UINT uLen;
	LPBYTE lpbCsr;
	HCURSOR hCsr;

	if( lpbMem == NULL || lpcCsr == NULL ){
		return( FALSE );
	}

	fRet = FALSE;

	hCsr = LoadCursor( NULL, lpcCsr );
	if( hCsr != NULL ){
		lpbCsr = GlobalLock( hCsr );
		if( lpbCsr !=  NULL ){
			uLen = (UINT)GlobalSize( hCsr );
			if( uLen != 0 ){
				for( uCnt = 0; uCnt < uLen; uCnt++ ){
					*( lpbCsr + (DWORD)uCnt ) = *( lpbMem + (DWORD)uCnt );
				}
				fRet = TRUE;
			}
		}
		GlobalUnlock( hCsr );
	}

	return( fRet );
}

//
//	Cursor To Memory
//
BOOL CursorToMemory( LPCSTR lpcCsr, LPBYTE lpbMem )
{
	BOOL fRet;
	UINT uCnt;
	UINT uLen;
	LPBYTE lpbCsr;
	HCURSOR hCsr;

	if( lpbMem == NULL || lpcCsr == NULL ){
		return( FALSE );
	}

	fRet = FALSE;

	hCsr = LoadCursor( NULL, lpcCsr );
	if( hCsr != NULL ){
		lpbCsr = GlobalLock( hCsr );
		if( lpbCsr != NULL ){
			uLen = (UINT)GlobalSize( hCsr );
			if( uLen != 0 ){
				for( uCnt = 0; uCnt < uLen; uCnt++ ){
					*( lpbMem + (DWORD)uCnt ) = *( lpbCsr + (DWORD)uCnt );
				}
				fRet = TRUE;
			}
		}
		GlobalUnlock( hCsr );
	}

	return( fRet );
}

// [EOF]
