# COMP_hack（ソース再建版） #

[![AGPL License](http://img.shields.io/badge/license-AGPL-brightgreen.svg)](https://opensource.org/licenses/AGPL-3.0)

真・女神転生IMAGINE（Shin Megami Tensei IMAGINE）のサーバーエミュレーター
[COMP_hack](docs/README.upstream.md) を、**ソースコードから再びビルドできるようにした**
リポジトリです。独自の機能追加も行っています。

*English summary: This is COMP_hack (SMT IMAGINE server emulator) made buildable
again. The original `libcomp`, `sqrat` and dependency repositories are gone, so
they are vendored here (reconstructed where needed). See
[docs/fork/RECONSTRUCTION.md](docs/fork/RECONSTRUCTION.md). Build on Windows
with `windows_build.bat all`.*

## このリポジトリについて ##

- ベース: [HyperChiicken/SMT](https://github.com/HyperChiicken/SMT) の `develop`
  （コミット `95b6ae26`、2022-04-08）。元の COMP_hack の最終版に相当します。
- 元のリポジトリが参照していた `libcomp`・`sqrat`・`datastore` や依存ライブラリの
  取得先は削除されていて、そのままではビルドできませんでした。
  このリポジトリでは、それらを同梱（必要な部分は再建）してビルドできるようにしています。
  経緯と方法は [docs/fork/RECONSTRUCTION.md](docs/fork/RECONSTRUCTION.md) を参照してください。
- ゲームのクライアントやそのデータ（BinaryData など）、サーバー用の
  ゲームデータ（datastore）は含みません。

## 追加した機能 ##

| 機能 | 説明 | 設定 |
|---|---|---|
| 原料タンク自動収納 | 拾った素材を原料タンクへ自動で収納します | `constants.xml` の `AUTO_MATERIAL_TANK`（既定: 無効） |

詳しくは [docs/fork/FEATURES.md](docs/fork/FEATURES.md) を参照してください。
変更履歴は [docs/fork/CHANGELOG.md](docs/fork/CHANGELOG.md) にあります。

## ビルド方法（Windows 64bit） ##

### 必要なもの ###

- **Visual Studio 2022**（Community で可）
  - ワークロード「C++ によるデスクトップ開発」
  - 個別のコンポーネント「MSVC v141 - VS 2017 C++ x64/x86 ビルド ツール」
    （元の配布版と同じコンパイラです）
- **Qt 5**（MSVC 2015 / 2017 64bit 版。例: Qt 5.10.1 の `msvc2015_64`）
  - 既定の場所は `C:\Qt\Qt5.10.1\5.10.1\msvc2015_64` です。別の場所の場合は
    環境変数 `QT_DIR` にそのフォルダを設定してください。
- **Git for Windows**

### 手順 ###

```bat
git clone <このリポジトリのURL> comp_hack
cd comp_hack
git submodule update --init
windows_build.bat all
```

- 初回は依存ライブラリ（`deps/*.zip`）のビルドも行うため時間がかかります。
- できあがったサーバーは `build\bin\` に出力されます:
  `comp_lobby.exe`、`comp_world.exe`、`comp_channel.exe`
- ソースを変更した後は `windows_build.bat build` だけで再ビルドできます。
  特定のものだけ作る場合は `windows_build.bat build comp_channel` のように指定します。

### 既存のサーバーに使う ###

できあがった exe で、既存のサーバーフォルダの同名ファイルを置き換えます
（置き換える前に元のファイルをバックアップしてください）。
設定ファイル（`config\*.xml`）、データベース、datastore はそのまま使えます。
サーバーの設定方法は [Definitive Guide](https://comp-hack.readthedocs.io/en/latest/) を参照してください。

## ライセンス ##

COMP_hack は [GNU Affero General Public License v3](LICENSE.AGPL) で公開されています。
このリポジトリの変更も同じライセンスです。
改造したサーバーを配布したり、ネットワーク経由で他の人に使わせたりする場合は、
利用者がそのサーバーの対応するソースコードを入手できるようにする必要があります。

同梱している依存ライブラリ（`deps/`）は、それぞれのライセンスに従います。

Shin Megami Tensei は株式会社アトラス（旧 Index Corporation）の登録商標です。
このプロジェクトは非公式のものであり、権利者とは関係ありません。
