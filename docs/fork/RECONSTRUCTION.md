# ソース再建の記録 / How the source was reconstructed #

元の COMP_hack は `libcomp`・`sqrat`・`datastore` を別リポジトリ（git サブモジュール）で
管理し、依存ライブラリは `github.com/comphack/*` から取得していました。
これらはすべて削除されていたため、ベースのコミット（`95b6ae26`、2022-04-08）は
そのままではビルドできませんでした。このページはその再建の方法と出典の記録です。

*English: The libcomp, sqrat and datastore submodules and the comphack/\*
dependency repositories no longer exist. This page records where the vendored
code came from and what had to be rebuilt.*

## 出典 / Sources ##

| 対象 | 出典 |
|---|---|
| サーバー本体（server/, libhack/ など） | [HyperChiicken/SMT](https://github.com/HyperChiicken/SMT) `develop` のコミット `95b6ae26` |
| `libcomp/`（2020年12月時点） | Launchpad PPA `ppa:compomega/comphack` のソースパッケージ `comphack_4.12.2.tar.xz`（公開日 2020-12-14、SHA-256 `643ea6376d33c9b3481e545071f96e8c8afcb4a6fc8c0e9e0d62ca9473d46119`） |
| `deps/sqrat/`（comp_hack 版の sqrat） | 同上 |
| `deps/*.zip`（依存ライブラリ） | 同上。12 個とも `cmake/deps/*.cmake` に書かれた SHA-1 と一致 |
| `deps/objgen/comp_objgen.exe` | COMP_hack 2022 年版の Windows 配布物に含まれていたもの |

## 2020 年版 libcomp から 2022 年版への再建 ##

2020 年 12 月から 2022 年 4 月の間に、ゲーム固有の処理が `libcomp` から
`libhack` に移され、`libcomp` は汎用ライブラリになりました。
ベースのコミットのコードはこの新しい構成を前提としているため、
2020 年版の `libcomp` を 2022 年版の構成に合わせて組み替えました。

正解として使ったのは、2022 年版の公式配布物（Jenkins の RelWithDebInfo ビルド）の
デバッグ情報（PDB）です。PDB に含まれるソースファイルの一覧、クラスのメンバー配置、
関数の宣言、列挙型の値、コンパイラの引数を参照し、必要な箇所は配布物の
逆アセンブルで処理内容を確認しました。

### 新しく書いたファイル ###

| ファイル | 内容 |
|---|---|
| `BaseLog.h/.cpp` | ログの基底クラス（旧 `Log`）。コンポーネントごとのログレベル、別スレッドでの書き出し |
| `BaseScriptEngine.h/.cpp` | スクリプトエンジンの基底クラス（旧 `ScriptEngine`）。`logic.Foo` のような名前空間付きの登録に対応 |
| `BaseConfig.h/.cpp` | ファイル暗号化の定数（旧 `Config`） |
| `BaseConstants.h` | libcomp 用の定数（旧 `Constants.h.in` の汎用部分） |
| `Message.cpp`、`ConnectionMessage.cpp` | メッセージの `Clone()`・`GetRawType()` とスクリプト用の登録 |
| `Utils.h` | `set_diff` |
| `Undestructible.h` | 終了時に破棄されないオブジェクトのラッパー |
| `schema/serverconfig.xml` など | 設定オブジェクトのスキーマ（PDB のメンバー配置に合わせて作成） |

### 主な変更 ###

- ゲーム固有の処理を `libhack` 側へ移す形に変更
  - `BaseServer`: 定数の読み込みとアカウントの処理を仮想関数（`InitializeConstants`、`ProcessDataLoadObject`）に分離。`CreateScriptEngine` は派生クラスが実装
  - `PersistentObject`: 型の登録（`Initialize`）を削除し `InitializationFailed` を追加
  - `DataSyncManager`: パケットコードをコンストラクタで受け取る
  - `Database::Setup` / `ApplyMigration`: スクリプトエンジンを引数で受け取る
- 2022 年版で追加されていた関数: `TcpConnection::SendObject/QueueObject`（パケットコード付き）、`Worker::RemoveManager`、`Platform::IsPathSeparator`、`Platform::CreateDirectory`

### 未反映のもの ###

- `sqrat` の 2021〜2022 年の変更（`ClassTypeCasts`、`ObjectReference::IsShared` など）は
  出典が見つからず、2020 年版のままです。サーバー本体のコードはこれらに依存していません。
- `libobjgen` のソースは 2020 年版のままです。コード生成には 2022 年版の
  `comp_objgen.exe`（`deps/objgen/`）を使うため、生成されるコードは配布物と同じです。

## 現在のツールでビルドするための修正 ##

依存ライブラリの zip には手を加えず、展開後に最小限の修正を適用します。

| ファイル | 内容 |
|---|---|
| `cmake/deps/mariadb-patch.cmake` | `END()` を `ENDIF()` に修正（CMake 3.x で必要）、Visual Studio 専用の .pdb コピーを任意に |
| `cmake/deps/civet-fix-warnings.cmake` | 警告をエラー扱いする `/WX` を外す（新しい Windows SDK の警告対策） |
| `cmake/deps/zlib.cmake` | zip からビルドする場合も `ZLIB_LIBRARY` を設定（physfs が自前の zlib を作らないように） |
| `cmake/deps/physfs.cmake` | zlib の後にビルドされるよう依存関係を追加 |
| `windows_build.bat` | MSVC v141、Visual Studio 付属の CMake 3.x、Ninja でビルド |

## 確認したこと ##

- lobby / world / channel の 3 つがビルドでき、既存のサーバー環境（設定・データベース・datastore）で起動し、クライアントで一通りのプレイができることを確認しました。
- channel の起動時の警告・エラーのログが、公式の配布物と一致することを確認しました。
