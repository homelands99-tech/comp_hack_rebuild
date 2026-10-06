# 変更履歴 / Changelog #

新しい変更を上に追記します。

## 2026-09-26 ##

- 公開用に整理（README、ビルド手順、再建の記録）
- サブモジュールの URL を絶対パスに変更
- `windows_build.bat` を追加

## 2026-09-25 ##

- 原料タンク自動収納を追加（`AUTO_MATERIAL_TANK`、既定は無効）
- `libcomp`・`sqrat`・依存ライブラリを同梱し、ソースからビルドできるように再建
  （詳細は [RECONSTRUCTION.md](RECONSTRUCTION.md)）

## ベース ##

- [HyperChiicken/SMT](https://github.com/HyperChiicken/SMT) `develop` のコミット `95b6ae26`（2022-04-08）
