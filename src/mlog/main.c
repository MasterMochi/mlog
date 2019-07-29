/******************************************************************************/
/*                                                                            */
/* src/mlog/main.c                                                            */
/*                                                                 2019/07/28 */
/* Copyright (C) 2019 Mochi.                                                  */
/*                                                                            */
/******************************************************************************/
/******************************************************************************/
/* インクルード                                                               */
/******************************************************************************/
/* 標準ヘッダ */
#include <stdbool.h>
#include <stdlib.h>

/* ライブラリヘッダ */
#include <libmk.h>

/* 共通ヘッダ */
#include <mlog.h>

/* モジュールヘッダ */
#include "Put.h"
#include "Vram.h"


/******************************************************************************/
/* 定義                                                                       */
/******************************************************************************/
/** 機能関数型 */
typedef void ( *Func_t )( MkTaskId_t taskId,
                          void       *pArg   );


/******************************************************************************/
/* ローカル関数宣言                                                           */
/******************************************************************************/
/* メインループ */
static void Loop( void );


/******************************************************************************/
/* グローバル変数定義                                                         */
/******************************************************************************/
/** 機能関数テーブル */
static Func_t gFuncTbl[ MLOG_FUNCID_NUM ] =
    { PutDo }; /* MLOG_FUNCID_PUT */


/******************************************************************************/
/* グローバル関数定義                                                         */
/******************************************************************************/
/******************************************************************************/
/**
 * @brief       mlogメイン関数
 * @details     タスク名を登録しメインループ処理を起動する。
 */
/******************************************************************************/
void main( void )
{
    MkRet_t ret;    /* カーネル戻り値 */
    MkErr_t err;    /* エラー内容     */

    /* 初期化 */
    err = MK_ERR_NONE;

    /* VRAMモジュール初期化 */
    VramInit();

    /* タスク名登録 */
    ret = LibMkTaskNameRegister( "LOG", &err );

    /* 登録結果判定 */
    if ( ret != MK_RET_SUCCESS ) {
        /* 失敗 */

        /* TODO: アボート */
        VramWriteStr( VRAM_DEFAULT_ATTR, "LibMkTaskNameRegister() error." );
    }

    /* メインループ */
    Loop();

    return;
}


/******************************************************************************/
/* ローカル関数定義                                                           */
/******************************************************************************/
/******************************************************************************/
/**
 * @brief   メインループ
 * @details メッセージの受信と機能呼び出しを行う。
 */
/******************************************************************************/
static void Loop( void )
{
    size_t       size;      /* メッセージサイズ   */
    MkRet_t      ret;       /* 戻り値             */
    MkErr_t      err;       /* エラー内容         */
    MkTaskId_t   srcTaskId; /* 送信元タスクID     */
    MlogMsgHdr_t *pMsgHdr;  /* メッセージバッファ */

    /* バッファメモリ割当て */
    pMsgHdr = ( MlogMsgHdr_t * ) malloc( MK_MSG_SIZE_MAX );

    /* 割当て結果判定 */
    if ( pMsgHdr == NULL ) {
        /* 失敗 */

        /* TODO: アボート */
        VramWriteStr( VRAM_DEFAULT_ATTR, "malloc() error." );
    }

    /* バージョン表示 */
    VramWriteStr( VRAM_DEFAULT_ATTR, "                __            \n" );
    VramWriteStr( VRAM_DEFAULT_ATTR, "               / /            \n" );
    VramWriteStr( VRAM_DEFAULT_ATTR, "    ____ ___  / /___  ____    \n" );
    VramWriteStr( VRAM_DEFAULT_ATTR, "   / __ `__ \\/ / __ \\/ __ \\   \n" );
    VramWriteStr( VRAM_DEFAULT_ATTR, "  / / / / / / / /_/ / /_/ /   \n" );
    VramWriteStr( VRAM_DEFAULT_ATTR, " /_/ /_/ /_/_/\\____/\\__, /v0.1\n" );
    VramWriteStr( VRAM_DEFAULT_ATTR, "===================/____/=====\n" );

    /* メインループ */
    while ( true ) {
        /* 初期化 */
        err       = MK_ERR_NONE;
        srcTaskId = MK_TASKID_NULL;

        /* メッセージ受信 */
        ret = LibMkMsgReceive( MK_TASKID_NULL,        /* 受信タスクID   */
                               pMsgHdr,               /* バッファ       */
                               MK_MSG_SIZE_MAX,       /* バッファサイズ */
                               &srcTaskId,            /* 送信元タスクID */
                               &size,                 /* 受信サイズ     */
                               &err             );    /* エラー番号     */

        /* 受信結果判定 */
        if ( ret != MK_RET_SUCCESS ) {
            /* 失敗 */

            continue;
        }

        /* 機能ID範囲チェック */
        if ( pMsgHdr->funcId > MLOG_FUNCID_MAX ) {
            /* 範囲外 */

            continue;
        }

        /* 機能ID呼び出し */
        ( gFuncTbl[ pMsgHdr->funcId ] )( srcTaskId, pMsgHdr );
    }
}


/******************************************************************************/
