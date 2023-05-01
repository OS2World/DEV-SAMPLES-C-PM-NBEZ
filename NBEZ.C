/*********************************************************************
 *                                                                   *
 * MODULE NAME :  nbez.c                 AUTHOR:  Rick Fishman       *
 * DATE WRITTEN:  12-05-92                                           *
 *                                                                   *
 * HOW TO RUN THIS PROGRAM:                                          *
 *                                                                   *
 *  Just enter NBEZ on the command line.                             *
 *                                                                   *
 * MODULE DESCRIPTION:                                               *
 *                                                                   *
 *  Only module for NBEZ.EXE, a program that demonstrates the        *
 *  simplest way to create a Notebook control with only MAJOR tabs.  *
 *  It loads the dialogs that will be associated with the Notebook   *
 *  pages when the Notebook is initialized. No error-checking is     *
 *  done to make the program more streamlined.                       *
 *                                                                   *
 *  To provide more functionality, like using pages with MINOR tabs  *
 *  and loading the dialogs on demand, get NBBASE.ZIP.               *
 *                                                                   *
 * NOTES:                                                            *
 *                                                                   *
 *  This program is strictly a sample and should be treated as such. *
 *  There is nothing real-world about it and the dialogs that it     *
 *  uses do nothing useful. I think it still demonstrates the        *
 *  notebook control as well as can be expected though.              *
 *                                                                   *
 *  I hope this code proves useful for other PM programmers. The     *
 *  more of us the better!                                           *
 *                                                                   *
 * HISTORY:                                                          *
 *  2023-05-01 - Changes to compile with gcc 9 under ArcaOs			 *
 *                                                                   *
 *  1992-12-05 - Copied source code from the NBBASE.EXE sample and   *
 *                streamlined it.                                    *
 *                                                                   *
 *                                                                   *
 *********************************************************************/

//#pragma strings(readonly)   // used for debug version of memory mgmt routines

/*********************************************************************/
/*------- Include relevant sections of the OS/2 header files --------*/
/*********************************************************************/

#define  INCL_GPILCIDS
#define  INCL_GPIPRIMITIVES
#define  INCL_WINDIALOGS
#define  INCL_WINERRORS
#define  INCL_WINFRAMEMGR
#define  INCL_WINMESSAGEMGR
#define  INCL_WINSTDBOOK
#define  INCL_WINSYS
#define  INCL_WINWINDOWMGR

/**********************************************************************/
/*----------------------------- INCLUDES -----------------------------*/
/**********************************************************************/

#include <os2.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "nbez.h"

/*********************************************************************/
/*------------------- APPLICATION DEFINITIONS -----------------------*/
/*********************************************************************/

#define FRAME_FLAGS           (FCF_TASKLIST | FCF_TITLEBAR   | FCF_SYSMENU | \
                               FCF_MINMAX   | FCF_SIZEBORDER | FCF_ICON)

#define TAB_WIDTH_MARGIN      10   // Padding for the width of a notebook tab
#define TAB_HEIGHT_MARGIN     6    // Padding for the height of a notebook tab

/**********************************************************************/
/*----------------------- FUNCTION PROTOTYPES ------------------------*/
/**********************************************************************/

       INT  main            ( VOID );
static VOID CreateNotebook  ( HWND hwndClient );
static VOID SetUpPage       ( HWND hwndNB, INT iArrayIndex );
static VOID SetTabDimensions( HWND hwndNB );

FNWP wpClient, wpPage1, wpPage2, wpPage3, wpPage4;

/**********************************************************************/
/*------------------------ GLOBAL VARIABLES --------------------------*/
/**********************************************************************/

NBPAGE nbpage[] =    // INFORMATION ABOUT NOTEBOOK PAGES (see NBEZ.H)
{
    { wpPage1, "Page 1", "Page ~1", IDD_PAGE1 },
    { wpPage2, "Page 2", "Page ~2", IDD_PAGE2 },
    { wpPage3, "Page 3", "Page ~3", IDD_PAGE3 },
    { wpPage4, "Page 4", "Page ~4", IDD_PAGE4 }
};

#define PAGE_COUNT (sizeof( nbpage ) / sizeof( NBPAGE ))

/**********************************************************************/
/*------------------------------- main -------------------------------*/
/*                                                                    */
/*  PROGRAM ENTRYPOINT                                                */
/*                                                                    */
/*  INPUT: nothing                                                    */
/*                                                                    */
/*  1.                                                                */
/*                                                                    */
/*  OUTPUT: nothing                                                   */
/*                                                                    */
/*--------------------------------------------------------------------*/
/**********************************************************************/
INT main( VOID )
{
    HAB   hab;
    HMQ   hmq;
    HWND  hwndFrame, hwndClient;
    QMSG  qmsg;
    ULONG flFrame = FRAME_FLAGS;

    hab = WinInitialize( 0 );

    hmq = WinCreateMsgQueue( hab, 0 );

    // CS_CLIPCHILDREN so the client doesn't need to paint the area covered
    // by the notebook. CS_SIZEREDRAW so the notebook gets sized correctly
    // the first time the Frame/Client get drawn.

    WinRegisterClass( hab, NOTEBOOK_WINCLASS, wpClient,
                      CS_CLIPCHILDREN | CS_SIZEREDRAW, 0 );

    hwndFrame = WinCreateStdWindow( HWND_DESKTOP, 0, &flFrame,
                                    NOTEBOOK_WINCLASS, NULL, 0, NULLHANDLE,
                                    ID_NBWINFRAME, &hwndClient );

    WinSetWindowPos( hwndFrame, NULLHANDLE, 50, 50, 500, 450,
                     SWP_SIZE | SWP_MOVE | SWP_SHOW | SWP_ACTIVATE );

    WinSetWindowText( hwndFrame, PROGRAM_TITLE );

    while( WinGetMsg( hab, &qmsg, NULLHANDLE, 0, 0 ) )
        WinDispatchMsg( hab, &qmsg );

    WinDestroyWindow( hwndFrame );

    WinDestroyMsgQueue( hmq );

    WinTerminate( hab );

    return 0;
}

/**********************************************************************/
/*----------------------------- wpClient -----------------------------*/
/*                                                                    */
/*  CLIENT WINDOW PROCEDURE                                           */
/*                                                                    */
/*  INPUT: window proc params                                         */
/*                                                                    */
/*  1.                                                                */
/*                                                                    */
/*  OUTPUT: retcode from processing message                           */
/*                                                                    */
/*--------------------------------------------------------------------*/
/**********************************************************************/
MRESULT EXPENTRY wpClient( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
    switch( msg )
    {
        case WM_CREATE:

            CreateNotebook( hwnd );

            break;


        case WM_SIZE:

            // Size the notebook with the client window

            WinSetWindowPos( WinWindowFromID( hwnd, ID_NB ), 0, 0, 0,
                             SHORT1FROMMP( mp2 ), SHORT2FROMMP( mp2 ),
                             SWP_SIZE | SWP_SHOW );

            break;


        case WM_ERASEBACKGROUND:

            // Paint the client in the default background color

            return (MRESULT) TRUE;
    }

    return WinDefWindowProc( hwnd, msg, mp1, mp2 );
}

/**********************************************************************/
/*-------------------------- CreateNotebook --------------------------*/
/*                                                                    */
/*  CREATE THE NOTEBOOK WINDOW                                        */
/*                                                                    */
/*  INPUT: client window handle                                       */
/*                                                                    */
/*  1.                                                                */
/*                                                                    */
/*  OUTPUT: nothing                                                   */
/*                                                                    */
/*--------------------------------------------------------------------*/
/**********************************************************************/
static VOID CreateNotebook( HWND hwndClient )
{
    HWND hwndNB;
    INT  i;

    // Create the notebook. Its parent and owner will be the client window.
    // Its pages will show on the bottom right of the notebook. Its major tabs
    // will be on the right and they will be rounded. The status text will be
    // centered. Its binding will be spiraled rather than solid. The tab text
    // will be left-justified.

    hwndNB = WinCreateWindow( hwndClient, WC_NOTEBOOK, NULL,
                BKS_BACKPAGESBR | BKS_MAJORTABRIGHT | BKS_ROUNDEDTABS |
                BKS_STATUSTEXTCENTER | BKS_SPIRALBIND | BKS_TABTEXTLEFT |
                WS_GROUP | WS_TABSTOP | WS_VISIBLE,
                0, 0, 0, 0, hwndClient, HWND_TOP, ID_NB, NULL, NULL );

    SetTabDimensions( hwndNB );

    // Insert all the pages into the notebook and load and associate the dialog
    // boxes with them.

    for( i = 0; i < PAGE_COUNT; i++ )
        SetUpPage( hwndNB, i );
}

/**********************************************************************/
/*----------------------------- SetUpPage ----------------------------*/
/*                                                                    */
/*  SET UP A NOTEBOOK PAGE.                                           */
/*                                                                    */
/*  INPUT: window handle of notebook control,                         */
/*         index into nbpage array                                    */
/*                                                                    */
/*  1.                                                                */
/*                                                                    */
/*  OUTPUT: nothing                                                   */
/*                                                                    */
/*--------------------------------------------------------------------*/
/**********************************************************************/
static VOID SetUpPage( HWND hwndNB, INT iPage )
{
    ULONG ulPageId;
    HWND  hwndDlg, hwndClient = WinQueryWindow( hwndNB, QW_PARENT );

    // Insert a page into the notebook and store it in the array of page data.
    // Specify that it is to have status text and the window associated with
    // each page will be automatically sized by the notebook according to the
    // size of the page.

    ulPageId = (ULONG) WinSendMsg( hwndNB, BKM_INSERTPAGE, NULL,
                            MPFROM2SHORT( BKA_MAJOR | BKA_STATUSTEXTON |
                                          BKA_AUTOPAGESIZE, BKA_LAST ) );

    // Set the text into the status line.

    WinSendMsg( hwndNB, BKM_SETSTATUSLINETEXT, MPFROMP( ulPageId ),
                MPFROMP( nbpage[ iPage ].szStatusLineText ) );

    // Set the text into the tab for this page.

    WinSendMsg( hwndNB, BKM_SETTABTEXT, MPFROMP( ulPageId ),
                MPFROMP( nbpage[ iPage ].szTabText ) );

    // Load the dialog

    hwndDlg = WinLoadDlg( hwndClient, hwndClient, nbpage[ iPage ].pfnwpDlg, 0,
                          nbpage[ iPage ].idDlg, NULL );

    // Associate the dialog with the notebook page

    WinSendMsg( hwndNB, BKM_SETPAGEWINDOWHWND, MPFROMP( ulPageId ),
                MPFROMLONG( hwndDlg ) );
}

/**********************************************************************/
/*-------------------------- SetTabDimensions ------------------------*/
/*                                                                    */
/*  SET THE DIMENSIONS OF THE NOTEBOOK TABS.                          */
/*                                                                    */
/*  INPUT: window handle of notebook control                          */
/*                                                                    */
/*  1.                                                                */
/*                                                                    */
/*  OUTPUT: nothing                                                   */
/*                                                                    */
/*--------------------------------------------------------------------*/
/**********************************************************************/
static VOID SetTabDimensions( HWND hwndNB )
{
    HPS          hps = WinGetPS( hwndNB );
    FONTMETRICS  fm;
    INT          i, iLongestText = 0;
    POINTL       aptl[ TXTBOX_COUNT ];

    (void) memset( &fm, 0, sizeof( FONTMETRICS ) );

    // Calculate the height of a tab as the height of an average font character
    // plus a margin value.

    GpiQueryFontMetrics( hps, sizeof( FONTMETRICS ), &fm );

    fm.lMaxBaselineExt += TAB_HEIGHT_MARGIN;

    // Calculate the longest tab text

    for( i = 0; i < PAGE_COUNT; i++ )
    {
        GpiQueryTextBox( hps, strlen( nbpage[ i ].szTabText ),
                         nbpage[ i ].szTabText, TXTBOX_COUNT, aptl );

        if( aptl[ TXTBOX_CONCAT ].x > iLongestText )
            iLongestText = aptl[ TXTBOX_CONCAT ].x;
    }

    WinReleasePS( hps );

    // Add a margin amount to the longest tab text

    iLongestText += TAB_WIDTH_MARGIN;

    // Set the tab dimensions for the pages. Note that the docs as of this
    // writing say to use BKA_MAJOR and BKA_MINOR in mp2 but you really need
    // BKA_MAJORTAB and BKA_MINORTAB.

    WinSendMsg( hwndNB, BKM_SETDIMENSIONS,
                MPFROM2SHORT( iLongestText, (SHORT)fm.lMaxBaselineExt ),
                MPFROMSHORT( BKA_MAJORTAB ) );
}

/**********************************************************************/
/*----------------------------- wpPage1 ------------------------------*/
/*                                                                    */
/*  WINDOW PROCEDURE FOR PAGE 1 DIALOG                                */
/*                                                                    */
/*  INPUT: window handle, message id, message parameter 1 and 2.      */
/*                                                                    */
/*  1.                                                                */
/*                                                                    */
/*  OUTPUT: return code                                               */
/*--------------------------------------------------------------------*/
/**********************************************************************/
MRESULT EXPENTRY wpPage1( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
    return WinDefDlgProc( hwnd, msg, mp1, mp2 );
}

/**********************************************************************/
/*----------------------------- wpPage2 ------------------------------*/
/*                                                                    */
/*  WINDOW PROCEDURE FOR PAGE 2 DIALOG                                */
/*                                                                    */
/*  INPUT: window handle, message id, message parameter 1 and 2.      */
/*                                                                    */
/*  1.                                                                */
/*                                                                    */
/*  OUTPUT: return code                                               */
/*--------------------------------------------------------------------*/
/**********************************************************************/
MRESULT EXPENTRY wpPage2( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
    return WinDefDlgProc( hwnd, msg, mp1, mp2 );
}

/**********************************************************************/
/*----------------------------- wpPage3 ------------------------------*/
/*                                                                    */
/*  WINDOW PROCEDURE FOR PAGE 3 DIALOG                                */
/*                                                                    */
/*  INPUT: window handle, message id, message parameter 1 and 2.      */
/*                                                                    */
/*  1.                                                                */
/*                                                                    */
/*  OUTPUT: return code                                               */
/*--------------------------------------------------------------------*/
/**********************************************************************/
MRESULT EXPENTRY wpPage3( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
    return WinDefDlgProc( hwnd, msg, mp1, mp2 );
}

/**********************************************************************/
/*----------------------------- wpPage4 ------------------------------*/
/*                                                                    */
/*  WINDOW PROCEDURE FOR PAGE 4 DIALOG                                */
/*                                                                    */
/*  INPUT: window handle, message id, message parameter 1 and 2.      */
/*                                                                    */
/*  1.                                                                */
/*                                                                    */
/*  OUTPUT: return code                                               */
/*--------------------------------------------------------------------*/
/**********************************************************************/
MRESULT EXPENTRY wpPage4( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
    return WinDefDlgProc( hwnd, msg, mp1, mp2 );
}

/************************************************************************
 *                      E N D   O F   S O U R C E                       *
 ************************************************************************/
